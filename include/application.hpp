#pragma once

#include <memory>
#include <stdexcept>

#include "camera.hpp"
#include "event.hpp"
#include "grid.hpp"
#include "gui.hpp"
#include "time.hpp"
#include "window.hpp"

class Application
{
    public:
        // WARNING : Initialization order of member variables in a class is in the order of their declaration in the class 
        Window m_window;
        std::unique_ptr<Grid> m_grid;
        Camera m_camera;
        GridEventController m_eventController;
        ImGuiLayer m_gui;
        Time m_time;
        float m_stepTimer;
        float m_timer;

        Application();
        bool Run(); // Gameloop
        void SwitchAutomata(const SetAutomata e);
};