#pragma once

#include <glad/gl.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

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

    // Los Set* requieren que el programa este activo mediante Bind().
    void SetMat4(const char* name, const glm::mat4& matrix) const;
    void SetVec3(const char* name, const glm::vec3& value) const;
    void SetInt(const char* name, int value) const;
    void SetFloat(const char* name, float value) const;
    void SetBool(const char* name, bool value) const;

private:
    GLint GetUniformLocation(const char* name) const;

    GLuint m_Program = 0;
    mutable std::unordered_map<std::string, GLint> m_UniformLocations;
};
