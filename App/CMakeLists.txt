find_package(glfw3 CONFIG REQUIRED)

add_executable(App
    main.cpp
)

target_link_libraries(App PRIVATE
    Engine
    glad
    glfw
)
