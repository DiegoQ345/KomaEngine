#pragma once

#include "Transform.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
    Camera();

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    void SetPosition(const glm::vec3& position);
    glm::vec3 GetPosition() const;

    void SetRotation(const glm::quat& rotation);
    glm::quat GetRotation() const;
    void SetRotationEulerDegrees(const glm::vec3& eulerDegrees);

    glm::vec3 GetForward() const;
    glm::vec3 GetRight() const;
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
    Transform m_Transform;

    float m_Fov = 45.0f;
    float m_AspectRatio = 16.0f / 9.0f;
    float m_Near = 0.1f;
    float m_Far = 1000.0f;
};
