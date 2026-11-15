#include "Logger.hpp"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    std::string Timestamp()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t time = std::chrono::system_clock::to_time_t(now);

        std::tm local{};
#ifdef _WIN32
        const errno_t error = localtime_s(&local, &time);
        if (error != 0)
        {
            std::fill(&local, &local + 1, std::tm{});
        }
#else
        const std::tm* result = std::localtime(&time);
        if (result != nullptr)
        {
            local = *result;
        }
#endif

        std::ostringstream stream;
        stream << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
        return stream.str();
    }

    std::string_view FileName(std::string_view path)
    {
        const std::size_t separator = path.find_last_of("/\\");
        return separator == std::string_view::npos ? path : path.substr(separator + 1);
    }

    const char* LevelLabel(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::Info:    return "INFO ";
            case LogLevel::Warning: return "WARN ";
            case LogLevel::Error:   return "ERROR";
        }

        return "?????";
    }
}

void Logger::Write(
    LogLevel level,
    std::string_view message,
    const std::source_location& location
)
{
    std::ostream& out = level == LogLevel::Error ? std::cerr : std::cout;

    out << '[' << Timestamp() << "] [" << LevelLabel(level) << "] "
        << FileName(location.file_name()) << ':' << location.line() << " — "
        << message << '\n';
}

void Logger::Info(
    std::string_view message,
    const std::source_location& location
)
{
    Write(LogLevel::Info, message, location);
}

void Logger::Warning(
    std::string_view message,
    const std::source_location& location
)
{
    Write(LogLevel::Warning, message, location);
}

void Logger::Error(
    std::string_view message,
    const std::source_location& location
)
{
    Write(LogLevel::Error, message, location);
}
