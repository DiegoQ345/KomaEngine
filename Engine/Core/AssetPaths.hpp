#pragma once

#include <filesystem>
#include <string>

std::filesystem::path ResolveAssetsPath();

std::filesystem::path AssetPath(const char* relative);

// Conserva la indentidad normalizada de un Shader (ambas rutas) para claves de cache
std::string ShaderAssetKey(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
