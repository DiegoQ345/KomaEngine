#pragma once

#include "Assets/Skeleton.hpp"
#include "Renderer/OpenGL/Mesh.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <cstddef>

struct ImportedMaterial
{
    std::string Name;
    glm::vec3 DiffuseColor{1.0f};
    std::filesystem::path TexturePath;
    std::filesystem::path NormalMapPath;
    bool HasTangents = false;
    bool HasPBR = false;
    float Metallic = 0.0f;
    float Roughness = 0.5f;
};

struct ModelMesh
{
    std::shared_ptr<Mesh> Mesh;
    std::size_t MaterialIndex = 0;
    /// Per aiMesh bone list in skin order; empty if not skinned.
    /// Inverse bind matrices come from aiBone::mOffsetMatrix (converted via ToGLM), stored per mesh.
    std::vector<KomaEngine::BoneBinding> BoneBindings;
    /// Per-vertex skin influences aligned with Mesh vertices (index v == aiMesh vertex index).
    /// Empty if the mesh has no skin data imported.
    std::vector<std::vector<KomaEngine::SkinVertexInfluence>> SkinWeights;
};

struct ModelNode
{
    std::string Name;
    glm::mat4 LocalTransform{1.0f};
    std::vector<std::size_t> MeshIndices;
    std::vector<ModelNode> Children;
};

struct Model
{
    std::string Name;
    std::vector<ModelMesh> Meshes;
    std::vector<ImportedMaterial> Materials;
    ModelNode RootNode;
    /// Flat list of all ModelNode instances in traversal order (index = stable node id for bones).
    std::vector<std::shared_ptr<ModelNode>> NodeTable;
    /// Optional; null when the model has no skeleton.
    std::shared_ptr<KomaEngine::Skeleton> SkeletonData;
};

class ModelImporter
{
public:
    Model Load(const std::filesystem::path& path);
};
