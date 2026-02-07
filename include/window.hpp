#pragma once

#include <iostream>
#include <string>

#include <SDL2/SDL.h>

class Window
{
    private:
        void CreateWindow();

        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_Color m_bg_color;
        int m_width;
        int m_height;
        const std::string m_title;

    public:
        Window(const std::string& title, const SDL_Color bg_color);
        ~Window();

        SDL_Renderer* GetRenderer() const;
        int GetWidth() const;
        int GetHeight() const;
        bool HasError() const;
        void ClearRenderer() const;
        void UpdateRender() const;
        void SetBackgroundColor(const SDL_Color bg_color);
        void HideCursor();
};