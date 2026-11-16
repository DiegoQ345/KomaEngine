#include "Application.hpp"

#include "Assets/AssetManager.hpp"
#include "Core/AssetPaths.hpp"
#include "Core/Logger.hpp"
#include "Platform/Input/Input.hpp"
#include "Core/Camera.hpp"
#include "Core/CameraController.hpp"
#include "Core/GameObject.hpp"
#include "Core/Scene.hpp"
#include "Platform/Window/Window.hpp"
#include "Renderer/OpenGL/Mesh.hpp"
#include "Renderer/OpenGL/Renderer.hpp"
#include "Renderer/OpenGL/Shader.hpp"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <string>
#include <string>
#include <vector>

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

    Camera camera;

    const auto updateViewport = [&window, &camera, this]() {
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window.GetHandle(), &width, &height);
        if (width <= 0 || height <= 0) return;
        if (width == m_FramebufferWidth && height == m_FramebufferHeight) return;
        m_FramebufferWidth = width;
        m_FramebufferHeight = height;
        glViewport(0, 0, width, height);
        camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    };

    updateViewport();

    Renderer renderer;
    AssetManager assets;

    const std::string vertexPath = AssetPath("Shaders/basic.vert").string();
    const std::string fragmentPath = AssetPath("Shaders/basic.frag").string();

    auto shader = assets.LoadShader(vertexPath, fragmentPath);
    shader->Bind();
    Logger::Info(std::string("Shader loaded successfully: ") + vertexPath);

    const std::vector<Mesh::Vertex> vertices{
        {  0.0f,  0.5f, 0.0f },
        { -0.5f, -0.5f, 0.0f },
        {  0.5f, -0.5f, 0.0f }
    };
    const std::vector<unsigned int> indices{ 0, 1, 2 };

    Mesh mesh(vertices, indices);
    Logger::Info("Mesh created successfully");

    const float cos45 = 0.70710678f;
    const float cosX = 0.81649658f;
    const float sinX = 0.57735027f;
    const float s = 0.3f;
    const float offsetX = 0.55f;
    const float offsetY = 0.1f;

    std::vector<Mesh::Vertex> cubeVertices;
    cubeVertices.reserve(8);

    const float corner[8][3] = {
        { s,  s,  s}, { s,  s, -s}, { s, -s,  s}, { s, -s, -s},
        {-s,  s,  s}, {-s,  s, -s}, {-s, -s,  s}, {-s, -s, -s}
    };

    for (const auto& c : corner)
    {
        const float xr = c[0] * cos45 + c[2] * cos45;
        const float yr = c[1];
        const float zr = -c[0] * cos45 + c[2] * cos45;

        Mesh::Vertex v{
            xr + offsetX,
            yr * cosX - zr * sinX + offsetY,
            yr * sinX + zr * cosX
        };
        cubeVertices.push_back(v);
    }

    const std::vector<unsigned int> cubeIndices{
        0, 6, 2, 0, 4, 6, // cara +z
        1, 7, 5, 1, 3, 7, // cara -z
        1, 0, 2, 1, 2, 3, // cara +x (trasera)
        4, 7, 6, 4, 5, 7, // cara -x (visible)
        1, 5, 4, 1, 4, 0, // cara +y
        3, 2, 6, 3, 6, 7  // cara -y
    };

    Mesh cube(cubeVertices, cubeIndices);
    Logger::Info("Cube created successfully");

    Scene scene;

    auto& cubeLeft = scene.AddGameObject("Cube_Left");
    cubeLeft.GetTransform().SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    cubeLeft.GetTransform().SetRotationEulerDegrees(glm::vec3(0.0f, 45.0f, 0.0f));

    auto& cubeRight = scene.AddGameObject("Cube_Right");
    cubeRight.GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    cubeRight.GetTransform().SetScale(glm::vec3(0.7f, 0.7f, 0.7f));

    Input input(window.GetHandle());
    CameraController cameraController(camera, input, window);

    auto last = std::chrono::steady_clock::now();

    while (!window.ShouldClose())
    {
        window.PollEvents();

        const auto now = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(now - last).count();
        deltaTime = std::clamp(deltaTime, 0.0f, 0.1f);
        last = now;

        input.Update();
        cameraController.Update(deltaTime);

        updateViewport();

        if (input.IsKeyPressed(Key::Escape))
        {
            window.Close();
        }

        Update(deltaTime);

        const glm::mat4 viewProjection = camera.GetProjectionMatrix() * camera.GetViewMatrix();

        renderer.SetClearColor(0.08f, 0.10f, 0.15f, 1.0f);
        renderer.Clear();

        shader->Bind();
        shader->SetMat4("u_ViewProjection", viewProjection);
        shader->SetMat4("u_Model", glm::mat4(1.0f));
        renderer.Draw(mesh);
        for (const auto& object : scene.GetGameObjects())
        {
            shader->SetMat4("u_Model", scene.GetWorldMatrix(*object));
            renderer.Draw(cube);
        }

        window.SwapBuffers();
    }
}

void Application::Update(float deltaTime)
{
    m_Time += deltaTime;
}

