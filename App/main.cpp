#include <exception>

#include "Core/Application.hpp"
#include "Core/Logger.hpp"

#include <string>

int main()
{
    try
    {
        Application app;
        app.Run();
    }
    catch (const std::exception& exception)
    {
        Logger::Error(std::string("Engine error: ") + exception.what());

        return -1;
    }

    return 0;
}
