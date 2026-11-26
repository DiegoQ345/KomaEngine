#pragma once

#include "Renderer/OpenGL/Mesh.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

struct Model
{
    std::string Name;
    std::vector<std::shared_ptr<Mesh>> Meshes;
};

class ModelImporter
{
public:
    Model Load(const std::filesystem::path& path);
};
