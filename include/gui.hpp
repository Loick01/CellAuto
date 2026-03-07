#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "camera.hpp"
#include "grid.hpp"
#include "notifier.hpp"
#include "window.hpp"

enum class SetAutomata
{
    Elementary, GoL, Langton, GreenbergHastings, ForestFire, Cyclic, Hodgepodge, AbelianSandpile, Wireworld, FallingSand
};

class ImGuiLayer : public Notifier<SetAutomata>
{
    private:
        void SetFrame();
        void Init(Window& window);

        SDL_Renderer* m_windowRenderer;

        Grid* m_grid;
        SDL_Color& m_bgColor;
        float& m_stepTimer;

        Camera& m_camera;
        int m_selectedAutomata;
        int m_selectedNbh;
        int m_selectedState; 
        int m_setSize;

    public:
        ImGuiLayer(Window& window, float& stepTimer, SDL_Color& bgColor, Grid* grid, Camera& camera);
        ~ImGuiLayer();

        int GetSelectedState() const;
        int GetSetSize() const;
        void SetGrid(Grid* grid);
        void Draw();
};