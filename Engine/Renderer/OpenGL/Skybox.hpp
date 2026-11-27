#pragma once

#include <glad/gl.h>

#include <glm/vec3.hpp>

#include <memory>

class Camera;
class Renderer;
class Shader;

class Skybox
{
public:
    explicit Skybox(std::shared_ptr<Shader> shader);
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void Draw(const Camera& camera, Renderer& renderer, float elapsedTime) const;

    const glm::vec3& GetAmbientColor() const;

private:
    std::shared_ptr<Shader> m_Shader;

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;

    glm::vec3 m_AmbientColor{0.22f, 0.25f, 0.30f};
};
