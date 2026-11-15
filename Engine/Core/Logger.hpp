#pragma once

#include <string_view>
#include <source_location>

enum class LogLevel {
    Info,
    Warning,
    Error
};

class Logger
{
public:
    static void Info(
        std::string_view message,
        const std::source_location& location = std::source_location::current()
    );
    static void Warning(
        std::string_view message,
        const std::source_location& location = std::source_location::current()
    );
    static void Error(
        std::string_view message,
        const std::source_location& location = std::source_location::current()
    );

private:
    static void Write(
        LogLevel level,
        std::string_view message,
        const std::source_location& location
    );
};
