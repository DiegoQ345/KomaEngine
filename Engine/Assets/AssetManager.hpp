#pragma once

#include "Assets/Resource.hpp"
#include "Renderer/OpenGL/Shader.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <cstddef>

class AssetManager
{
public:
    std::shared_ptr<Shader> LoadShader(
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath
    );

    void ReleaseAll();

    std::size_t GetResourceCount() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_Resources;
};
