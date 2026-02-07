#include <iostream>

#include "event.hpp"
#include "window.hpp"

int main()
{
    Window window("GameOfLife", {25,25,25});
    GameOfLifeEventController golEventController;
    bool gameloop = true;

    while(gameloop){
        window.ClearRenderer();
        golEventController.PollAllEvents();
        gameloop = golEventController.HandleWindowEvents();
        golEventController.HandleEvents();
        window.UpdateRender();
    }

    return 0;
}