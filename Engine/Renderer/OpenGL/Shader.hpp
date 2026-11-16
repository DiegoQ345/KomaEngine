#pragma once

#include <glad/gl.h>

#include <glm/mat4x4.hpp>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Bind() const;
    // Requiere que el programa este enlazado/Bind
    void SetMat4(const char* name, const glm::mat4& matrix) const;

private:
    GLuint m_Program = 0;
};
