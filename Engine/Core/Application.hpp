#pragma once

class Application
{
public:
    Application() = default;
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Run();

private:
    void Update(float deltaTime);

    float m_Time = 0.0f;
};
