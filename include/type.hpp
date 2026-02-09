#pragma once 

#define GRID_WIDTH 64
#define GRID_HEIGHT 64

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