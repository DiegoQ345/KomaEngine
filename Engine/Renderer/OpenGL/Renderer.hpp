#pragma once

#include <cstdint>

class Mesh;
class Material;

class Renderer
{
public:
    Renderer();

    void SetClearColor(float r, float g, float b, float a = 1.0f);
    void Clear() const;
    void Draw(const Mesh& mesh) const;
    void Draw(const Mesh& mesh, const Material& material) const;
    void SetFrontFaceWinding(bool clockwise) const;

    void BeginFrameStats();
    void AddDrawCall(uint32_t count = 1);
    uint32_t GetDrawCalls() const;

private:
    uint32_t m_DrawCallsThisFrame = 0;
};
