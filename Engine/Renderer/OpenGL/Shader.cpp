#include "Shader.hpp"

#include "Core/AssetPaths.hpp"
#include "Core/Logger.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
    std::string ReadFile(const char* path)
    {
        std::ifstream file(path);
        if (!file)
        {
            Logger::Error(std::string("Failed to open shader file: ") + path);
            throw std::runtime_error(std::string("Failed to open shader file: ") + path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    GLuint CompileShader(GLenum type, const char* source, const char* label)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE)
        {
            GLint logLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            std::vector<char> log(static_cast<size_t>(logLength));
            glGetShaderInfoLog(shader, logLength, nullptr, log.data());

            Logger::Error(
                std::string("Failed to compile ") + label + ": " + std::string(log.data())
            );

            glDeleteShader(shader);
            throw std::runtime_error(std::string("Failed to compile ") + label);
        }

        return shader;
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
    : Resource(ShaderAssetKey(std::filesystem::path(vertexPath), std::filesystem::path(fragmentPath)))
{
    const std::string vertexSource = ReadFile(vertexPath);
    const std::string fragmentSource = ReadFile(fragmentPath);

    GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSource.c_str(), vertexPath);

    GLuint fragment = 0;
    try
    {
        fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str(), fragmentPath);
    }
    catch (...)
    {
        glDeleteShader(vertex);
        throw;
    }

    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertex);
    glAttachShader(m_Program, fragment);
    glLinkProgram(m_Program);

    GLint success = GL_FALSE;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);

    if (success != GL_TRUE)
    {
        GLint logLength = 0;
        glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> log(static_cast<size_t>(logLength));
        glGetProgramInfoLog(m_Program, logLength, nullptr, log.data());

        Logger::Error(
            std::string("Failed to link shader program (") + vertexPath + ", " + fragmentPath +
            "): " + std::string(log.data())
        );

        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(m_Program);
        m_Program = 0;
        throw std::runtime_error("Failed to link shader program");
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    if (m_Program != 0)
    {
        glDeleteProgram(m_Program);
    }
}

void Shader::Bind() const
{
    glUseProgram(m_Program);
}

void Shader::SetMat4(const char* name, const glm::mat4& matrix) const
{
    const GLint location = glGetUniformLocation(m_Program, name);

    if (location == -1)
    {
        Logger::Warning(std::string("SetMat4: uniform not found: ") + name);
        return;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
