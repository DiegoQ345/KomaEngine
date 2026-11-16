#include "VertexBuffer.hpp"

#include "Core/Logger.hpp"

#include <stdexcept>
#include <string>

VertexBuffer::VertexBuffer(const void* data, GLsizeiptr size, GLenum usage)
{
    if (data == nullptr || size <= 0)
    {
        Logger::Error("VertexBuffer received null data or a non-positive size");
        throw std::runtime_error("VertexBuffer received null data or a non-positive size");
    }

    glGenBuffers(1, &m_Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
    if (m_Buffer != 0)
    {
        glDeleteBuffers(1, &m_Buffer);
    }
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
}
