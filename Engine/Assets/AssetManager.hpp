#pragma once

#include "Assets/Resource.hpp"
#include "Renderer/OpenGL/Shader.hpp"
#include "Renderer/OpenGL/Texture2D.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <cstddef>

class AssetManager
{
public:
    std::shared_ptr<Shader> LoadShader(
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath
    );

    std::shared_ptr<Texture2D> LoadTexture2D(
        const std::filesystem::path& path,
        bool sRGB = true
    );

    void ReleaseAll();

    std::size_t GetResourceCount() const;

private:
    template <typename T, typename Factory>
    std::shared_ptr<T> LoadCached(const std::string& key, Factory&& create)
    {
        if (const auto cached = m_Resources.find(key); cached != m_Resources.end())
        {
            return std::static_pointer_cast<T>(cached->second);
        }

        std::shared_ptr<T> resource = create();
        m_Resources[key] = resource;
        return resource;
    }

    std::unordered_map<std::string, std::shared_ptr<Resource>> m_Resources;
};
