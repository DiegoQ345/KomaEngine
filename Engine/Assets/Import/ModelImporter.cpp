#include "ModelImporter.hpp"

#include "Core/Logger.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <filesystem>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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

    ImportedMaterial ConvertMaterial(
        const aiMaterial* material,
        const std::filesystem::path& modelDirectory
    )
    {
        ImportedMaterial result;

        aiString name;
        if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
        {
            result.Name = name.C_Str();
        }

        aiColor3D diffuse;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
        {
            result.DiffuseColor = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        }

        float metallic = 0.0f;
        float roughness = 0.5f;
        const bool hasMetallic = material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS;
        const bool hasRoughness = material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS;

        result.HasPBR = hasMetallic && hasRoughness;
        result.Metallic = hasMetallic ? glm::clamp(metallic, 0.0f, 1.0f) : 0.0f;
        result.Roughness = hasRoughness ? glm::clamp(roughness, 0.04f, 1.0f) : 0.5f;

        aiString texturePath;
        const unsigned int count = material->GetTextureCount(aiTextureType_DIFFUSE);
        if (count > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            const std::string raw = texturePath.C_Str();

            if (!raw.empty() && raw[0] == '*')
            {
                Logger::Warning("ModelImporter: embedded diffuse textures are not supported: " + result.Name);
            }
            else if (!raw.empty())
            {
                std::filesystem::path path(raw);
                path.make_preferred();

                if (!path.is_absolute())
                {
                    path = modelDirectory / path;
                }

                std::error_code error;
                const std::filesystem::path canonical = std::filesystem::weakly_canonical(path, error);

                if (!error && std::filesystem::exists(canonical))
                {
                    result.TexturePath = canonical;
                }
                else
                {
                    Logger::Warning("ModelImporter: diffuse texture not found: " + path.string());
                }
            }
        }

        return result;
    }

    std::shared_ptr<Mesh> ConvertMesh(const aiMesh* mesh)
    {
        if (mesh == nullptr || !mesh->HasPositions() || mesh->mNumVertices == 0)
        {
            return nullptr;
        }

        bool valid = true;
        std::vector<Mesh::Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            const aiVector3D& source = mesh->mVertices[i];
            const glm::vec3 position(source.x, source.y, source.z);

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
            normal = glm::normalize(normal);
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
            indices.push_back(b);
            indices.push_back(c);
        }

        if (indices.empty())
        {
            return nullptr;
        }

        return std::make_shared<Mesh>(vertices, indices);
    }

    ModelNode TraverseNode(
        const aiNode* node,
        const aiScene* scene,
        Model& model,
        std::vector<std::size_t>& meshMap
    )
    {
        ModelNode result;

        if (node == nullptr) return result;

        if (node->mName.length > 0)
        {
            result.Name = node->mName.C_Str();
        }

        result.LocalTransform = ToGLM(node->mTransformation);

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            const unsigned int sourceIndex = node->mMeshes[i];
            if (sourceIndex >= scene->mNumMeshes) continue;

            std::size_t modelIndex = meshMap[sourceIndex];
            if (modelIndex == std::numeric_limits<std::size_t>::max())
            {
                std::shared_ptr<Mesh> mesh = ConvertMesh(scene->mMeshes[sourceIndex]);
                if (!mesh) continue;

                std::size_t materialIndex = scene->mMeshes[sourceIndex]->mMaterialIndex;
                if (materialIndex >= model.Materials.size())
                {
                    Logger::Warning("ModelImporter: invalid material index, using first material");
                    materialIndex = 0;
                }

                modelIndex = model.Meshes.size();
                model.Meshes.push_back(ModelMesh{std::move(mesh), materialIndex});
                meshMap[sourceIndex] = modelIndex;
            }

            result.MeshIndices.push_back(modelIndex);
        }

        result.Children.reserve(node->mNumChildren);
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            result.Children.push_back(TraverseNode(node->mChildren[i], scene, model, meshMap));
        }

        return result;
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

    const std::filesystem::path modelDirectory = std::filesystem::weakly_canonical(path).parent_path();

    model.Materials.reserve(scene->mNumMaterials);
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        model.Materials.push_back(ConvertMaterial(scene->mMaterials[i], modelDirectory));
    }

    if (model.Materials.empty())
    {
        model.Materials.push_back(ImportedMaterial{});
    }

    std::vector<std::size_t> meshMap(
        scene->mNumMeshes,
        std::numeric_limits<std::size_t>::max()
    );
    model.RootNode = TraverseNode(scene->mRootNode, scene, model, meshMap);

    if (model.Meshes.empty())
    {
        Logger::Error("ModelImporter: model contains no valid meshes: " + path.string());
        throw std::runtime_error("ModelImporter: model contains no valid meshes");
    }

    return model;
}
