#pragma once

#include <filesystem>

std::filesystem::path ResolveAssetsPath();

std::filesystem::path AssetPath(const char* relative);
