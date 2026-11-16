#pragma once

#include <glad/gl.h>

#include <glm/mat4x4.hpp>
#include <string_view>

#include "Assets/Resource.hpp"

class Shader : public Resource
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader() override;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    std::string_view GetType() const override { return "shader"; }

    void Bind() const;
    // Requiere que el programa este enlazado/Bind
    void SetMat4(const char* name, const glm::mat4& matrix) const;

private:
    GLuint m_Program = 0;
};
