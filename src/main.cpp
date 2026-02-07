#include <iostream>

#include "event.hpp"
#include "grid.hpp"
#include "time.hpp"
#include "window.hpp"

#define UPDATE_TIMER 0.1

int main()
{
    Window window("GameOfLife", {50,50,50});
    GameOfLifeEventController golEventController;
    Grid2D gol(window, {25, 240, 50});
    bool gameloop = true;
    Time time;
    float timer = UPDATE_TIMER;

    while(gameloop){
        window.ClearRenderer();
        golEventController.PollAllEvents();
        gameloop = golEventController.HandleWindowEvents();
        golEventController.HandleEvents();

        if (golEventController.GetIsPaused()){
            if (golEventController.GetIsSet()){
                gol.Set(golEventController.GetMouse());
            }
        }else{
            time.Update();
            timer -= time.m_delta_time;
            if (timer < 0.0){
                timer = UPDATE_TIMER;
                gol.Update();
            }
        }

        gol.Draw();
        window.UpdateRender();
    }

    return 0;
}