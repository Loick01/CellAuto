#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "grid.hpp"
#include "window.hpp"

class ImGuiLayer
{
    private:
        void SetFrame() const;
        void Init(Window& window);

        SDL_Renderer* m_windowRenderer;

        Grid& m_grid;
        SDL_Color& m_cellColor;
        SDL_Color& m_bgColor;
        float& m_stepTimer;
        int& m_gridWidth;
        int& m_gridHeight;

    public:
        ImGuiLayer(Window& window, float& stepTimer, SDL_Color& bgColor, Grid& grid);
        ~ImGuiLayer();

        void Draw() const;
};