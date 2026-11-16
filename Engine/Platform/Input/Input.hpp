#pragma once

#include <glm/vec2.hpp>

struct GLFWwindow;

enum class Key
{
    W,
    A,
    S,
    D,
    Escape,
    Space,
    Up,
    Down,
    Left,
    Right
};

enum class MouseButton
{
    Left,
    Right,
    Middle
};

class Input
{
public:
    explicit Input(GLFWwindow* window);
    ~Input() = default;

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    void Update();

    bool IsKeyPressed(Key key) const;
    bool IsMouseButtonPressed(MouseButton button) const;
    glm::vec2 GetMousePosition() const;
    glm::vec2 GetMouseDelta() const;

private:
    GLFWwindow* m_Window;

    glm::vec2 m_LastMousePos{0.0f};
    glm::vec2 m_MouseDelta{0.0f};
    bool m_MouseInitialized = false;
};
