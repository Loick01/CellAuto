#include "window.hpp"

Window::Window(const std::string& title, const SDL_Color bg_color) :
    m_title(title), m_window(nullptr), m_renderer(nullptr), m_bg_color(bg_color)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) std::cout << "Failed to initialize SDL library\n";
    CreateWindow();
}

Window::~Window()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::CreateWindow()
{
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(!m_window) std::cout << "Failed to initialize SDL window\n";
    SDL_GetWindowSize(m_window, &m_width, &m_height);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) std::cout << "Failed to create a renderer for the window\n";
}

SDL_Renderer* Window::GetRenderer() const
{
    return m_renderer;
}

SDL_Window* Window::GetWindow() const
{
    return m_window;
}

SDL_Color& Window::GetBackgroundColor()
{
    return m_bg_color;
}

int Window::GetWidth() const
{
    return m_width;
}

int Window::GetHeight() const
{
    return m_height;
}

bool Window::HasError() const
{ 
    return !m_window || !m_renderer;
}

void Window::ClearRenderer()
{
    SetRenderColor(m_bg_color);
    SDL_RenderClear(m_renderer);
}

void Window::UpdateRender() const
{
    SDL_RenderPresent(m_renderer);
}

void Window::SetRenderColor(const SDL_Color& color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 255);
}

void Window::HideCursor()
{
    SDL_ShowCursor(SDL_DISABLE); 
}