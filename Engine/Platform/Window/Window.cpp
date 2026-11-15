#include "Window.hpp"

#include "Core/Logger.hpp"

#include <stdexcept>

Window::Window(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        Logger::Error("Failed to initialize GLFW");
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Request OpenGL 4.6 Core Profile (GLAD generated for gl:core=4.6)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(
        width,
        height,
        title,
        nullptr,
        nullptr
    );

    if (!m_Window)
    {
        glfwTerminate();
        Logger::Error("Failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_Window);
}

Window::~Window()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
    }

    glfwTerminate();
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_Window);
}