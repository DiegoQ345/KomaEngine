#include "AssetManager.hpp"

#include "Core/AssetPaths.hpp"
#include "Core/Logger.hpp"

#include <stdexcept>
#include <string>
#include <utility>

std::shared_ptr<Shader> AssetManager::LoadShader(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath
)
{
    const std::filesystem::path vertex = AssetPath(vertexPath.string().c_str());
    const std::filesystem::path fragment = AssetPath(fragmentPath.string().c_str());

    const std::string key = ShaderAssetKey(vertex, fragment);

    return LoadCached<Shader>(key, [&] {
        return std::make_shared<Shader>(vertex.string().c_str(), fragment.string().c_str());
    });
}

std::shared_ptr<Texture2D> AssetManager::LoadTexture2D(const std::filesystem::path& path)
{
    const std::filesystem::path resolved = AssetPath(path.string().c_str());
    const std::string key = "texture|" + std::filesystem::weakly_canonical(resolved).string();

    return LoadCached<Texture2D>(key, [&] {
        return std::make_shared<Texture2D>(resolved);
    });
}

void AssetManager::ReleaseAll()
{
    m_Resources.clear();
}

std::size_t AssetManager::GetResourceCount() const
{
    return m_Resources.size();
}
