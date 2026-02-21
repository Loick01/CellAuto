#include <iostream>
#include <memory>

#include "camera.hpp"
#include "event.hpp"
#include "grid.hpp"
#include "gui.hpp"
#include "time.hpp"
#include "window.hpp"

// Should be in Controller class
// -----------------------------
Window window("CellAuto", {50,50,50});
float stepTimer = 0.5f;

GridEventController eventController;
const int gridWidth = 64;
const int gridHeight = 64;
std::unique_ptr<Grid> ca = std::make_unique<GameOfLife>(window, gridWidth, gridHeight, 
    std::initializer_list<int>{3}, std::initializer_list<int>{2, 3});
Camera camera(window, ca.get());

ImGuiLayer gui(window, stepTimer, window.GetBackgroundColor(), ca.get(), camera);
// -----------------------------

void SwitchAutomata(const SetAutomata e)
{
    switch(e){
        case SetAutomata::Elementary: {
            ca = std::make_unique<Grid1D>(window, gridWidth, gridHeight, SDL_Color{25, 240, 50}, 150);
            break;
        } 
        case SetAutomata::GoL: {
            ca = std::make_unique<GameOfLife>(window, gridWidth, gridHeight, 
                std::initializer_list<int>{3}, std::initializer_list<int>{2, 3});
            break;
        }
        case SetAutomata::Langton: {
            ca = std::make_unique<LangtonAnt>(window, gridWidth, gridHeight, Grid2DPosition{gridWidth/2, gridHeight/2}, 1);
            break;
        }
        case SetAutomata::GreenbergHastings: {
            ca = std::make_unique<GreenbergHastings>(window, gridWidth, gridHeight);
            break;
        }
        case SetAutomata::ForestFire: {
            ca = std::make_unique<ForestFire>(window, gridWidth, gridHeight, 0.5, 0.01);
            break;
        }
        case SetAutomata::Cyclic: {
            ca = std::make_unique<Cyclic>(window, gridWidth, gridHeight, 10, 1);
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
    
    GridEventController eventController;

    bool gameloop = true;
    Time time;
    float timer = stepTimer;

    gui.AddCallback([](SetAutomata e){SwitchAutomata(e);});
    
    while(gameloop){
        window.ClearRenderer();
        eventController.PollAllEvents();
        gameloop = eventController.HandleWindowEvents();
        eventController.HandleEvents();

        camera.Move(eventController.GetIsMoving(), eventController.GetMouse());
        if (eventController.GetIsPaused()){
            if (eventController.GetIsSet()){
                ca->Set(eventController.GetMouse(), camera.GetPosition());
            }
        }else{
            time.Update();
            timer -= time.m_delta_time;
            if (timer <= 0.0){
                timer = stepTimer;
                ca->Update();
            }
        }

        ca->Draw(camera.GetPosition());
        gui.Draw();
        window.UpdateRender();
    }

    return 0;
}