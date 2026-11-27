#pragma once

#include "Assets/Resource.hpp"

#include <glad/gl.h>

#include <filesystem>
#include <string_view>

#include <glm/vec2.hpp>

class Texture2D : public Resource
{
public:
    explicit Texture2D(const std::filesystem::path& path, bool sRGB = true);
    ~Texture2D() override;

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    std::string_view GetType() const override { return "texture"; }

    glm::ivec2 GetSize() const;

    void Bind(GLuint unit = 0) const;

private:
    GLuint m_Texture = 0;
    int m_Width = 0;
    int m_Height = 0;
};
