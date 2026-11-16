#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <stdexcept>
#include <cmath>

glm::vec3 Transform::GetPosition() const
{
    return m_Position;
}

void Transform::SetPosition(const glm::vec3& position)
{
    m_Position = position;
}

glm::quat Transform::GetRotation() const
{
    return m_Rotation;
}

void Transform::SetRotation(const glm::quat& rotation)
{
    const float length = glm::length(rotation);
    if (length < 1e-6f)
    {
        throw std::invalid_argument("Transform::SetRotation received a zero-length quaternion");
    }

    m_Rotation = glm::normalize(rotation);
}

void Transform::SetRotationEulerDegrees(const glm::vec3& eulerDegrees)
{
    const glm::vec3 radians = glm::radians(eulerDegrees);

    const glm::quat yaw = glm::angleAxis(radians.y, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat pitch = glm::angleAxis(radians.x, glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::quat roll = glm::angleAxis(radians.z, glm::vec3(0.0f, 0.0f, 1.0f));

    // Orden YXZ con vectores columna: R = yaw * pitch * roll.
    // Las transformaciones de la derecha se aplican primero,
    // por lo que el orden es roll -> pitch -> yaw.
    m_Rotation = glm::normalize(yaw * pitch * roll);
}

glm::vec3 Transform::GetScale() const
{
    return m_Scale;
}

void Transform::SetScale(const glm::vec3& scale)
{
    m_Scale = scale;
}

glm::mat4 Transform::GetModelMatrix() const
{
    const glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);
    const glm::mat4 rotation = glm::mat4_cast(m_Rotation);
    const glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

    return translation * rotation * scale;
}
