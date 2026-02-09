#pragma once 

#include <bitset>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "type.hpp"
#include "window.hpp"

// 2D grid with only 2 states
class Grid2D
{
    protected:    
        std::bitset <GRID_WIDTH*GRID_HEIGHT> m_current_grid;
        std::bitset <GRID_WIDTH*GRID_HEIGHT> m_next_grid;
        Window& m_window;
        SDL_Color m_cell_color;
        Grid2DPosition m_last_cell; // Last interacted cell with the mouse
        PixelPosition m_grid_margin;
        int m_cell_size;

        public:
            Grid2D(Window& window, const SDL_Color& cell_color) : 
            m_window(window), m_cell_color(cell_color)
            {
                const int window_width = window.GetWidth();
                const int window_height = window.GetHeight();
                const int max_size_x = window_width/GRID_WIDTH/2; // Maximum cell size to fill half window width
                const int max_size_y = window_height/GRID_HEIGHT; // Maximum cell size to fill window height 
                if (max_size_x < max_size_y){
                    m_cell_size = max_size_x;
                }else{
                    m_cell_size = max_size_y;
                }
                const int grid_size_x = GRID_WIDTH*m_cell_size;
                const int grid_size_y = GRID_HEIGHT*m_cell_size;
                // m_grid_margin.x = window_width/2 - grid_size_x/2;
                m_grid_margin.x = 0;
                m_grid_margin.y = window_height/2 - grid_size_y/2;

                std::srand(std::time({}));
                m_last_cell = {-1, -1}; // Unvalid cell
            }
            
            PixelPosition GetMargin() const { 
                return m_grid_margin;
            }

            std::bitset<GRID_WIDTH*GRID_HEIGHT> GetGrid() const {
                return m_current_grid;
            }

            int GetWidth() const {
                return GRID_WIDTH;
            }

            int GetHeight() const {
                return GRID_HEIGHT;
            }

            int GetCellSize() const {
                return m_cell_size;
            }
            
            void Empty(){ // Set each bit to 0
                m_current_grid.reset();
            }

            void Fill(){ // Set each bit to 1
                m_current_grid.set();
            }

            void Randomize(){
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    if (rand()%2) m_current_grid.set(i); 
                    else m_current_grid.reset(i);
                } 
            }

            int GetNrNeighbor(const size_t cell_index, const int range=1) const {
                unsigned int neighbor = 0;
                const int cell_line = cell_index/GRID_WIDTH; 
                const int cell_column = cell_index%GRID_WIDTH;
                
                for (int x = -range ; x <= range ; x++){
                    for (int y = -range ; y <= range ; y++){
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
            
            void Draw() const{
                m_window.SetRenderColor(m_cell_color);
                SDL_Renderer* window_renderer = m_window.GetRenderer();
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    if (m_current_grid[i]){
                        const int line = (GRID_HEIGHT-1) - i/GRID_WIDTH; 
                        const int column = (GRID_WIDTH-1) - i%GRID_WIDTH;
                        SDL_Rect cell_rect = {m_grid_margin.x + column*m_cell_size, m_grid_margin.y + line*m_cell_size, m_cell_size, m_cell_size};
                        SDL_RenderFillRect(window_renderer, &cell_rect);
                    }
                }
            }

            void Set(const PixelPosition& mouse){
                const int column = (GRID_WIDTH-1) - (mouse.x - m_grid_margin.x) / m_cell_size; 
                const int line = (GRID_HEIGHT-1) - (mouse.y - m_grid_margin.y) / m_cell_size;
                if (column == m_last_cell.x && line == m_last_cell.y)
                    return;
                if (line < 0 || line >= GRID_HEIGHT || column < 0 || column >= GRID_WIDTH)
                    return;
                m_current_grid.flip(line*GRID_WIDTH+column); 
                m_last_cell = {column, line};
            }

            virtual void Update() = 0;
};

// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
class GameOfLife : public Grid2D 
{
    private:
        const int m_birth, m_underpopulation, m_overpopulation;

    public :
        GameOfLife(Window& window, const SDL_Color& cell_color, const int birth, const int underpopulation, const int overpopulation):
        Grid2D(window, cell_color), m_birth(birth), m_underpopulation(underpopulation), m_overpopulation(overpopulation)
        {
            Randomize();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                const int nr_neighbor = GetNrNeighbor(i);
                if (m_current_grid[i]){ // Living cell 
                    if (nr_neighbor < m_underpopulation || nr_neighbor > m_overpopulation)
                        m_next_grid.reset(i);
                    else m_next_grid.set(i);
                }else{ // Dead cell
                    if (nr_neighbor == m_birth)
                        m_next_grid.set(i);
                    else m_next_grid.reset(i);
                }
            }
            m_current_grid = m_next_grid;    
        }
};

// https://en.wikipedia.org/wiki/Langton%27s_ant
class LangtonAnt : public Grid2D
{
    private:
        Grid2DPosition m_ant_position;
        unsigned int m_ant_direction;
        Grid2DPosition m_directions[4];
        
        void TurnRight(){
            m_ant_direction = (m_ant_direction+1)%4;
        }

        void TurnLeft(){
            m_ant_direction = (m_ant_direction+3)%4;
        }
        
    public:
        LangtonAnt(Window& window, const SDL_Color& cell_color, const Grid2DPosition initial_position, const unsigned int initial_direction=3):
        Grid2D(window, cell_color), m_ant_position(initial_position), m_ant_direction(initial_direction)
        {
            m_directions[0] = {0, 1}; m_directions[1] = {1, 0}; m_directions[2] = {0, -1}; m_directions[3] = {-1, 0};
            Empty();
        }

        void Update() override {
            const int index = m_ant_position.y * GRID_WIDTH + m_ant_position.x;
            if (m_current_grid[index]){
                TurnRight();
            }else{
                TurnLeft();
            }
            m_ant_position += m_directions[m_ant_direction]; // Move forward one unit
            m_current_grid[index].flip();
        }
};