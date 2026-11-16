#pragma once

class Mesh;

class Renderer
{
public:
    Renderer();

    void SetClearColor(float r, float g, float b, float a = 1.0f);
    void Clear() const;
    void Draw(const Mesh& mesh) const;
};
