#include "Material.hpp"

#include <stdexcept>
#include <string>
#include <utility>

Material::Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture2D> texture)
    : m_Shader(std::move(shader))
    , m_Texture(std::move(texture))
{
    if (!m_Shader)
    {
        throw std::invalid_argument("Material received a null Shader");
    }
}

void Material::Bind() const
{
    m_Shader->Bind();

    if (m_Texture)
    {
        m_Texture->Bind(0);
        m_Shader->SetInt("u_UseTexture", 1);
        m_Shader->SetInt("u_TextureSampler", 0);
    }
    else
    {
        m_Shader->SetInt("u_UseTexture", 0);
    }

    m_Shader->SetVec3("u_BaseColor", m_BaseColor);
}

void Material::SetBaseColor(const glm::vec3& color)
{
    m_BaseColor = color;
}

const glm::vec3& Material::GetBaseColor() const
{
    return m_BaseColor;
}

std::shared_ptr<Shader> Material::GetShader() const
{
    return m_Shader;
}

std::shared_ptr<Texture2D> Material::GetTexture() const
{
    return m_Texture;
}
