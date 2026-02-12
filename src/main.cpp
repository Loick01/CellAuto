#include <iostream>
#include <memory>

#include "event.hpp"
#include "grid.hpp"
#include "gui.hpp"
#include "time.hpp"
#include "window.hpp"

int main()
{
    float stepTimer = 0.5f;
    Window window("CellAuto", {50,50,50});
    
    Grid2DEventController eventController;
    
    std::unique_ptr<Grid> ca = std::make_unique<GameOfLife>(window, SDL_Color{25, 240, 50}, 3, 2, 3);
    //std::unique_ptr<Grid> ca = std::make_unique<LangtonAnt>(window, SDL_Color{25, 240, 50}, Grid2DPosition{GRID_WIDTH/2, GRID_HEIGHT/2}, 1);
    //std::unique_ptr<Grid> ca = std::make_unique<Grid1D>(window, SDL_Color{25, 240, 50}, 150);

    bool gameloop = true;
    Time time;
    float timer = stepTimer;

    ImGuiLayer gui(window, stepTimer, window.GetBackgroundColor(), ca->GetCellColor(), *ca);
    
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
                timer = stepTimer;
                ca->Update();
            }
        }

        ca->Draw();
        gui.Draw();
        window.UpdateRender();
    }

    return 0;
}