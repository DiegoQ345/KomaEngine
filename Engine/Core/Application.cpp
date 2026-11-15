#include "Application.hpp"

#include "Platform/Window/Window.hpp"

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

void Application::Run()
{
    Window window(1280, 720, "KomaEngine");

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    std::cout << "OpenGL initialized successfully\n";

    std::cout << "Version: "
              << glGetString(GL_VERSION) << '\n';

    std::cout << "Renderer: "
              << glGetString(GL_RENDERER) << '\n';

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
