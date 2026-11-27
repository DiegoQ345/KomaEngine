#include "CubeMapTexture.hpp"

#include "Core/Logger.hpp"

#include <stb_image.h>

#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
    std::string BuildId(const CubeMapTexture::Faces& faces)
    {
        std::string id = "cubemap|";
        for (const auto& face : faces)
        {
            id += std::filesystem::weakly_canonical(face).string() + "|";
        }
        return id;
    }

    float SrgbToLinear(unsigned char value)
    {
        const float channel = value / 255.0f;
        return channel <= 0.04045f
            ? channel / 12.92f
            : std::pow((channel + 0.055f) / 1.055f, 2.4f);
    }
}

CubeMapTexture::CubeMapTexture(const Faces& faces)
    : Resource(BuildId(faces))
{
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);

    int expectedWidth = 0;
    int expectedHeight = 0;
    int expectedChannels = 0;

    glm::vec3 ambientSum(0.0f);
    unsigned long long totalPixels = 0;

    for (std::size_t face = 0; face < faces.size(); face++)
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        stbi_uc* pixels = stbi_load(faces[face].string().c_str(), &width, &height, &channels, 0);

        if (pixels == nullptr)
        {
            Logger::Error(std::string("CubeMapTexture: failed to load face: ") + faces[face].string());
            glDeleteTextures(1, &m_Texture);
            m_Texture = 0;
            throw std::runtime_error("CubeMapTexture: failed to load face");
        }

        if (face == 0)
        {
            expectedWidth = width;
            expectedHeight = height;
            expectedChannels = channels;
        }
        else if (width != expectedWidth || height != expectedHeight || channels != expectedChannels)
        {
            Logger::Error("CubeMapTexture: incompatible face dimensions or channels");
            stbi_image_free(pixels);
            glDeleteTextures(1, &m_Texture);
            m_Texture = 0;
            throw std::runtime_error("CubeMapTexture: incompatible faces");
        }

        const GLenum internalFormat = channels == 3 ? GL_SRGB8 : GL_SRGB8_ALPHA8;
        const GLenum sourceFormat = channels == 3 ? GL_RGB : GL_RGBA;

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(face),
            0,
            internalFormat,
            width,
            height,
            0,
            sourceFormat,
            GL_UNSIGNED_BYTE,
            pixels
        );

        for (int i = 0; i < width * height; i++)
        {
            const stbi_uc* pixel = pixels + static_cast<std::size_t>(i) * channels;
            ambientSum += glm::vec3(
                SrgbToLinear(pixel[0]),
                SrgbToLinear(pixel[1]),
                SrgbToLinear(pixel[2])
            );
        }
        totalPixels += static_cast<unsigned long long>(width) * height;

        stbi_image_free(pixels);
    }

    if (totalPixels > 0)
    {
        m_AmbientColor = ambientSum / static_cast<float>(totalPixels);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMapTexture::~CubeMapTexture()
{
    if (m_Texture != 0)
    {
        glDeleteTextures(1, &m_Texture);
    }
}

void CubeMapTexture::Bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_Texture);
}

const glm::vec3& CubeMapTexture::GetAmbientColor() const
{
    return m_AmbientColor;
}
