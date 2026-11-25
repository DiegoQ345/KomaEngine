#include "Mesh.hpp"

#include "Core/Logger.hpp"

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : m_VertexArray()
    , m_VertexBuffer(vertices.data(), static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)))
    , m_IndexBuffer(indices.data(), indices.size())
{
    if (vertices.empty() || indices.empty())
    {
        Logger::Error("Mesh received empty vertex or index data");
        throw std::runtime_error("Mesh received empty vertex or index data");
    }

    if (indices.size() > static_cast<size_t>(std::numeric_limits<GLsizei>::max()))
    {
        Logger::Error("Mesh index count exceeds GLsizei range");
        throw std::runtime_error("Mesh index count exceeds GLsizei range");
    }

    const size_t vertexBytes = vertices.size() * sizeof(Vertex);
    if (vertexBytes / sizeof(Vertex) != vertices.size())
    {
        Logger::Error("Mesh vertex data size overflow");
        throw std::runtime_error("Mesh vertex data size overflow");
    }

    m_VertexArray.Bind();
    m_VertexArray.AddVertexBuffer(m_VertexBuffer);
    m_VertexArray.EnableVertexAttribute(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(Vertex)),
        offsetof(Vertex, px)
    );
    m_VertexArray.EnableVertexAttribute(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(Vertex)),
        offsetof(Vertex, u)
    );
    m_VertexArray.EnableVertexAttribute(
        2,
        3,
        GL_FLOAT,
        GL_FALSE,
        static_cast<GLsizei>(sizeof(Vertex)),
        offsetof(Vertex, nx)
    );
    m_VertexArray.SetIndexBuffer(m_IndexBuffer);
}

void Mesh::Draw() const
{
    m_VertexArray.Bind();

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(m_IndexBuffer.GetCount()),
        GL_UNSIGNED_INT,
        nullptr
    );
}
