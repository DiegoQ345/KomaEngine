find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(glad STATIC
    ${PROJECT_SOURCE_DIR}/Dependencies/GLAD/src/gl.c
)

target_include_directories(glad PUBLIC
    ${PROJECT_SOURCE_DIR}/Dependencies/GLAD/include
)

# Engine
add_library(Engine STATIC
    Core/Application.cpp
    Core/AssetPaths.cpp
    Core/Logger.cpp
    Platform/Input/Input.cpp
    Platform/Window/Window.cpp
    Renderer/OpenGL/IndexBuffer.cpp
    Renderer/OpenGL/Mesh.cpp
    Renderer/OpenGL/Renderer.cpp
    Renderer/OpenGL/Shader.cpp
    Renderer/OpenGL/VertexArray.cpp
    Renderer/OpenGL/VertexBuffer.cpp
)

target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Engine
    PUBLIC
        glfw
        glm::glm
    PRIVATE
        glad
)
