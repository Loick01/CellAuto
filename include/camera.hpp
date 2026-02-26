#pragma once

// #include <algorithm>
// #include <iostream>

#include "grid.hpp"
#include "type.hpp"
#include "window.hpp"

class Camera
{
    private:
        const PixelPosition m_windowSize;
        PixelPosition m_position;
        PixelPosition m_lastMousePosition; // Could be in EventController ?
        float m_zoom;

    public:
        Camera(const Window& window, const Grid* grid);
        
        PixelPosition GetPosition() const;
        float GetZoom() const;
        void AddZoom(const float z);
        void LookAtGrid(const PixelPosition& gridCenter);
        void Move(const bool isMoving, const PixelPosition& mousePosition);
};