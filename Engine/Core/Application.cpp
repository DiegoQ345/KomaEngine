#include "Application.hpp"

#include "Core/Logger.hpp"
#include "Platform/Input/Input.hpp"
#include "Platform/Window/Window.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <string>

void Application::Run()
{
    Window window(1280, 720, "KomaEngine");

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        Logger::Error("Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD");
    }

    GLint majorVersion = 0;
    GLint minorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

    if (majorVersion < 4 || (majorVersion == 4 && minorVersion < 6))
    {
        Logger::Error(
            "OpenGL 4.6 Core required, but got " + std::to_string(majorVersion) + "." +
            std::to_string(minorVersion)
        );
        throw std::runtime_error("OpenGL 4.6 Core context is not available");
    }

    Logger::Info(
        "OpenGL " + std::to_string(majorVersion) + "." + std::to_string(minorVersion) +
        " Core validated"
    );

    Logger::Info("OpenGL initialized successfully");
    Logger::Info(std::string("Version: ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Logger::Info(std::string("Renderer: ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    const auto updateViewport = [&window, this]() {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window.GetHandle(), &width, &height);
        if (width <= 0 || height <= 0) return;
        if (width == m_FramebufferWidth && height == m_FramebufferHeight) return;
        m_FramebufferWidth = width;
        m_FramebufferHeight = height;
        glViewport(0, 0, width, height);
    };

    updateViewport();

    Input input(window.GetHandle());

    auto last = std::chrono::steady_clock::now();

    while (!window.ShouldClose())
    {
        window.PollEvents();

        const auto now = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(now - last).count();
        deltaTime = std::clamp(deltaTime, 0.0f, 0.1f);
        last = now;

        updateViewport();

        input.Update();

        if (input.IsKeyPressed(Key::Escape))
        {
            window.Close();
        }

        Update(deltaTime, input);

        glClear(GL_COLOR_BUFFER_BIT);

        window.SwapBuffers();
    }
}

void Application::Update(float deltaTime, const Input& input)
{
    m_Time += deltaTime;

    // Prueba temporal de Input
    if (input.IsKeyPressed(Key::W)) m_Green = std::min(1.0f, m_Green + deltaTime * 0.5f);
    if (input.IsKeyPressed(Key::S)) m_Green = std::max(0.0f, m_Green - deltaTime * 0.5f);
    if (input.IsKeyPressed(Key::A)) m_Red = std::min(1.0f, m_Red + deltaTime * 0.5f);
    if (input.IsKeyPressed(Key::D)) m_Red = std::max(0.0f, m_Red - deltaTime * 0.5f);

    const glm::vec2 delta = input.GetMouseDelta();
    m_LogTimer += deltaTime;
    if ((delta.x != 0.0f || delta.y != 0.0f) && m_LogTimer >= 0.5f)
    {
        const glm::vec2 position = input.GetMousePosition();
        Logger::Info(
            "Mouse pos=(" + std::to_string(position.x) + ", " + std::to_string(position.y) +
            ") delta=(" + std::to_string(delta.x) + ", " + std::to_string(delta.y) + ")"
        );
        m_LogTimer = 0.0f;
    }

    float blue = 0.15f;
    if (input.IsMouseButtonPressed(MouseButton::Left))
    {
        blue = 0.28f + 0.18f * std::sin(m_Time * 8.0f);
    }

    glClearColor(m_Red, m_Green, blue, 1.0f);
}
