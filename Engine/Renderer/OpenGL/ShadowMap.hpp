#pragma once

#include <glad/gl.h>

#include <glm/mat4x4.hpp>

class ShadowMap
{
public:
    ShadowMap();
    ~ShadowMap();

    ShadowMap(const ShadowMap&) = delete;
    ShadowMap& operator=(const ShadowMap&) = delete;

    void Begin() const;
    void End() const;

    void BindTexture(GLuint unit) const;

    const glm::mat4& GetLightSpaceMatrix() const;
    void SetLightSpaceMatrix(const glm::mat4& matrix);

    float GetBias() const;
    void SetBias(float bias);

    static constexpr int Resolution = 1024;

private:
    GLuint m_Framebuffer = 0;
    GLuint m_DepthTexture = 0;

    glm::mat4 m_LightSpaceMatrix{1.0f};
    float m_Bias = 0.005f;

    mutable GLint m_PreviousFramebuffer = 0;
    mutable GLint m_PreviousViewport[4]{0, 0, 0, 0};
    mutable GLboolean m_PreviousColorMask[4]{GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE};
    mutable GLboolean m_PreviousCullEnabled = GL_FALSE;
    mutable GLint m_PreviousCullFace = GL_BACK;
    mutable GLint m_PreviousFrontFace = GL_CCW;
};
