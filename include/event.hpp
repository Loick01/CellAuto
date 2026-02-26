#pragma once 

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "camera.hpp"
#include "gui.hpp"
#include "type.hpp"

class ActionController
{
    public:
        virtual bool IsPauseAction() = 0;
        virtual bool IsSetAction() = 0;
        virtual bool IsMovingCameraAction() = 0;
        virtual void GetActions(PixelPosition& current_mouse) = 0;
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
        bool IsMovingCameraAction() override;
        void GetActions(PixelPosition& current_mouse) override;
        void GetKeyboardActions();
        void GetMouseActions(PixelPosition& current_mouse);
};

class EventController
{
    protected:
        std::vector<SDL_Event> m_events;
        bool m_break_events;
        
    public:
        bool HandleWindowEvents() const;
        virtual void HandleStateEvents() = 0;
        virtual void HandlePolledEvents() = 0;
        void PollAllEvents();
};

class GridEventController: public EventController
{
    private:
        Camera& m_camera;
        ActionController* m_action_controller;
        PixelPosition m_current_mouse;
        bool m_is_paused;
        bool m_is_set;
        bool m_is_moving;

    public:
        GridEventController(Camera& camera);
        PixelPosition GetMouse() const;
        bool GetIsPaused() const;
        bool GetIsSet() const;
        bool GetIsMoving() const;
        void HandleStateEvents() override;
        void HandlePolledEvents() override;
};