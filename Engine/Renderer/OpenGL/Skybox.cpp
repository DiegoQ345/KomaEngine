#include "Skybox.hpp"

#include "Core/Camera.hpp"
#include "Core/Logger.hpp"
#include "Renderer/OpenGL/Renderer.hpp"
#include "Renderer/OpenGL/Shader.hpp"

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

#include <stdexcept>

namespace
{
    constexpr float Vertices[] = {
        // +X
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,
        // -X
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
        // +Y
        -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        // -Y
        -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        // +Z
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
        // -Z
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f
    };
}

Skybox::Skybox(std::shared_ptr<Shader> shader)
    : m_Shader(std::move(shader))
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindVertexArray(0);

    if (m_VAO == 0 || m_VBO == 0)
    {
        Logger::Error("Skybox: failed to create geometry");
        throw std::runtime_error("Skybox: failed to create geometry");
    }
}

Skybox::~Skybox()
{
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
}

void Skybox::Draw(const Camera& camera, Renderer& renderer, float elapsedTime) const
{
    GLint previousDepthFunc = GL_LESS;
    GLboolean previousDepthMask = GL_TRUE;
    GLboolean previousCullEnabled = GL_FALSE;
    GLint previousCullFace = GL_BACK;
    GLint previousFrontFace = GL_CCW;

    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &previousDepthMask);
    glGetBooleanv(GL_CULL_FACE, &previousCullEnabled);
    glGetIntegerv(GL_CULL_FACE_MODE, &previousCullFace);
    glGetIntegerv(GL_FRONT_FACE, &previousFrontFace);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    const glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    const glm::mat4 projection = camera.GetProjectionMatrix();

    m_Shader->Bind();
    m_Shader->SetMat4("uProjection", projection);
    m_Shader->SetMat4("uView", view);
    m_Shader->SetFloat("uTime", elapsedTime);
    m_Shader->SetVec3("uSunDir", glm::normalize(glm::vec3(0.5f, 1.0f, 0.3f)));
    m_Shader->SetFloat("uCloudSpeed", 1.0f);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    renderer.AddDrawCall(1);

    glFrontFace(previousFrontFace);
    glCullFace(previousCullFace);
    if (previousCullEnabled == GL_FALSE) glDisable(GL_CULL_FACE);
    else glEnable(GL_CULL_FACE);
    glDepthMask(previousDepthMask);
    glDepthFunc(previousDepthFunc);

    (void)renderer;
}

const glm::vec3& Skybox::GetAmbientColor() const
{
    return m_AmbientColor;
}
