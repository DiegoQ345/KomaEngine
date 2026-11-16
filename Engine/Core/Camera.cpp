#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <stdexcept>
#include <cmath>

Camera::Camera()
{
    m_Transform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
}

glm::mat4 Camera::GetViewMatrix() const
{
    // View = (T * R)^-1 = R^-1 * T(-p) = mat4_cast(conjugate(q)) * translate(-p)
    const glm::mat4 rotation = glm::mat4_cast(glm::conjugate(m_Transform.GetRotation()));
    const glm::mat4 inverseTranslation = glm::translate(glm::mat4(1.0f), -m_Transform.GetPosition());

    return rotation * inverseTranslation;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_Near, m_Far);
}

void Camera::SetPosition(const glm::vec3& position)
{
    m_Transform.SetPosition(position);
}

glm::vec3 Camera::GetPosition() const
{
    return m_Transform.GetPosition();
}

void Camera::SetRotation(const glm::quat& rotation)
{
    m_Transform.SetRotation(rotation);
}

glm::quat Camera::GetRotation() const
{
    return m_Transform.GetRotation();
}

void Camera::SetRotationEulerDegrees(const glm::vec3& eulerDegrees)
{
    m_Transform.SetRotationEulerDegrees(eulerDegrees);
}

glm::vec3 Camera::GetForward() const
{
    return glm::normalize(m_Transform.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Camera::GetRight() const
{
    return glm::normalize(m_Transform.GetRotation() * glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 Camera::GetUp() const
{
    return glm::normalize(m_Transform.GetRotation() * glm::vec3(0.0f, 1.0f, 0.0f));
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
