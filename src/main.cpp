#include <iostream>
#include <memory>

#include "event.hpp"
#include "grid.hpp"
#include "result.hpp"
#include "time.hpp"
#include "window.hpp"

#define UPDATE_TIMER 0.1

int main()
{
    Window window("CellAuto", {50,50,50});
    
    Grid2DEventController eventController;
    
    //std::unique_ptr<Grid> ca = std::make_unique<GameOfLife>(window, SDL_Color{25, 240, 50}, 3, 1, 3);
    //std::unique_ptr<Grid> ca = std::make_unique<LangtonAnt>(window, SDL_Color{25, 240, 50}, Grid2DPosition{GRID_WIDTH/2, GRID_HEIGHT/2}, 1);
    std::unique_ptr<Grid> ca = std::make_unique<Grid1D>(window, SDL_Color{25, 240, 50}, 126);

    //TextureResult result(window, ca->GetMargin(), ca->GetWidth(), ca->GetHeight(), ca->GetCellSize());
    //result.Update(*ca);

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
                ca->Set(eventController.GetMouse());
            }
        }else{
            time.Update();
            timer -= time.m_delta_time;
            if (timer <= 0.0){
                timer = UPDATE_TIMER;
                ca->Update();
                //result.Update(*ca);
            }
        }

        ca->Draw();
        //float t = timer == UPDATE_TIMER ? 0. : 1.0 - timer/UPDATE_TIMER;
        //result.Draw(t);
        window.UpdateRender();
    }

    return 0;
}