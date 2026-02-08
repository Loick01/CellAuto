#include <iostream>

#include "event.hpp"
#include "grid.hpp"
#include "result.hpp"
#include "time.hpp"
#include "window.hpp"

#define UPDATE_TIMER 0.5

int main()
{
    Window window("GameOfLife", {50,50,50});
    GameOfLifeEventController golEventController;
    Grid2D gol(window, {25, 240, 50});
    TextureResult result(window.GetRenderer(), gol.GetMargin(), gol.GetWidth(), gol.GetHeight(), gol.GetCellSize());
    result.Update(gol.GetGrid());
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
            if (timer <= 0.0){
                timer = UPDATE_TIMER;
                gol.Update();
                result.Update(gol.GetGrid());
            }
        }

        gol.Draw();
        float t = timer == UPDATE_TIMER ? 0. : 1.0 - timer/UPDATE_TIMER;
        result.Draw(t);
        window.UpdateRender();
    }

    return 0;
}