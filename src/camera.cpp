#include "camera.hpp"

Camera::Camera(const Window& window, const Grid* grid):
    m_windowSize(PixelPosition{window.GetWidth(), window.GetHeight()}), m_zoom(1.f)
{
    LookAtGrid(grid->GetSize()/2);
}

PixelPosition Camera::GetPosition() const
{
    return m_position;
}

float Camera::GetZoom() const 
{
    return m_zoom;
}

void Camera::AddZoom(const float z)
{
    m_zoom += z;
}

void Camera::LookAtGrid(const PixelPosition& gridCenter)
{
    m_position.x = -1 * (m_windowSize.x/2 - gridCenter.x); 
    m_position.y = -1 * (m_windowSize.y/2 - gridCenter.y); 
}

void Camera::Move(const bool isMoving, const PixelPosition& mousePosition)
{
    if (isMoving){
        const PixelPosition diffPosition = m_lastMousePosition - mousePosition;
        m_position += diffPosition;
    }
    m_lastMousePosition = mousePosition;
}
