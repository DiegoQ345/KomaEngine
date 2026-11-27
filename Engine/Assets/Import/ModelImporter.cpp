#include "ModelImporter.hpp"

#include "Core/Logger.hpp"

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <functional>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace
{
    constexpr float kSkinWeightMinEps = 1e-6f;
    constexpr float kSkinWeightSumTol = 1e-3f;

    glm::mat4 ToGLM(const aiMatrix4x4& matrix)
    {
        return glm::transpose(glm::make_mat4(&matrix.a1));
    }

    bool IsFinite(const glm::vec3& value)
    {
        return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
    }

    bool MatricesNearlyEqual(const glm::mat4& a, const glm::mat4& b, float eps)
    {
        for (int c = 0; c < 4; ++c)
        {
            for (int r = 0; r < 4; ++r)
            {
                if (std::fabs(a[c][r] - b[c][r]) > eps)
                    return false;
            }
        }
        return true;
    }

    ImportedMaterial ConvertMaterial(
        const aiMaterial* material,
        const std::filesystem::path& modelDirectory)
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

        aiString normalPath;
        const unsigned int normalCount = material->GetTextureCount(aiTextureType_NORMALS);
        if (normalCount > 0 &&
            material->GetTexture(aiTextureType_NORMALS, 0, &normalPath) == AI_SUCCESS)
        {
            const std::string raw = normalPath.C_Str();

            if (!raw.empty() && raw[0] == '*')
            {
                Logger::Warning("ModelImporter: embedded normal textures are not supported: " + result.Name);
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
                    result.NormalMapPath = canonical;
                }
                else
                {
                    Logger::Warning("ModelImporter: normal texture not found: " + path.string());
                }
            }
        }

        return result;
    }

    std::shared_ptr<Mesh> ConvertMesh(const aiMesh* mesh, bool& hasTangents)
    {
        hasTangents = false;

        if (mesh == nullptr || !mesh->HasPositions() || mesh->mNumVertices == 0)
        {
            return nullptr;
        }

        const bool importTangents =
            mesh->HasTangentsAndBitangents() && mesh->HasNormals() && mesh->HasTextureCoords(0);
        hasTangents = importTangents;

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
                mesh->HasNormals() ? mesh->mNormals[i].z : 1.0f);
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

            glm::vec4 tangent(1.0f, 0.0f, 0.0f, 1.0f);
            if (importTangents)
            {
                const aiVector3D& t = mesh->mTangents[i];
                const aiVector3D& b = mesh->mBitangents[i];

                const glm::vec3 n = normal;
                const glm::vec3 tv(t.x, t.y, t.z);
                const glm::vec3 bv(b.x, b.y, b.z);
                const float handedness = glm::dot(glm::cross(n, tv), bv);
                const float w = handedness >= 0.0f ? 1.0f : -1.0f;

                tangent = glm::vec4(tv, w);
            }

            vertices.push_back(Mesh::Vertex{position.x, position.y, position.z,
                                            normal.x, normal.y, normal.z,
                                            uv.x, uv.y,
                                            tangent.x, tangent.y, tangent.z, tangent.w});
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

    struct PendingBoneInfo
    {
        std::string name;
        glm::mat4 offsetMatrix{1.0f};
        bool hasOffset = false;
        const aiNode* node = nullptr;
        std::size_t modelNodeIndex = KomaEngine::kInvalidModelNodeIndex;
        unsigned int sourceBoneCount = 0;
    };

    struct NodeBuildContext
    {
        const aiScene* scene = nullptr;
        Model* model = nullptr;
        std::vector<std::size_t>* meshMap = nullptr;
        std::unordered_map<const aiNode*, std::size_t>* aiNodeToModelIndex = nullptr;
    };

    ModelNode TraverseNodeBuild(
        const aiNode* node,
        NodeBuildContext& ctx)
    {
        ModelNode result;
        if (node == nullptr)
            return result;

        const std::size_t thisModelIndex = ctx.model->NodeTable.size();
        ctx.model->NodeTable.push_back(nullptr);
        (*ctx.aiNodeToModelIndex)[node] = thisModelIndex;

        if (node->mName.length > 0)
        {
            result.Name = node->mName.C_Str();
        }
        result.LocalTransform = ToGLM(node->mTransformation);

        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            const unsigned int sourceIndex = node->mMeshes[i];
            if (sourceIndex >= ctx.scene->mNumMeshes)
                continue;

            std::size_t modelIndex = (*ctx.meshMap)[sourceIndex];
            if (modelIndex == std::numeric_limits<std::size_t>::max())
            {
                bool hasTangents = false;
                std::shared_ptr<Mesh> mesh = ConvertMesh(ctx.scene->mMeshes[sourceIndex], hasTangents);
                if (!mesh)
                    continue;

                std::size_t materialIndex = ctx.scene->mMeshes[sourceIndex]->mMaterialIndex;
                if (materialIndex >= ctx.model->Materials.size())
                {
                    Logger::Warning("ModelImporter: invalid material index, using first material");
                    materialIndex = 0;
                }

                if (hasTangents)
                {
                    ctx.model->Materials[materialIndex].HasTangents = true;
                }

                modelIndex = ctx.model->Meshes.size();
                ctx.model->Meshes.push_back(ModelMesh{std::move(mesh), materialIndex, {}});
                (*ctx.meshMap)[sourceIndex] = modelIndex;
            }

            result.MeshIndices.push_back(modelIndex);
        }

        result.Children.reserve(node->mNumChildren);
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            result.Children.push_back(TraverseNodeBuild(node->mChildren[i], ctx));
        }

        ctx.model->NodeTable[thisModelIndex] = std::make_shared<ModelNode>(result);
        return result;
    }
} // namespace

