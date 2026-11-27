#include "Application.hpp"

#include "Assets/AssetManager.hpp"
#include "Assets/Import/ModelImporter.hpp"
#include "Core/AssetPaths.hpp"
#include "Core/Logger.hpp"
#include "Platform/Input/Input.hpp"
#include "Core/Camera.hpp"
#include "Core/CameraController.hpp"
#include "Core/GameObject.hpp"
#include "Core/Scene.hpp"
#include "Core/Transform.hpp"
#include "Platform/Window/Window.hpp"
#include "Renderer/OpenGL/Mesh.hpp"
#include "Renderer/OpenGL/Material.hpp"
#include "Renderer/OpenGL/Mesh.hpp"
#include "Renderer/OpenGL/ModelRenderer.hpp"
#include "Renderer/OpenGL/Renderer.hpp"
#include "Renderer/OpenGL/Texture2D.hpp"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <memory>
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
    camera.SetPosition(glm::vec3(0.0f, 0.5f, -1.0f));

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

    const std::string texturePath = AssetPath("Textures/pattern.png").string();
    auto texture = assets.LoadTexture2D(texturePath);
    texture->Bind(0);
    shader->SetInt("u_TextureSampler", 0);
    Logger::Info("Texture loaded and bound: " + std::to_string(texture->GetSize().x) + "x" + std::to_string(texture->GetSize().y));

    Material materialTriangle(shader, texture);
    materialTriangle.SetBaseColor(glm::vec3(1.0f));

    Material materialLeft(shader, texture);
    materialLeft.SetBaseColor(glm::vec3(0.95f, 0.25f, 0.25f));

    Material materialRight(shader, texture);
    materialRight.SetBaseColor(glm::vec3(0.05f, 0.15f, 1.0f));

    const std::vector<Mesh::Vertex> vertices{
        {  0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f },
        { -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f },
        {  0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f }
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

    const float corner[8][3] = {
        { s,  s,  s}, { s,  s, -s}, { s, -s,  s}, { s, -s, -s},
        {-s,  s,  s}, {-s,  s, -s}, {-s, -s,  s}, {-s, -s, -s}
    };

    glm::vec3 projected[8];
    for (int i = 0; i < 8; i++)
    {
        const float* c = corner[i];
        const float xr = c[0] * cos45 + c[2] * cos45;
        const float yr = c[1];
        const float zr = -c[0] * cos45 + c[2] * cos45;

        projected[i] = glm::vec3(
            xr + offsetX,
            yr * cosX - zr * sinX + offsetY,
            yr * sinX + zr * cosX
        );
    }

    struct CubeFace
    {
        int corners[4];
        bool reversed;
    };

    const CubeFace cubeFaces[6] = {
        { { 0, 2, 6, 4 }, false }, // cara +z
        { { 1, 5, 7, 3 }, false }, // cara -z
        { { 1, 0, 2, 3 }, false }, // cara +x
        { { 4, 6, 7, 5 }, false }, // cara -x
        { { 1, 5, 4, 0 }, false }, // cara +y
        { { 3, 2, 6, 7 }, false }  // cara -y
    };

    const glm::vec2 faceUVs[4] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(0.0f, 1.0f)
    };

    const glm::vec3 cubeCenter{offsetX, offsetY, 0.0f};

    std::vector<Mesh::Vertex> cubeVertices;
    cubeVertices.reserve(24);
    std::vector<unsigned int> cubeIndices;
    cubeIndices.reserve(36);

    for (const CubeFace& face : cubeFaces)
    {
        bool reversed = face.reversed;

        const glm::vec3 p0 = projected[face.corners[0]];
        const glm::vec3 p1 = projected[face.corners[1]];
        const glm::vec3 p2 = projected[face.corners[2]];
        const glm::vec3 p3 = projected[face.corners[3]];

        glm::vec3 normal = glm::normalize(glm::cross(p1 - p0, p2 - p0));
        if (glm::dot(normal, p0 - cubeCenter) < 0.0f)
        {
            normal = -normal;
            reversed = !reversed;
        }

        const unsigned int base = static_cast<unsigned int>(cubeVertices.size());
        const glm::vec3 corners[4] = { p0, p1, p2, p3 };

        for (int i = 0; i < 4; i++)
        {
            cubeVertices.push_back(Mesh::Vertex{
                corners[i].x, corners[i].y, corners[i].z,
                normal.x, normal.y, normal.z,
                faceUVs[i].x, faceUVs[i].y
            });
        }

        if (reversed)
        {
            cubeIndices.push_back(base + 0);
            cubeIndices.push_back(base + 2);
            cubeIndices.push_back(base + 1);
            cubeIndices.push_back(base + 0);
            cubeIndices.push_back(base + 3);
            cubeIndices.push_back(base + 2);
        }
        else
        {
            cubeIndices.push_back(base + 0);
            cubeIndices.push_back(base + 1);
            cubeIndices.push_back(base + 2);
            cubeIndices.push_back(base + 0);
            cubeIndices.push_back(base + 2);
            cubeIndices.push_back(base + 3);
        }
    }

    Mesh cube(cubeVertices, cubeIndices);
    Logger::Info("Cube created successfully");

    Scene scene;

    auto& cubeLeft = scene.AddGameObject("Cube_Left");
    cubeLeft.GetTransform().SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    cubeLeft.GetTransform().SetRotationEulerDegrees(glm::vec3(0.0f, 45.0f, 0.0f));

    auto& cubeRight = scene.AddGameObject("Cube_Right");
    cubeRight.GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    cubeRight.GetTransform().SetScale(glm::vec3(0.7f, 0.7f, 0.7f));

    ModelImporter importer;
    auto importedModel = std::make_shared<const Model>(
        importer.Load(AssetPath("Models/Tests/material_test.obj").string().c_str())
    );
    Logger::Info("Model loaded: " + importedModel->Name + " meshes=" +
                 std::to_string(importedModel->Meshes.size()) +
                 " materials=" + std::to_string(importedModel->Materials.size()));

    std::vector<std::shared_ptr<Material>> importedMaterials;
    importedMaterials.reserve(importedModel->Materials.size());
    for (const ImportedMaterial& info : importedModel->Materials)
    {
        std::shared_ptr<Texture2D> importedTexture = nullptr;
        if (!info.TexturePath.empty())
        {
            importedTexture = assets.LoadTexture2D(info.TexturePath);
        }

        auto importedMaterial = std::make_shared<Material>(shader, importedTexture);
        importedMaterial->SetBaseColor(info.DiffuseColor);
        importedMaterials.push_back(std::move(importedMaterial));
    }

    auto characterModel = std::make_shared<const Model>(
        importer.Load(AssetPath("Models/Yanagi/Yanagi.fbx").string().c_str())
    );
    Logger::Info("Model loaded: " + characterModel->Name + " meshes=" +
                 std::to_string(characterModel->Meshes.size()) +
                 " materials=" + std::to_string(characterModel->Materials.size()));

    std::vector<std::shared_ptr<Material>> characterMaterials;
    characterMaterials.reserve(characterModel->Materials.size());
    for (const ImportedMaterial& info : characterModel->Materials)
    {
        std::shared_ptr<Texture2D> characterTexture = nullptr;
        if (!info.TexturePath.empty())
        {
            characterTexture = assets.LoadTexture2D(info.TexturePath);
        }

        auto characterMaterial = std::make_shared<Material>(shader, characterTexture);
        characterMaterial->SetBaseColor(info.DiffuseColor);
        characterMaterials.push_back(std::move(characterMaterial));
    }

    auto& materialInstLeft = scene.AddGameObject("MaterialInst_Left");
    materialInstLeft.GetTransform().SetPosition(glm::vec3(-2.5f, 0.4f, -3.0f));
    materialInstLeft.GetTransform().SetScale(glm::vec3(0.9f, 0.9f, 0.9f));

    auto& materialInstRight = scene.AddGameObject("MaterialInst_Right");
    materialInstRight.GetTransform().SetPosition(glm::vec3(2.5f, 0.4f, -3.0f));
    materialInstRight.GetTransform().SetScale(glm::vec3(0.9f, 0.9f, 0.9f));

    auto& characterInst = scene.AddGameObject("CharacterInst");
    characterInst.GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, -4.0f));
    characterInst.GetTransform().SetRotationEulerDegrees(glm::vec3(0.0f, 0.0f, 0.0f));
    characterInst.GetTransform().SetScale(glm::vec3(0.01f, 0.01f, 0.01f));

    ModelRenderer materialRendererA(importedModel, importedMaterials);
    ModelRenderer materialRendererB(importedModel, importedMaterials);
    ModelRenderer characterRenderer(characterModel, characterMaterials);

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
        renderer.Draw(mesh, materialTriangle);

        for (const GameObject* object : { &cubeLeft, &cubeRight })
        {
            const Material& material = (object == &cubeLeft) ? materialLeft : materialRight;

            shader->SetMat4("u_Model", scene.GetWorldMatrix(*object));
            renderer.Draw(cube, material);
        }

        materialRendererA.Draw(scene.GetWorldMatrix(materialInstLeft), renderer);
        materialRendererB.Draw(scene.GetWorldMatrix(materialInstRight), renderer);
        characterRenderer.Draw(scene.GetWorldMatrix(characterInst), renderer);

        window.SwapBuffers();
    }
}

void Application::Update(float deltaTime)
{
    m_Time += deltaTime;
}

