#include <iostream>
#include <exception>

#include "Core/Application.hpp"

int main()
{
    try
    {
        Application app;
        app.Run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Engine error: "
                  << exception.what() << '\n';

        return -1;
    }

    return 0;
}
