#include "Application.hpp"

#include "Core/Logger.hpp"
#include "Platform/Window/Window.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

    glClearColor(
        0.08f,
        0.10f,
        0.15f,
        1.0f
    );

    while (!window.ShouldClose())
    {
        window.PollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        window.SwapBuffers();
    }
}
