#include "ModelImporter.hpp"

#include "Core/Logger.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>

namespace
{
    glm::mat4 ToGLM(const aiMatrix4x4& matrix)
    {
        return glm::transpose(glm::make_mat4(&matrix.a1));
    }

    bool IsFinite(const glm::vec3& value)
    {
        return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
    }

    std::shared_ptr<Mesh> ConvertMesh(
        const aiMesh* mesh,
        const glm::mat4& transform
    )
    {
        if (mesh == nullptr || !mesh->HasPositions() || mesh->mNumVertices == 0)
        {
            return nullptr;
        }

        const glm::mat3 transform3(transform);
        const float determinant = glm::determinant(transform3);
        const glm::mat3 normalMatrix =
            std::fabs(determinant) > 1e-6f ? glm::transpose(glm::inverse(transform3)) : glm::mat3(1.0f);

        bool valid = true;
        std::vector<Mesh::Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            const aiVector3D& source = mesh->mVertices[i];
            const glm::vec3 position = glm::vec3(transform * glm::vec4(source.x, source.y, source.z, 1.0f));

            if (!IsFinite(position))
            {
                valid = false;
                break;
            }

            glm::vec3 normal(
                mesh->HasNormals() ? mesh->mNormals[i].x : 0.0f,
                mesh->HasNormals() ? mesh->mNormals[i].y : 0.0f,
                mesh->HasNormals() ? mesh->mNormals[i].z : 1.0f
            );
            normal = glm::normalize(normalMatrix * normal);
            if (!IsFinite(normal))
            {
                normal = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            glm::vec2 uv(0.0f);
            if (mesh->HasTextureCoords(0))
            {
                uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }

            vertices.push_back(Mesh::Vertex{
                position.x, position.y, position.z,
                normal.x, normal.y, normal.z,
                uv.x, uv.y
            });
        }

        if (!valid)
        {
            Logger::Error("ModelImporter: non-finite vertex data");
            return nullptr;
        }

        std::vector<unsigned int> indices;
        indices.reserve(static_cast<size_t>(mesh->mNumFaces) * 3);

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            if (face.mNumIndices != 3)
            {
                Logger::Warning("ModelImporter: non-triangular face skipped");
                continue;
            }

            const unsigned int a = face.mIndices[0];
            const unsigned int b = face.mIndices[1];
            const unsigned int c = face.mIndices[2];

            if (a >= mesh->mNumVertices || b >= mesh->mNumVertices || c >= mesh->mNumVertices)
            {
                Logger::Error("ModelImporter: face index out of range");
                return nullptr;
            }

            indices.push_back(a);
            indices.push_back(determinant < 0.0f ? c : b);
            indices.push_back(determinant < 0.0f ? b : c);
        }

        if (indices.empty())
        {
            return nullptr;
        }

        return std::make_shared<Mesh>(vertices, indices);
    }

    void TraverseNode(
        const aiNode* node,
        const aiScene* scene,
        const glm::mat4& parentTransform,
        Model& model
    )
    {
        if (node == nullptr) return;

        const glm::mat4 transform = parentTransform * ToGLM(node->mTransformation);

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            const unsigned int meshIndex = node->mMeshes[i];
            if (meshIndex >= scene->mNumMeshes) continue;

            std::shared_ptr<Mesh> mesh = ConvertMesh(scene->mMeshes[meshIndex], transform);
            if (mesh)
            {
                model.Meshes.push_back(std::move(mesh));
            }
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            TraverseNode(node->mChildren[i], scene, transform, model);
        }
    }
}

Model ModelImporter::Load(const std::filesystem::path& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path.string(),
        aiProcess_Triangulate
        | aiProcess_GenNormals
        | aiProcess_JoinIdenticalVertices
        | aiProcess_ValidateDataStructure
    );

    if (scene == nullptr || scene->mRootNode == nullptr || scene->mNumMeshes == 0)
    {
        Logger::Error(std::string("ModelImporter: failed to load model: ") + path.string() +
            " - " + importer.GetErrorString());
        throw std::runtime_error("ModelImporter: failed to load model: " + path.string());
    }

    Model model;
    model.Name = path.stem().string();

    TraverseNode(scene->mRootNode, scene, glm::mat4(1.0f), model);

    if (model.Meshes.empty())
    {
        Logger::Error("ModelImporter: model contains no valid meshes: " + path.string());
        throw std::runtime_error("ModelImporter: model contains no valid meshes");
    }

    return model;
}
