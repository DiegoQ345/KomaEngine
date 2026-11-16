#include "IndexBuffer.hpp"

#include "Core/Logger.hpp"

#include <limits>
#include <stdexcept>
#include <string>

IndexBuffer::IndexBuffer(const unsigned int* indices, size_t count, GLenum usage)
{
    if (indices == nullptr || count == 0)
    {
        Logger::Error("IndexBuffer received null data or a zero index count");
        throw std::runtime_error("IndexBuffer received null data or a zero index count");
    }

    const size_t bytes = count * sizeof(unsigned int);
    if (bytes / sizeof(unsigned int) != count
        || bytes > static_cast<size_t>(std::numeric_limits<GLsizeiptr>::max()))
    {
        Logger::Error("IndexBuffer byte size overflow");
        throw std::runtime_error("IndexBuffer byte size overflow");
    }

    glGenBuffers(1, &m_Buffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_Buffer);
    glBufferData(GL_COPY_WRITE_BUFFER, static_cast<GLsizeiptr>(bytes), indices, usage);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    m_Count = count;
}

IndexBuffer::~IndexBuffer()
{
    if (m_Buffer != 0)
    {
        glDeleteBuffers(1, &m_Buffer);
    }
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
}

size_t IndexBuffer::GetCount() const
{
    return m_Count;
}
