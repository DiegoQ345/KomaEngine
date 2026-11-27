#include "ModelRenderer.hpp"

#include "Material.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>

#include <algorithm>

ModelRenderer::ModelRenderer(
    std::shared_ptr<const Model> model,
    std::vector<std::shared_ptr<Material>> materials
)
    : m_Model(std::move(model))
    , m_Materials(std::move(materials))
{
}

void ModelRenderer::Draw(const glm::mat4& worldTransform, Renderer& renderer) const
{
    DrawNode(m_Model->RootNode, glm::mat4(1.0f), worldTransform, renderer);
}

void ModelRenderer::DrawNode(
    const ModelNode& node,
    const glm::mat4& parentWorld,
    const glm::mat4& rootTransform,
    Renderer& renderer
) const
{
    const glm::mat4 world = parentWorld * node.LocalTransform;
    const glm::mat4 finalTransform = rootTransform * world;

    renderer.SetFrontFaceWinding(glm::determinant(finalTransform) < 0.0f);

    for (const std::size_t meshIndex : node.MeshIndices)
    {
        const ModelMesh& entry = m_Model->Meshes[meshIndex];
        const std::size_t materialIndex =
            std::min(entry.MaterialIndex, m_Materials.size() - 1);

        const std::shared_ptr<Material>& material = m_Materials[materialIndex];
        material->GetShader()->Bind();
        material->GetShader()->SetMat4("u_Model", finalTransform);
        renderer.Draw(*entry.Mesh, *material);
    }

    renderer.SetFrontFaceWinding(false);

    for (const ModelNode& child : node.Children)
    {
        DrawNode(child, world, rootTransform, renderer);
    }
}

void ModelRenderer::DrawDepth(
    const glm::mat4& worldTransform,
    const Shader& shader,
    Renderer& renderer
) const
{
    shader.Bind();
    DrawDepthNode(m_Model->RootNode, glm::mat4(1.0f), worldTransform, shader, renderer);
}

void ModelRenderer::DrawDepthNode(
    const ModelNode& node,
    const glm::mat4& parentWorld,
    const glm::mat4& rootTransform,
    const Shader& shader,
    Renderer& renderer
) const
{
    const glm::mat4 world = parentWorld * node.LocalTransform;
    const glm::mat4 finalTransform = rootTransform * world;

    renderer.SetFrontFaceWinding(glm::determinant(finalTransform) < 0.0f);

    for (const std::size_t meshIndex : node.MeshIndices)
    {
        shader.SetMat4("u_Model", finalTransform);
        renderer.Draw(*m_Model->Meshes[meshIndex].Mesh);
    }

    renderer.SetFrontFaceWinding(false);

    for (const ModelNode& child : node.Children)
    {
        DrawDepthNode(child, world, rootTransform, shader, renderer);
    }
}
