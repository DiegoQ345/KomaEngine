#include "AssetPaths.hpp"

#include "Core/Logger.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    std::filesystem::path ExecutablePath()
    {
#ifdef _WIN32
        std::vector<wchar_t> buffer(MAX_PATH);
        DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        while (length == buffer.size())
        {
            buffer.resize(buffer.size() * 2);
            length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        }
        if (length == 0)
        {
            Logger::Error("GetModuleFileNameW failed to resolve the executable path");
            throw std::runtime_error("GetModuleFileNameW failed to resolve the executable path");
        }

        return std::filesystem::path(buffer.data());
#else
        return std::filesystem::canonical("/proc/self/exe");
#endif
    }

    std::filesystem::path FindAssetsDirectory(const std::filesystem::path& startDirectory)
    {
        std::filesystem::path current = std::filesystem::absolute(startDirectory);

        while (true)
        {
            const std::filesystem::path candidate = current / "Assets";
            if (std::filesystem::is_directory(candidate))
            {
                return candidate;
            }

            const std::filesystem::path parent = current.parent_path();
            if (parent == current)
            {
                Logger::Error(
                    std::string("Could not locate the Assets directory relative to the executable: ") +
                    startDirectory.string()
                );
                throw std::runtime_error("Could not locate the Assets directory relative to the executable");
            }

            current = parent;
        }
    }
}

std::filesystem::path ResolveAssetsPath()
{
    return FindAssetsDirectory(ExecutablePath().parent_path());
}

std::filesystem::path AssetPath(const char* relative)
{
    std::filesystem::path path = ResolveAssetsPath() / relative;

    if (!std::filesystem::exists(path))
    {
        Logger::Error(std::string("Asset not found: ") + path.string());
        throw std::runtime_error(std::string("Asset not found: ") + path.string());
    }

    return path;
}
