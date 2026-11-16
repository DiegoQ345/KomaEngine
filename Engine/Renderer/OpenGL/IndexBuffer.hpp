#pragma once

#include <glad/gl.h>

#include <cstddef>

class IndexBuffer
{
public:
    IndexBuffer(const unsigned int* indices, size_t count, GLenum usage = GL_STATIC_DRAW);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    // Requiere un VertexArray activo para vincularlo como elemento activo (GL_ELEMENT_ARRAY_BUFFER)
    void Bind() const;

    size_t GetCount() const;

private:
    GLuint m_Buffer = 0;
    size_t m_Count = 0;
};
