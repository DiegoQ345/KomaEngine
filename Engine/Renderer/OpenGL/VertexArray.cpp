#include "VertexArray.hpp"

#include "Core/Logger.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"

#include <stdexcept>
#include <string>

VertexArray::VertexArray()
{
    GLuint vertexArray = 0;
    glGenVertexArrays(1, &vertexArray);

    if (vertexArray == 0)
    {
        Logger::Error("Failed to create VertexArray");
        throw std::runtime_error("Failed to create VertexArray");
    }

    m_VertexArray = vertexArray;
}

VertexArray::~VertexArray()
{
    if (m_VertexArray != 0)
    {
        glDeleteVertexArrays(1, &m_VertexArray);
    }
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_VertexArray);
}

void VertexArray::AddVertexBuffer(const VertexBuffer& buffer)
{
    buffer.Bind();
}

void VertexArray::EnableVertexAttribute(
    GLuint index,
    GLint componentCount,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    std::size_t offset
)
{
    if (componentCount < 1 || componentCount > 4)
    {
        Logger::Error("VertexArray attribute component count out of range (1-4)");
        throw std::runtime_error("VertexArray attribute component count out of range (1-4)");
    }

    if (stride < 0)
    {
        Logger::Error("VertexArray attribute stride must not be negative");
        throw std::runtime_error("VertexArray attribute stride must not be negative");
    }

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(
        index,
        componentCount,
        type,
        normalized,
        stride,
        reinterpret_cast<const void*>(offset)
    );
}

void VertexArray::SetIndexBuffer(const IndexBuffer& buffer)
{
    buffer.Bind();
}
