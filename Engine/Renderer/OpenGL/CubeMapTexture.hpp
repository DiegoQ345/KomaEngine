#pragma once

#include "Assets/Resource.hpp"

#include <glad/gl.h>

#include <glm/vec3.hpp>

#include <array>
#include <filesystem>
#include <string>

class CubeMapTexture : public Resource
{
public:
    using Faces = std::array<std::filesystem::path, 6>;

    explicit CubeMapTexture(const Faces& faces);
    ~CubeMapTexture() override;

    std::string_view GetType() const override { return "cubemap"; }

    CubeMapTexture(const CubeMapTexture&) = delete;
    CubeMapTexture& operator=(const CubeMapTexture&) = delete;

    void Bind(GLuint unit) const;

    const glm::vec3& GetAmbientColor() const;

private:
    GLuint m_Texture = 0;
    glm::vec3 m_AmbientColor{0.25f};
};
