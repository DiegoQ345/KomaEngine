#pragma once

#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"

#include <cstddef>
#include <vector>

class Mesh
{
public:
    struct Vertex
    {
        float px, py, pz;
        float nx, ny, nz;
        float u, v;
    };

    static_assert(sizeof(Vertex) == 32);
    static_assert(offsetof(Vertex, px) == 0);
    static_assert(offsetof(Vertex, nx) == 12);
    static_assert(offsetof(Vertex, u) == 24);

    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh() = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Draw() const;

private:
    VertexArray m_VertexArray;
    VertexBuffer m_VertexBuffer;
    IndexBuffer m_IndexBuffer;
};
