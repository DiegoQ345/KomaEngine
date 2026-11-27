#pragma once

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
};

class ModelImporter
{
public:
    Model Load(const std::filesystem::path& path);
};
