#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "window.hpp"

class ImGuiLayer
{
    private:
        SDL_Renderer* m_window_renderer;
        void SetFrame() const;
        void Init(Window& window);

    public:
        ImGuiLayer(Window& window);
        ~ImGuiLayer();

        void Draw() const;
};