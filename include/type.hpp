#pragma once 

#define GRID_WIDTH 256
#define GRID_HEIGHT 256

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