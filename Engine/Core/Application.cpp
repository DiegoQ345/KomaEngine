#include "Application.hpp"

#include "Core/Logger.hpp"
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

    Logger::Info("OpenGL initialized successfully");
    Logger::Info(std::string("Version: ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    Logger::Info(std::string("Renderer: ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    glViewport(0, 0, 1280, 720);

    auto last = std::chrono::steady_clock::now();

    while (!window.ShouldClose())
    {
        window.PollEvents();

        const auto now = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(now - last).count();
        deltaTime = std::clamp(deltaTime, 0.0f, 0.1f);
        last = now;

        Update(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT);

        window.SwapBuffers();
    }
}

void Application::Update(float deltaTime)
{
    m_Time += deltaTime;

    const float oscillation = 0.5f + 0.5f * std::sin(m_Time);
    const float blue = 0.15f + 0.25f * oscillation;

    glClearColor(0.08f, 0.10f, blue, 1.0f);
}
