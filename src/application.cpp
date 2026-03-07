#include "application.hpp"

Application::Application():
    m_window("CellAuto", {50, 50, 50}),
    m_grid(std::make_unique<GameOfLife>(m_window, std::initializer_list<int>{3}, std::initializer_list<int>{2, 3})),
    m_camera(m_window, m_grid.get()), m_stepTimer(0.5f),
    m_eventController(m_camera),
    m_gui(m_window, m_stepTimer, m_window.GetBackgroundColor(), m_grid.get(), m_camera)
{
    m_timer = m_stepTimer;
    m_gui.AddCallback([this](SetAutomata e){SwitchAutomata(e);});
}

bool Application::Run()
{
    m_window.ClearRenderer();
    m_eventController.PollAllEvents();
    const bool loop = m_eventController.HandleWindowEvents();
    m_eventController.HandlePolledEvents(); // Should not be here ?
    m_eventController.HandleStateEvents();

    m_camera.Move(m_eventController.GetIsMoving(), m_eventController.GetMouse());
    
    if (m_eventController.GetIsSet())
        m_grid->Set(m_eventController.GetMouse(), m_camera.GetPosition(), m_camera.GetZoom(), m_gui.GetSelectedState(), m_gui.GetSetSize());
    if (!m_eventController.GetIsPaused()){
        m_time.Update();
        m_timer -= m_time.m_delta_time;
        if (m_timer <= 0.0){
            m_timer = m_stepTimer;
            m_grid->Update();
        }
    }

    m_grid->Draw(m_camera.GetPosition(), m_camera.GetZoom());
    m_gui.Draw();
    m_window.UpdateRender();
    return loop;
}

void Application::SwitchAutomata(const SetAutomata e)
{
    switch(e){
        case SetAutomata::Elementary: {
            m_grid = std::make_unique<Grid1D>(m_window, SDL_Color{25, 240, 50}, 150);
            break;
        } 
        case SetAutomata::GoL: {
            m_grid = std::make_unique<GameOfLife>(m_window, 
                std::initializer_list<int>{3}, std::initializer_list<int>{2, 3});
            break;
        }
        case SetAutomata::Langton: {
            m_grid = std::make_unique<LangtonAnt>(m_window, Grid2DPosition{32, 32}, 1);
            break;
        }
        case SetAutomata::GreenbergHastings: {
            m_grid = std::make_unique<GreenbergHastings>(m_window);
            break;
        }
        case SetAutomata::ForestFire: {
            m_grid = std::make_unique<ForestFire>(m_window, 0.5, 0.01);
            break;
        }
        case SetAutomata::Cyclic: {
            m_grid = std::make_unique<Cyclic>(m_window, 10, 1);
            break;
        }
        case SetAutomata::Hodgepodge: {
            m_grid = std::make_unique<Hodgepodge>(m_window, 100, 3, 3, 28);
            break;
        }
        case SetAutomata::AbelianSandpile: {
            m_grid = std::make_unique<AbelianSandpile>(m_window, 4);
            break;
        }
        case SetAutomata::Wireworld: {
            m_grid = std::make_unique<Wireworld>(m_window);
            break;
        }
        case SetAutomata::FallingSand: {
            m_grid = std::make_unique<FallingSand>(m_window);
            break;
        }
        default:
            throw std::invalid_argument("Unknown automata");
    }
    m_gui.SetGrid(m_grid.get());
}