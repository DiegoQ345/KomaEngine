#include "ShadowMap.hpp"

#include "Core/Logger.hpp"

#include <stdexcept>

ShadowMap::ShadowMap()
{
    glGenFramebuffers(1, &m_Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

    glGenTextures(1, &m_DepthTexture);
    glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT24,
        Resolution,
        Resolution,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    const float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        m_DepthTexture,
        0
    );
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        Logger::Error("ShadowMap: incomplete framebuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        throw std::runtime_error("ShadowMap: incomplete framebuffer");
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap()
{
    if (m_DepthTexture != 0)
    {
        glDeleteTextures(1, &m_DepthTexture);
    }
    if (m_Framebuffer != 0)
    {
        glDeleteFramebuffers(1, &m_Framebuffer);
    }
}

void ShadowMap::Begin() const
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_PreviousFramebuffer);
    glGetIntegerv(GL_VIEWPORT, m_PreviousViewport);
    glGetBooleanv(GL_COLOR_WRITEMASK, m_PreviousColorMask);
    glGetBooleanv(GL_CULL_FACE, &m_PreviousCullEnabled);
    glGetIntegerv(GL_CULL_FACE_MODE, &m_PreviousCullFace);
    glGetIntegerv(GL_FRONT_FACE, &m_PreviousFrontFace);

    glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
    glViewport(0, 0, Resolution, Resolution);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::End() const
{
    glFrontFace(m_PreviousFrontFace);
    glCullFace(m_PreviousCullFace);
    if (m_PreviousCullEnabled == GL_FALSE)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }

    glColorMask(
        m_PreviousColorMask[0],
        m_PreviousColorMask[1],
        m_PreviousColorMask[2],
        m_PreviousColorMask[3]
    );
    glViewport(
        m_PreviousViewport[0],
        m_PreviousViewport[1],
        m_PreviousViewport[2],
        m_PreviousViewport[3]
    );
    glBindFramebuffer(GL_FRAMEBUFFER, m_PreviousFramebuffer);
}

void ShadowMap::BindTexture(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
}

const glm::mat4& ShadowMap::GetLightSpaceMatrix() const
{
    return m_LightSpaceMatrix;
}

void ShadowMap::SetLightSpaceMatrix(const glm::mat4& matrix)
{
    m_LightSpaceMatrix = matrix;
}

float ShadowMap::GetBias() const
{
    return m_Bias;
}

void ShadowMap::SetBias(float bias)
{
    m_Bias = bias;
}
