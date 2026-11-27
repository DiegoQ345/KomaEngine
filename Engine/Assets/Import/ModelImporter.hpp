#pragma once

#include "Renderer/OpenGL/Mesh.hpp"

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
};

struct ModelMesh
{
    std::shared_ptr<Mesh> Mesh;
    std::size_t MaterialIndex = 0;
};

struct Model
{
    std::string Name;
    std::vector<ModelMesh> Meshes;
    std::vector<ImportedMaterial> Materials;
};

class ModelImporter
{
public:
    Model Load(const std::filesystem::path& path);
};
