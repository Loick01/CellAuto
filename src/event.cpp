#include "event.hpp"

KeyboardMouseActionController::KeyboardMouseActionController()
{
    m_trigger_pause = false;
}

void KeyboardMouseActionController::GetActions(MousePosition& current_mouse)
{
   GetKeyboardActions();
   GetMouseActions(current_mouse);
}

void KeyboardMouseActionController::GetKeyboardActions()
{
    m_keyboard_state = SDL_GetKeyboardState(NULL);
}

void KeyboardMouseActionController::GetMouseActions(MousePosition& current_mouse)
{
    m_mouse_state = SDL_GetMouseState(&current_mouse.x, &current_mouse.y);
}

bool KeyboardMouseActionController::IsPauseAction() // Return true if the pause value must be inverted, false otherwise
{
    bool is_pressed = m_keyboard_state[SDL_SCANCODE_SPACE];
    if (!is_pressed) m_trigger_pause = false;
    if (is_pressed != m_trigger_pause) {
        m_trigger_pause = true;
        return true;
    }
    return false;
}

bool KeyboardMouseActionController::IsSetAction()
{
    return m_mouse_state & SDL_BUTTON_LMASK;
}

bool EventController::HandleWindowEvents() const
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_QUIT:
                return false;
            
            case SDL_KEYDOWN:
                SDL_Scancode event_scancode = event.key.keysym.scancode;
                if (event_scancode == SDL_SCANCODE_ESCAPE)
                    return false;
                break;
        }
    }
    return true;
}

void EventController::PollAllEvents()
{
    m_events.clear();
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        m_events.push_back(event);
    }
}

GameOfLifeEventController::GameOfLifeEventController()
{
    m_action_controller = new KeyboardMouseActionController();
    m_is_paused = false;
    m_is_set = false;
}

MousePosition GameOfLifeEventController::GetMouse() const
{
    return m_current_mouse;
}

bool GameOfLifeEventController::GetIsPaused() const
{
    return m_is_paused;
}

bool GameOfLifeEventController::GetIsSet() const
{
    return m_is_set;
}

void GameOfLifeEventController::HandleEvents()
{
    m_action_controller->GetActions(m_current_mouse);
    if (m_action_controller->IsPauseAction()){
        m_is_paused = !m_is_paused;
        return;
    }
    m_is_set = m_action_controller->IsSetAction(); // Add/Remove a cell
}