#include "CameraController.hpp"

#include "Core/Camera.hpp"
#include "Platform/Input/Input.hpp"
#include "Platform/Window/Window.hpp"

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <algorithm>
#include <cmath>

CameraController::CameraController(Camera& camera, Input& input, Window& window)
    : m_Camera(camera), m_Input(input), m_Window(window)
{
    m_Window.SetCursorCaptured(true);
}

void CameraController::Update(float deltaTime)
{
    glm::vec3 direction(0.0f);

    if (m_Input.IsKeyPressed(Key::W)) direction += m_Camera.GetForward();
    if (m_Input.IsKeyPressed(Key::S)) direction -= m_Camera.GetForward();
    if (m_Input.IsKeyPressed(Key::D)) direction += m_Camera.GetRight();
    if (m_Input.IsKeyPressed(Key::A)) direction -= m_Camera.GetRight();
    if (m_Input.IsKeyPressed(Key::Space)) direction += glm::vec3(0.0f, 1.0f, 0.0f);
    if (m_Input.IsKeyPressed(Key::LeftControl)) direction -= glm::vec3(0.0f, 1.0f, 0.0f);

    const float length = glm::length(direction);
    if (length > 0.0f)
    {
        direction = glm::normalize(direction);

        float speed = m_MoveSpeed;
        if (m_Input.IsKeyPressed(Key::LeftShift) || m_Input.IsKeyPressed(Key::RightShift))
        {
            speed *= m_SprintMultiplier;
        }

        m_Camera.SetPosition(m_Camera.GetPosition() + direction * speed * deltaTime);
    }

    const glm::vec2 delta = m_Input.GetMouseDelta();

    if (delta.x != 0.0f || delta.y != 0.0f)
    {
        m_Yaw -= delta.x * m_MouseSensitivity;
        m_Pitch -= delta.y * m_MouseSensitivity;
        m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

        m_Camera.SetRotationEulerDegrees(glm::vec3(m_Pitch, m_Yaw, 0.0f));
    }
}
