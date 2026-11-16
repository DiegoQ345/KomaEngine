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
    Core/Logger.cpp
    Platform/Input/Input.cpp
    Platform/Window/Window.cpp
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
