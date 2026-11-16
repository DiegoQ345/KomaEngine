#pragma once

#include <glad/gl.h>

#include <cstddef>

class VertexBuffer;
class IndexBuffer;

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void Bind() const;

    // Vincula el VertexBuffer mediante su método Bind(); no almacena punteros
    void AddVertexBuffer(const VertexBuffer& buffer);

    // Requiere el VAO y el VertexBuffer correspondientes previamente vinculados.
    void EnableVertexAttribute(
        GLuint index,
        GLint componentCount,
        GLenum type,
        GLboolean normalized,
        GLsizei stride,
        std::size_t offset
    );

    // Vincula el IndexBuffer mediante su método Bind(); no almacena punteros
    void SetIndexBuffer(const IndexBuffer& buffer);

private:
    GLuint m_VertexArray = 0;
};
