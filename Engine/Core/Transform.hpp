#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3& position);

    glm::quat GetRotation() const;
    void SetRotation(const glm::quat& rotation);
    void SetRotationEulerDegrees(const glm::vec3& eulerDegrees);

    glm::vec3 GetScale() const;
    void SetScale(const glm::vec3& scale);

    glm::mat4 GetModelMatrix() const;

private:
    glm::vec3 m_Position{0.0f};
    glm::quat m_Rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 m_Scale{1.0f};
};
