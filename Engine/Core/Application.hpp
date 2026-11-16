#pragma once

class Input;

class Application
{
public:
    Application() = default;
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Run();

private:
    void Update(float deltaTime, const Input& input);

    float m_Time = 0.0f;
    float m_Red = 0.08f;
    float m_Green = 0.10f;
    float m_LogTimer = 0.0f;

    int m_FramebufferWidth = -1;
    int m_FramebufferHeight = -1;
};
