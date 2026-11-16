#pragma once

#include <glad/gl.h>

class VertexBuffer
{
public:
    VertexBuffer(const void* data, GLsizeiptr size, GLenum usage = GL_STATIC_DRAW);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    void Bind() const;

private:
    GLuint m_Buffer = 0;
};
