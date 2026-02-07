#pragma once 

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "type.hpp"

class ActionController
{
    public:
        virtual bool IsPauseAction() = 0;
        virtual bool IsSetAction() = 0;
        virtual void GetActions(MousePosition& current_mouse) = 0;
};

class KeyboardMouseActionController : public ActionController
{
    private:
        const Uint8* m_keyboard_state;
        Uint32 m_mouse_state;
        bool m_trigger_pause;

    public:
        KeyboardMouseActionController(); 
        bool IsPauseAction() override;
        bool IsSetAction() override;
        void GetActions(MousePosition& current_mouse) override;
        void GetKeyboardActions();
        void GetMouseActions(MousePosition& current_mouse);
};

class EventController
{
    protected:
        std::vector<SDL_Event> m_events;
        
    public:
        bool HandleWindowEvents() const;
        virtual void HandleEvents() = 0;
        void PollAllEvents();
};

class GameOfLifeEventController: public EventController
{
    private:
        ActionController* m_action_controller;
        MousePosition m_current_mouse;
        bool m_is_paused;
        bool m_is_set;

    public:
        GameOfLifeEventController();
        MousePosition GetMouse() const;
        bool GetIsPaused() const;
        bool GetIsSet() const;
        void HandleEvents() override;
};