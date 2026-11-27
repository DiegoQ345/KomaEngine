#pragma once

#include "Renderer/OpenGL/Shader.hpp"
#include "Renderer/OpenGL/Texture2D.hpp"

#include <glm/vec3.hpp>

#include <memory>

class Material
{
public:
    Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture2D> texture = nullptr);

    void Bind() const;

    void SetBaseColor(const glm::vec3& color);
    const glm::vec3& GetBaseColor() const;

    void SetMetallic(float value);
    float GetMetallic() const;
    void SetRoughness(float value);
    float GetRoughness() const;
    void SetUsePBR(bool value);
    bool GetUsePBR() const;

    std::shared_ptr<Shader> GetShader() const;
    std::shared_ptr<Texture2D> GetTexture() const;

private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Texture2D> m_Texture;
    glm::vec3 m_BaseColor{1.0f};
    float m_Metallic = 0.0f;
    float m_Roughness = 0.5f;
    bool m_UsePBR = false;
};
