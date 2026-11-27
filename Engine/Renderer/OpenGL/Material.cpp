#include "Material.hpp"

#include <algorithm>
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
    m_Shader->SetInt("u_UsePBR", m_UsePBR ? 1 : 0);
    m_Shader->SetFloat("u_Metallic", m_Metallic);
    m_Shader->SetFloat("u_Roughness", m_Roughness);
    m_Shader->SetInt("u_UseNormalMap", m_UseNormalMap ? 1 : 0);

    if (m_NormalTexture)
    {
        m_NormalTexture->Bind(1);
        m_Shader->SetInt("u_NormalSampler", 1);
    }
}

void Material::SetBaseColor(const glm::vec3& color)
{
    m_BaseColor = color;
}

const glm::vec3& Material::GetBaseColor() const
{
    return m_BaseColor;
}

void Material::SetMetallic(float value)
{
    m_Metallic = std::clamp(value, 0.0f, 1.0f);
}

float Material::GetMetallic() const
{
    return m_Metallic;
}

void Material::SetRoughness(float value)
{
    m_Roughness = std::clamp(value, 0.04f, 1.0f);
}

float Material::GetRoughness() const
{
    return m_Roughness;
}

void Material::SetUsePBR(bool value)
{
    m_UsePBR = value;
}

bool Material::GetUsePBR() const
{
    return m_UsePBR;
}

void Material::SetNormalTexture(std::shared_ptr<Texture2D> texture)
{
    m_NormalTexture = std::move(texture);
}

std::shared_ptr<Texture2D> Material::GetNormalTexture() const
{
    return m_NormalTexture;
}

void Material::SetUseNormalMap(bool value)
{
    m_UseNormalMap = value;
}

bool Material::GetUseNormalMap() const
{
    return m_UseNormalMap;
}

std::shared_ptr<Shader> Material::GetShader() const
{
    return m_Shader;
}

std::shared_ptr<Texture2D> Material::GetTexture() const
{
    return m_Texture;
}
