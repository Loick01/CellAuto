#include <iostream>

#include "event.hpp"
#include "grid.hpp"
#include "result.hpp"
#include "time.hpp"
#include "window.hpp"

#define UPDATE_TIMER 0.001

int main()
{
    Window window("GameOfLife", {50,50,50});
    
    Grid2DEventController eventController;
    
    // GameOfLife gol(window, {25, 240, 50}, 3, 2, 3);
    LangtonAnt la(window, {25, 240, 50}, {GRID_WIDTH/2, GRID_HEIGHT/2});

    // TextureResult result(window.GetRenderer(), gol.GetMargin(), gol.GetWidth(), gol.GetHeight(), gol.GetCellSize());
    // result.Update(gol.GetGrid());

    bool gameloop = true;
    Time time;
    float timer = UPDATE_TIMER;

    while(gameloop){
        window.ClearRenderer();
        eventController.PollAllEvents();
        gameloop = eventController.HandleWindowEvents();
        eventController.HandleEvents();

        if (eventController.GetIsPaused()){
            if (eventController.GetIsSet()){
                la.Set(eventController.GetMouse());
            }
        }else{
            time.Update();
            timer -= time.m_delta_time;
            if (timer <= 0.0){
                timer = UPDATE_TIMER;
                la.Update();
                // result.Update(gol.GetGrid());
            }
        }

        la.Draw();
        float t = timer == UPDATE_TIMER ? 0. : 1.0 - timer/UPDATE_TIMER;
        // result.Draw(t);
        window.UpdateRender();
    }

    return 0;
}