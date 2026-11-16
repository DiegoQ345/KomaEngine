#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture2D.hpp"

#include "Core/AssetPaths.hpp"
#include "Core/Logger.hpp"

#include <stdexcept>
#include <string>
#include <filesystem>

Texture2D::Texture2D(const std::filesystem::path& path)
    : Resource(std::filesystem::weakly_canonical(path).string())
{
    const std::string canonical = std::filesystem::weakly_canonical(path).string();

    stbi_set_flip_vertically_on_load(1);

    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_uc* pixels = stbi_load(canonical.c_str(), &width, &height, &channels, 0);

    if (pixels == nullptr)
    {
        const char* reason = stbi_failure_reason();
        Logger::Error(
            std::string("Failed to load image: ") + canonical + " - " +
            (reason ? reason : "unknown reason")
        );
        throw std::runtime_error(std::string("Failed to load image: ") + canonical);
    }

    const GLenum internalFormat = (channels == 1) ? GL_R8 : (channels == 3) ? GL_RGB8 : GL_RGBA8;
    const GLenum sourceFormat = (channels == 1) ? GL_RED : (channels == 3) ? GL_RGB : GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_Texture);
    if (m_Texture == 0)
    {
        stbi_image_free(pixels);
        Logger::Error(std::string("Failed to create OpenGL texture for ") + canonical);
        throw std::runtime_error(std::string("Failed to create OpenGL texture for ") + canonical);
    }

    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        width,
        height,
        0,
        sourceFormat,
        GL_UNSIGNED_BYTE,
        pixels
    );

    stbi_image_free(pixels);

    m_Width = width;
    m_Height = height;
}

Texture2D::~Texture2D()
{
    if (m_Texture != 0)
    {
        glDeleteTextures(1, &m_Texture);
    }
}

glm::ivec2 Texture2D::GetSize() const
{
    return glm::ivec2(m_Width, m_Height);
}

void Texture2D::Bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_Texture);
}
