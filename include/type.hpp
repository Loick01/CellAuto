#pragma once 

#define GRID_WIDTH 160
#define GRID_HEIGHT 90

struct PixelPosition
{
    int x, y;
};

struct Grid2DPosition
{
    int x, y;

    Grid2DPosition& operator+=(Grid2DPosition const& rhs){
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

inline float lerp(float a, float b, float t){
    return a+(b-a)*t;
}