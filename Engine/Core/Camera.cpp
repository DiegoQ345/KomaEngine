#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <cmath>
#include <algorithm>

namespace
{
    constexpr float kEpsilon = 1e-5f;

    bool IsDirectionDegenerate(const glm::vec3& direction, const glm::vec3& up)
    {
        if (glm::length(direction) < kEpsilon) return true;
        if (glm::length(up) < kEpsilon) return true;

        const glm::vec3 forward = glm::normalize(direction);
        const glm::vec3 upNormalized = glm::normalize(up);

        return glm::length(glm::cross(forward, upNormalized)) < kEpsilon;
    }
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_Position, m_Target, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_Near, m_Far);
}

void Camera::SetPosition(const glm::vec3& position)
{
    const glm::vec3 direction = m_Target - position;
    if (IsDirectionDegenerate(direction, m_Up))
    {
        throw std::invalid_argument("Camera position cannot coincide with target or be parallel to Up");
    }

    m_Position = position;
}

glm::vec3 Camera::GetPosition() const
{
    return m_Position;
}

void Camera::SetTarget(const glm::vec3& target)
{
    const glm::vec3 direction = target - m_Position;
    if (IsDirectionDegenerate(direction, m_Up))
    {
        throw std::invalid_argument("Camera target cannot coincide with position or be parallel to Up");
    }

    m_Target = target;
}

glm::vec3 Camera::GetTarget() const
{
    return m_Target;
}

void Camera::SetUp(const glm::vec3& up)
{
    const glm::vec3 direction = m_Target - m_Position;
    if (IsDirectionDegenerate(direction, up))
    {
        throw std::invalid_argument("Camera Up cannot be zero or parallel to the view direction");
    }

    m_Up = up;
}

glm::vec3 Camera::GetUp() const
{
    return m_Up;
}

void Camera::SetFieldOfView(float degrees)
{
    if (degrees <= 0.0f || degrees >= 180.0f)
    {
        throw std::invalid_argument("Field of view must be in the (0, 180) range");
    }

    m_Fov = degrees;
}

float Camera::GetFieldOfView() const
{
    return m_Fov;
}

void Camera::SetAspectRatio(float ratio)
{
    if (ratio <= 0.0f)
    {
        throw std::invalid_argument("Aspect ratio must be positive");
    }

    m_AspectRatio = ratio;
}

float Camera::GetAspectRatio() const
{
    return m_AspectRatio;
}

void Camera::SetNearPlane(float value)
{
    if (value <= 0.0f || value >= m_Far)
    {
        throw std::invalid_argument("Near plane must be positive and lower than far plane");
    }

    m_Near = value;
}

float Camera::GetNearPlane() const
{
    return m_Near;
}

void Camera::SetFarPlane(float value)
{
    if (value <= m_Near)
    {
        throw std::invalid_argument("Far plane must be greater than near plane");
    }

    m_Far = value;
}

float Camera::GetFarPlane() const
{
    return m_Far;
}
