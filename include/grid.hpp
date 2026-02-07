#pragma once 

#include <bitset>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "type.hpp"
#include "window.hpp"

#define GRID_WIDTH 32
#define GRID_HEIGHT 32

#define BIRTH 3
#define UNDERPOPULATION 2
#define OVERPOPULATION 3

class Grid2D
{
    private:    
        std::bitset <GRID_WIDTH*GRID_HEIGHT> m_current_grid;
        std::bitset <GRID_WIDTH*GRID_HEIGHT> m_next_grid;
        Window& m_window;
        SDL_Color m_cell_color;
        std::pair<int, int> m_last_cell; // Last interacted cell with the mouse

        public:
            Grid2D(Window& window, const SDL_Color& cell_color) : m_window(window), m_cell_color(cell_color){
                std::srand(std::time({}));
                Randomize();
                m_last_cell = {-1, -1}; // Unvalid cell
            }

            void Empty(){ // Set each bit to 0
                m_current_grid.reset();
                m_next_grid.reset();
            }

            void Fill(){ // Set each bit to 1
                m_current_grid.set();
                m_next_grid.set();
            }

            void Randomize(){
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    if (rand()%2) m_current_grid.set(i); 
                    else m_current_grid.reset(i);
                } 
            }

            unsigned int GetNrNeighbor(const size_t cell_index) const {
                unsigned int neighbor = 0;
                const int cell_line = cell_index/GRID_WIDTH; 
                const int cell_column = cell_index%GRID_WIDTH;
                
                for (int x = -1 ; x <= 1 ; x++){
                    for (int y = -1 ; y <= 1 ; y++){
                        if (x == 0 && y == 0)
                            continue;

                        const int neighbor_line = cell_line+y;
                        const int neighbor_column = cell_column+x;
                        if (neighbor_line < 0 || neighbor_line >= GRID_HEIGHT || neighbor_column < 0 || neighbor_column >= GRID_WIDTH)
                            continue;
                        if (m_current_grid[neighbor_line * GRID_WIDTH + neighbor_column]) neighbor++;
                    }
                }
                return neighbor;
            }
            
            void Update(){
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    const unsigned int nr_neighbor = GetNrNeighbor(i);
                    if (m_current_grid[i]){ // Living cell 
                        if (nr_neighbor < UNDERPOPULATION || nr_neighbor > OVERPOPULATION)
                            m_next_grid.reset(i);
                        else m_next_grid.set(i);
                    }else{ // Dead cell
                        if (nr_neighbor == BIRTH)
                            m_next_grid.set(i);
                        else m_next_grid.reset(i);
                    }
                }
                m_current_grid = m_next_grid;    
            }
            
            void Draw() const{
                m_window.SetRenderColor(m_cell_color);
                SDL_Renderer* window_renderer = m_window.GetRenderer();
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    if (m_current_grid[i]){
                        const int line = (GRID_HEIGHT-1) - i/GRID_WIDTH; 
                        const int column = (GRID_WIDTH-1) - i%GRID_WIDTH;
                        SDL_Rect cell_rect = {column*32, line*32, 32, 32};
                        SDL_RenderFillRect(window_renderer, &cell_rect);
                    }
                }
            }

            void Set(const MousePosition& mouse){
                const int column = (GRID_WIDTH-1) - mouse.x/32; // Cell size 
                const int line = (GRID_HEIGHT-1) - mouse.y/32; // Cell size
                if (column == m_last_cell.first && line == m_last_cell.second)
                    return;
                if (line < 0 || line >= GRID_HEIGHT || column < 0 || column >= GRID_WIDTH)
                    return;
                m_current_grid.flip(line*GRID_WIDTH+column); 
                m_last_cell = {column, line};
            }
};