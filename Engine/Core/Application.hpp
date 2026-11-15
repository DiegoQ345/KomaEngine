#pragma once

class Application
{
public:
    Application() = default;
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Run();
};
