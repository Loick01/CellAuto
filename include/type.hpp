#pragma once 

// https://gist.github.com/beached/38a4ae52fcadfab68cb6de05403fa393

struct PixelPosition
{
    int x, y;

    PixelPosition operator-(PixelPosition const& rhs) const{
        return PixelPosition{x-rhs.x, y-rhs.y};
    }

    PixelPosition& operator+=(PixelPosition const& rhs){
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    PixelPosition& operator/(const int rhs){
        x /= rhs;
        y /= rhs;
        return *this;
    }
};

struct Grid2DPosition
{
    int x, y;

    Grid2DPosition operator+(Grid2DPosition const& rhs) const{
        return Grid2DPosition{x+rhs.x, y+rhs.y};
    }

    Grid2DPosition operator-(Grid2DPosition const& rhs) const{
        return Grid2DPosition{x-rhs.x, y-rhs.y};
    }

    Grid2DPosition& operator+=(Grid2DPosition const& rhs){
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};