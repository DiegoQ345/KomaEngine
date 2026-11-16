#pragma once

class Camera;
class Input;
class Window;

class CameraController
{
public:
    CameraController(Camera& camera, Input& input, Window& window);

    void Update(float deltaTime);

private:
    Camera& m_Camera;
    Input& m_Input;
    Window& m_Window;

    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;

    float m_MoveSpeed = 3.0f;
    float m_SprintMultiplier = 3.0f;
    float m_MouseSensitivity = 0.08f;
};
