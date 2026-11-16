#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void SetPosition(const glm::vec3& position);
    glm::vec3 GetPosition() const;

    void SetTarget(const glm::vec3& target);
    glm::vec3 GetTarget() const;

    void SetUp(const glm::vec3& up);
    glm::vec3 GetUp() const;

    void SetFieldOfView(float degrees);
    float GetFieldOfView() const;

    void SetAspectRatio(float ratio);
    float GetAspectRatio() const;

    void SetNearPlane(float value);
    float GetNearPlane() const;

    void SetFarPlane(float value);
    float GetFarPlane() const;

private:
    glm::vec3 m_Position{0.0f, 0.0f, 3.0f};
    glm::vec3 m_Target{0.0f, 0.0f, 0.0f};
    glm::vec3 m_Up{0.0f, 1.0f, 0.0f};
    float m_Fov = 45.0f;
    float m_AspectRatio = 16.0f / 9.0f;
    float m_Near = 0.1f;
    float m_Far = 1000.0f;
};
