#include "Renderer.hpp"

#include "Material.hpp"
#include "Mesh.hpp"

#include <glad/gl.h>

Renderer::Renderer()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw(const Mesh& mesh) const
{
    mesh.Draw();
}

void Renderer::Draw(const Mesh& mesh, const Material& material) const
{
    material.Bind();
    mesh.Draw();
}
