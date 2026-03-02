#include <iostream>

#include "application.hpp"

int main()
{
    Application app;
    bool isRunning = true;
    
    try {
        while(isRunning){
            isRunning = app.Run();
        }
    } catch (const std::invalid_argument e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return 0;
}