Model ModelImporter::Load(const std::filesystem::path& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path.string(),
        aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace |
            aiProcess_ValidateDataStructure |
            aiProcess_PopulateArmatureData);

    if (scene == nullptr || scene->mRootNode == nullptr || scene->mNumMeshes == 0)
    {
        Logger::Error(
            std::string("ModelImporter: failed to load model: ") + path.string() + " - " + importer.GetErrorString());
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

    std::vector<std::size_t> meshMap(scene->mNumMeshes, std::numeric_limits<std::size_t>::max());
    std::unordered_map<const aiNode*, std::size_t> aiNodeToModelIndex;
    NodeBuildContext ctx;
    ctx.scene = scene;
    ctx.model = &model;
    ctx.meshMap = &meshMap;
    ctx.aiNodeToModelIndex = &aiNodeToModelIndex;
    model.RootNode = TraverseNodeBuild(scene->mRootNode, ctx);

    if (model.Meshes.empty())
    {
        Logger::Error("ModelImporter: model contains no valid meshes: " + path.string());
        throw std::runtime_error("ModelImporter: model contains no valid meshes");
    }

    std::vector<PendingBoneInfo> pendingBones;
    std::unordered_map<std::string, std::size_t> boneNameToPending;
    std::unordered_set<std::string> boneNameSet;
    unsigned int meshesWithSkin = 0;

    for (unsigned int mi = 0; mi < scene->mNumMeshes; mi++)
    {
        const aiMesh* aMesh = scene->mMeshes[mi];
        if (!aMesh || aMesh->mNumBones == 0)
            continue;
        meshesWithSkin++;
        for (unsigned int bi = 0; bi < aMesh->mNumBones; bi++)
        {
            const aiBone* bone = aMesh->mBones[bi];
            std::string bname = bone->mName.C_Str() ? bone->mName.C_Str() : std::string();
            if (bname.empty())
            {
                Logger::Warning("ModelImporter: bone with empty name in mesh index " + std::to_string(mi));
                continue;
            }
            auto it = boneNameToPending.find(bname);
            if (it == boneNameToPending.end())
            {
                PendingBoneInfo pb;
                pb.name = bname;
                pb.offsetMatrix = ToGLM(bone->mOffsetMatrix);
                pb.hasOffset = true;
                pb.node = nullptr;
                if (bone->mNode)
                    pb.node = bone->mNode;
                pb.sourceBoneCount = 1;
                const std::size_t idx = pendingBones.size();
                pendingBones.push_back(std::move(pb));
                boneNameToPending[bname] = idx;
                boneNameSet.insert(bname);
            }
            else
            {
                PendingBoneInfo& pb = pendingBones[it->second];
                pb.sourceBoneCount++;
                const glm::mat4 om = ToGLM(bone->mOffsetMatrix);
                if (!MatricesNearlyEqual(pb.offsetMatrix, om, 1e-5f))
                {
                    Logger::Warning(
                        "ModelImporter: inconsistent mOffsetMatrix for bone '" + bname +
                        "' across meshes (keeping first seen; per-mesh bindings store each mesh aiBone offset)");
                }
                if (pb.node == nullptr && bone->mNode != nullptr)
                    pb.node = bone->mNode;
            }
        }
    }

    if (!pendingBones.empty())
    {
        std::unordered_map<std::string, int> nodeNameCounts;
        std::function<void(const aiNode*)> countNodesFn = [&](const aiNode* nd) {
            if (!nd)
                return;
            if (nd->mName.length > 0)
                nodeNameCounts[std::string(nd->mName.C_Str())]++;
            for (unsigned int c = 0; c < nd->mNumChildren; ++c)
                countNodesFn(nd->mChildren[c]);
        };
        countNodesFn(scene->mRootNode);

        int ambiguousNames = 0;
        for (const auto& bn : boneNameSet)
        {
            auto cn = nodeNameCounts.find(bn);
            if (cn != nodeNameCounts.end() && cn->second > 1)
            {
                Logger::Warning("ModelImporter: duplicate aiNode name in scene for bone '" + bn +
                                "' (resolved via aiBone->mNode mapping, not by name search)");
                ambiguousNames++;
            }
        }
        if (ambiguousNames > 0)
        {
            Logger::Warning("ModelImporter: " + std::to_string(ambiguousNames) +
                            " bone name(s) had duplicate node names in tree; association uses aiBone->mNode when available");
        }

        int bonesNoNode = 0;
        for (auto& pb : pendingBones)
        {
            if (pb.node == nullptr)
            {
                Logger::Warning("ModelImporter: bone has no associated aiNode pointer: '" + pb.name + "'");
                bonesNoNode++;
                pb.modelNodeIndex = KomaEngine::kInvalidModelNodeIndex;
            }
            else
            {
                auto nit = aiNodeToModelIndex.find(pb.node);
                if (nit == aiNodeToModelIndex.end())
                {
                    Logger::Warning("ModelImporter: aiNode for bone '" + pb.name + "' not found in ModelNode table");
                    pb.modelNodeIndex = KomaEngine::kInvalidModelNodeIndex;
                    bonesNoNode++;
                }
                else
                {
                    pb.modelNodeIndex = nit->second;
                }
            }
        }

        auto skeleton = std::make_shared<KomaEngine::Skeleton>();
        skeleton->bones.resize(pendingBones.size());
        for (std::size_t i = 0; i < pendingBones.size(); ++i)
        {
            skeleton->bones[i].name = pendingBones[i].name;
            skeleton->bones[i].modelNodeIndex = pendingBones[i].modelNodeIndex;
            skeleton->bones[i].parentBoneIndex = -1;
            if (pendingBones[i].node != nullptr)
            {
                skeleton->bones[i].localBindTransform = ToGLM(pendingBones[i].node->mTransformation);
            }
            else
            {
                skeleton->bones[i].localBindTransform = glm::mat4(1.0f);
            }
        }
        for (std::size_t i = 0; i < pendingBones.size(); ++i)
        {
            skeleton->nameToIndex[pendingBones[i].name] = i;
        }
        for (std::size_t i = 0; i < pendingBones.size(); ++i)
        {
            if (pendingBones[i].node == nullptr)
            {
                skeleton->bones[i].parentBoneIndex = -1;
                continue;
            }
            const aiNode* parent = pendingBones[i].node->mParent;
            int foundParent = -1;
            while (parent != nullptr)
            {
                std::string pname = parent->mName.C_Str() ? parent->mName.C_Str() : std::string();
                if (!pname.empty())
                {
                    auto bit = skeleton->nameToIndex.find(pname);
                    if (bit != skeleton->nameToIndex.end())
                    {
                        foundParent = static_cast<int>(bit->second);
                        break;
                    }
                }
                parent = parent->mParent;
            }
            skeleton->bones[i].parentBoneIndex = foundParent;
        }

        model.SkeletonData = std::move(skeleton);

        std::size_t totalBindings = 0;
        std::size_t meshesWithSkinWeights = 0;
        std::size_t totalInfluencesImported = 0;
        std::size_t totalInfluencesDiscardedInvalid = 0;
        std::size_t totalInfluencesDiscardedZeroEps = 0;
        std::size_t totalInfluencesDiscardedNonFinite = 0;
        std::size_t totalInfluencesDiscardedOob = 0;
        std::size_t totalInfluencesDiscardedBadBone = 0;
        std::size_t totalVertsWithNoInfluences = 0;
        std::size_t maxInfluencesPerVertex = 0;
        std::size_t sumSquaresForDebug = 0; // not used

        for (unsigned int mi = 0; mi < scene->mNumMeshes; mi++)
        {
            const std::size_t modelMeshIdx = meshMap[mi];
            if (modelMeshIdx == std::numeric_limits<std::size_t>::max())
                continue;
            const aiMesh* aMesh = scene->mMeshes[mi];
            ModelMesh& mm = model.Meshes[modelMeshIdx];
            mm.BoneBindings.clear();
            mm.SkinWeights.clear();
            if (!aMesh)
                continue;

            if (aMesh->mNumBones == 0)
            {
                mm.SkinWeights.resize(aMesh->mNumVertices);
                continue;
            }

            meshesWithSkinWeights++;
            mm.SkinWeights.assign(aMesh->mNumVertices, {});

            for (unsigned int bi = 0; bi < aMesh->mNumBones; bi++)
            {
                const aiBone* ab = aMesh->mBones[bi];
                std::string bname = ab->mName.C_Str() ? ab->mName.C_Str() : std::string();
                KomaEngine::BoneBinding bb;
                auto bit = model.SkeletonData->nameToIndex.find(bname);
                if (bit == model.SkeletonData->nameToIndex.end())
                {
                    Logger::Warning("ModelImporter: bone not found in skeleton map: '" + bname + "'");
                    continue;
                }
                const std::size_t boneGlobal = bit->second;
                bb.skeletonBoneIndex = boneGlobal;
                bb.inverseBindMatrix = ToGLM(ab->mOffsetMatrix);
                mm.BoneBindings.push_back(bb);

                for (unsigned int wi = 0; wi < ab->mNumWeights; wi++)
                {
                    const aiVertexWeight& vw = ab->mWeights[wi];
                    if (vw.mVertexId >= aMesh->mNumVertices)
                    {
                        totalInfluencesDiscardedOob++;
                        totalInfluencesDiscardedInvalid++;
                        continue;
                    }
                    if (!std::isfinite(vw.mWeight))
                    {
                        totalInfluencesDiscardedNonFinite++;
                        totalInfluencesDiscardedInvalid++;
                        continue;
                    }
                    if (vw.mWeight <= kSkinWeightMinEps)
                    {
                        totalInfluencesDiscardedZeroEps++;
                        totalInfluencesDiscardedInvalid++;
                        continue;
                    }
                    if (boneGlobal >= model.SkeletonData->bones.size())
                    {
                        totalInfluencesDiscardedBadBone++;
                        totalInfluencesDiscardedInvalid++;
                        continue;
                    }
                    mm.SkinWeights[vw.mVertexId].push_back(
                        KomaEngine::SkinVertexInfluence{boneGlobal, vw.mWeight});
                    totalInfluencesImported++;
                }
            }

            // Normalize per vertex
            for (std::size_t vi = 0; vi < mm.SkinWeights.size(); vi++)
            {
                auto& infl = mm.SkinWeights[vi];
                if (infl.empty())
                {
                    totalVertsWithNoInfluences++;
                    continue;
                }
                if (infl.size() > maxInfluencesPerVertex)
                    maxInfluencesPerVertex = infl.size();
                float sumW = 0.0f;
                for (const auto& inf : infl)
                    sumW += inf.weight;
                if (!std::isfinite(sumW) || sumW <= 0.0f)
                {
                    totalVertsWithNoInfluences++;
                    infl.clear();
                    continue;
                }
                if (std::fabs(sumW - 1.0f) > kSkinWeightSumTol)
                {
                    const float inv = 1.0f / sumW;
                    for (auto& inf : infl)
                        inf.weight *= inv;
                }
            }
        }

        Logger::Info(
            "Skeleton imported: bones=" + std::to_string(model.SkeletonData->bones.size()) +
            " meshesWithSkin=" + std::to_string(meshesWithSkin));
        if (bonesNoNode > 0)
        {
            Logger::Warning("ModelImporter: bones without associated ModelNode (invalid index): " + std::to_string(bonesNoNode));
        }
        Logger::Info("Skeleton bone bindings imported (total entries across skinned model meshes): " + std::to_string(totalBindings));
        Logger::Info(
            "SkinWeights (diagnostic): meshesWithSkinWeights=" + std::to_string(meshesWithSkinWeights) +
            " totalInfluencesImported(raw,after filters)=" + std::to_string(totalInfluencesImported) +
            " discardedInvalid=" + std::to_string(totalInfluencesDiscardedInvalid) +
            " (oob=" + std::to_string(totalInfluencesDiscardedOob) +
            " nonFinite=" + std::to_string(totalInfluencesDiscardedNonFinite) +
            " <=eps=" + std::to_string(totalInfluencesDiscardedZeroEps) +
            " badBone=" + std::to_string(totalInfluencesDiscardedBadBone) + ")" +
            " verticesWithNoInfluences=" + std::to_string(totalVertsWithNoInfluences) +
            " maxInfluencesPerVertex=" + std::to_string(maxInfluencesPerVertex));
    }
    else
    {
        model.SkeletonData.reset();
        for (auto& mm : model.Meshes)
            mm.SkinWeights.resize(mm.Mesh ? 0 : 0); // keep empty if no mesh; but Mesh exists; set to empty vectors per vertex not needed
    }

    return model;
}
