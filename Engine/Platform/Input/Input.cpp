#include "Input.hpp"

#include "Core/Logger.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>

namespace
{
    int MapKey(Key key)
    {
        switch (key)
        {
            case Key::W:      return GLFW_KEY_W;
            case Key::A:      return GLFW_KEY_A;
            case Key::S:      return GLFW_KEY_S;
            case Key::D:      return GLFW_KEY_D;
            case Key::Escape: return GLFW_KEY_ESCAPE;
            case Key::Space:  return GLFW_KEY_SPACE;
            case Key::Up:     return GLFW_KEY_UP;
            case Key::Down:   return GLFW_KEY_DOWN;
            case Key::Left:   return GLFW_KEY_LEFT;
            case Key::Right:  return GLFW_KEY_RIGHT;
            case Key::LeftShift:    return GLFW_KEY_LEFT_SHIFT;
            case Key::RightShift:   return GLFW_KEY_RIGHT_SHIFT;
            case Key::LeftControl:  return GLFW_KEY_LEFT_CONTROL;
            case Key::RightControl: return GLFW_KEY_RIGHT_CONTROL;
        }

        return GLFW_KEY_UNKNOWN;
    }

    int MapMouseButton(MouseButton button)
    {
        switch (button)
        {
            case MouseButton::Left:   return GLFW_MOUSE_BUTTON_LEFT;
            case MouseButton::Right:  return GLFW_MOUSE_BUTTON_RIGHT;
            case MouseButton::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        }

        return -1;
    }
}

Input::Input(GLFWwindow* window)
    : m_Window(window)
{
    if (m_Window == nullptr)
    {
        Logger::Error("Input received a null GLFWwindow handle");
        throw std::runtime_error("Input received a null GLFWwindow handle");
    }
}

void Input::Update()
{
    double x = 0.0f;
    double y = 0.0f;
    glfwGetCursorPos(m_Window, &x, &y);

    const glm::vec2 position{
        static_cast<float>(x),
        static_cast<float>(y)
    };

    if (!m_MouseInitialized)
    {
        m_LastMousePos = position;
        m_MouseDelta = glm::vec2{0.0f};
        m_MouseInitialized = true;
        return;
    }

    m_MouseDelta = position - m_LastMousePos;
    m_LastMousePos = position;
}

bool Input::IsKeyPressed(Key key) const
{
    return glfwGetKey(m_Window, MapKey(key)) == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(MouseButton button) const
{
    return glfwGetMouseButton(m_Window, MapMouseButton(button)) == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition() const
{
    double x = 0.0f;
    double y = 0.0f;
    glfwGetCursorPos(m_Window, &x, &y);

    return glm::vec2{
        static_cast<float>(x),
        static_cast<float>(y)
    };
}

glm::vec2 Input::GetMouseDelta() const
{
    return m_MouseDelta;
}
