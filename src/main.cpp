#include <iostream>
#include <memory>

#include "event.hpp"
#include "grid.hpp"
#include "gui.hpp"
#include "time.hpp"
#include "window.hpp"

Window window("CellAuto", {50,50,50});
float stepTimer = 0.5f;

Grid2DEventController eventController;
const int gridWidth = 64;
const int gridHeight = 64;
std::unique_ptr<Grid> ca = std::make_unique<GameOfLife>(window, gridWidth, gridHeight, SDL_Color{25, 240, 50}, 3, 2, 3);

ImGuiLayer gui(window, stepTimer, window.GetBackgroundColor(), ca.get());

void SwitchAutomata(const SetAutomata e)
{
    switch(e){
        case SetAutomata::Elementary: {
            ca = std::make_unique<Grid1D>(window, gridWidth, gridHeight, SDL_Color{25, 240, 50}, 150);
            break;
        } 
        case SetAutomata::GoL: {
            ca = std::make_unique<GameOfLife>(window, gridWidth, gridHeight, SDL_Color{25, 240, 50}, 3, 2, 3);
            break;
        }
        case SetAutomata::Langton: {
            ca = std::make_unique<LangtonAnt>(window, gridWidth, gridHeight, SDL_Color{25, 240, 50}, Grid2DPosition{gridWidth/2, gridHeight/2}, 1);
            break;
        }
        default:{
            std::cout << "Undefined\n"; // Will throw an error
            break;
        }
    }
    gui.SetGrid(ca.get());
}

int main()
{
    
    Grid2DEventController eventController;

    bool gameloop = true;
    Time time;
    float timer = stepTimer;

    gui.AddCallback([](SetAutomata e){SwitchAutomata(e);});
    
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