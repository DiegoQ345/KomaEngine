#include "AssetManager.hpp"

#include "Core/AssetPaths.hpp"
#include "Core/Logger.hpp"

#include <stdexcept>

std::shared_ptr<Shader> AssetManager::LoadShader(
    const std::filesystem::path& vertexPath,
    const std::filesystem::path& fragmentPath
)
{
    const std::filesystem::path vertex = AssetPath(vertexPath.string().c_str());
    const std::filesystem::path fragment = AssetPath(fragmentPath.string().c_str());

    const std::string key = ShaderAssetKey(vertex, fragment);

    const auto cached = m_Resources.find(key);
    if (cached != m_Resources.end())
    {
        return std::static_pointer_cast<Shader>(cached->second);
    }

    auto shader = std::make_shared<Shader>(vertex.string().c_str(), fragment.string().c_str());
    m_Resources[key] = shader;

    return shader;
}

void AssetManager::ReleaseAll()
{
    m_Resources.clear();
}

std::size_t AssetManager::GetResourceCount() const
{
    return m_Resources.size();
}
