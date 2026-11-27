#pragma once

#include "Assets/Import/ModelImporter.hpp"

#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

class Material;
class Renderer;
class Shader;

class ModelRenderer
{
public:
    ModelRenderer(
        std::shared_ptr<const Model> model,
        std::vector<std::shared_ptr<Material>> materials
    );

    void Draw(const glm::mat4& worldTransform, Renderer& renderer) const;
    void DrawDepth(const glm::mat4& worldTransform, const Shader& shader, Renderer& renderer) const;

    const std::shared_ptr<const Model>& GetModel() const { return m_Model; }
    const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return m_Materials; }

private:
    void DrawNode(
        const ModelNode& node,
        const glm::mat4& parentWorld,
        const glm::mat4& rootTransform,
        Renderer& renderer
    ) const;

    void DrawDepthNode(
        const ModelNode& node,
        const glm::mat4& parentWorld,
        const glm::mat4& rootTransform,
        const Shader& shader,
        Renderer& renderer
    ) const;

    std::shared_ptr<const Model> m_Model;
    std::vector<std::shared_ptr<Material>> m_Materials;
};
