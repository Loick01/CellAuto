#pragma once 

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "type.hpp"
#include "window.hpp"

class Grid
{
    protected: 
        Window& m_window;
        SDL_Color m_cellColor;
        PixelPosition m_gridMargin;
        Grid2DPosition m_lastCell; // Last interacted cell with the mouse (should not nbe here)
        int m_cellSize;
        int m_gridWidth;
        int m_gridHeight;

        Grid(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cell_color) :
        m_window(window), m_cellColor(cell_color), m_gridWidth(gridWidth), m_gridHeight(gridHeight)
        {
            ComputeSize();
        }
        
        PixelPosition GetMargin() const { 
            return m_gridMargin;
        }
        
        Grid2DPosition GetGridPositionFromMouse(const PixelPosition& mouse){
            const int column = (mouse.x - m_gridMargin.x) / m_cellSize; 
            const int line = (mouse.y - m_gridMargin.y) / m_cellSize;
            return Grid2DPosition{column, line};
        }

        bool IsGridPositionValid(const Grid2DPosition& position){
            if (position.x == m_lastCell.x && position.y == m_lastCell.y)
                return false;
            if (position.y < 0 || position.y >= m_gridHeight || position.y < 0 || position.y >= m_gridWidth)
                return false;
            m_lastCell = Grid2DPosition{position.x, position.y};
            return true;
        }

    public:
        virtual void Draw() const = 0;
        virtual void Update() = 0;
        virtual void Set(const PixelPosition& mouse) = 0;
        virtual void Empty() = 0;
        virtual void Fill() = 0;
        virtual void Randomize() = 0;
        virtual void Resize() = 0;

        void ComputeSize(){ // Will be called when the grid size is modified with ImGui
            const int window_width = m_window.GetWidth();
            const int window_height = m_window.GetHeight();
            const int max_size_x = window_width/m_gridWidth; // Maximum cell size to fill window width
            const int max_size_y = window_height/m_gridHeight; // Maximum cell size to fill window height 
            if (max_size_x < max_size_y) 
                m_cellSize = max_size_x;
            else 
                m_cellSize = max_size_y;
            const int grid_size_x = m_gridWidth*m_cellSize;
            const int grid_size_y = m_gridHeight*m_cellSize;
            m_gridMargin.x = window_width/2 - grid_size_x/2; // Center the grid on the window
            m_gridMargin.y = window_height/2 - grid_size_y/2;
        }
        
        SDL_Color* GetCellColor() {
            return &m_cellColor;
        }

        int* GetWidth() {
            return &m_gridWidth;
        }

        int* GetHeight() {
            return &m_gridHeight;
        }
};

// One dimensional grid, drawed by stacking them in a 2D grid
// https://en.wikipedia.org/wiki/Elementary_cellular_automaton
class Grid1D : public Grid 
{
    protected: 
        std::vector<uint8_t> m_grid;
        const uint8_t m_rule;
        size_t m_generation;

    public:
        Grid1D(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cell_color, const uint8_t rule):
        Grid(window, gridWidth, gridHeight, cell_color), m_generation(0), m_rule(rule) {
            Resize();
            Initial();
        }

        void Resize() override {
            m_grid.assign(m_gridWidth*m_gridHeight, 1);
        }

        void Initial() {
            m_grid[m_generation * m_gridWidth + m_gridWidth / 2] = 1;
        }
        
        uint8_t GetNeighbors(const size_t cell_index) const{
            size_t rightIndex = (cell_index+1)%m_gridWidth;
            size_t leftIndex = (cell_index+m_gridWidth-1)%m_gridWidth;
            uint8_t right = m_grid[m_generation * m_gridWidth + rightIndex];
            uint8_t current = m_grid[m_generation * m_gridWidth + cell_index];
            uint8_t left = m_grid[m_generation * m_gridWidth + leftIndex];
            return right << 2 | current << 1 | left;
        }

        void Draw() const override {
            m_window.SetRenderColor(m_cellColor);
            SDL_Renderer* window_renderer = m_window.GetRenderer();
            for (std::size_t i = 0; i < m_grid.size(); i++) {
                if (m_grid[i]){
                    const int line = i/m_gridWidth; 
                    const int column = i%m_gridWidth;
                    SDL_Rect cell_rect = {m_gridMargin.x + column*m_cellSize, m_gridMargin.y + line*m_cellSize, m_cellSize, m_cellSize};
                    SDL_RenderFillRect(window_renderer, &cell_rect);
                }
            }
        }

        void Update() override {
            if (m_generation==m_gridHeight-1)
                return;
            for (std::size_t i = 0; i < m_gridWidth; i++) {
                const uint8_t n = GetNeighbors(i);
                uint8_t r = m_rule & (1 << n);
                m_grid[(m_generation+1) * m_gridWidth + i] = (r != 0) ? 1 : 0;
            }
            ++m_generation;
        }

        void Set(const PixelPosition& mouse) override {
            Grid2DPosition position = GetGridPositionFromMouse(mouse);
            if (IsGridPositionValid(position)){
                const int index = position.y*m_gridWidth+position.x;
                m_grid[index] ^= 1; // 2 states only
                m_generation = position.y; // Evolution will resume from the modified line
            }
        }

        void Empty() override{
            std::fill(m_grid.begin(), m_grid.begin()+m_gridWidth, 0);
            m_generation = 0;
        }

        void Fill() override{
            std::fill(m_grid.begin(), m_grid.begin()+m_gridWidth, 1);
            m_generation = 0;
        }

        void Randomize() override{
            for (std::size_t i = 0; i < m_gridWidth; i++)
                m_grid[i] = rand()%2;
            m_generation = 0;
        }
};

class Grid2D : public Grid
{
    protected:    
        std::vector<uint8_t> m_current_grid;
        std::vector<uint8_t> m_next_grid;
        std::vector<uint8_t> m_age_grid;

        public:
            Grid2D(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cell_color) : 
            Grid(window, gridWidth, gridHeight, cell_color)
            {
                Resize();
                std::srand(std::time({}));
                m_lastCell = {-1, -1}; // Unvalid cell
            }

            void Resize() override {
                m_current_grid.assign(m_gridWidth*m_gridHeight, 1);
                m_next_grid.assign(m_gridWidth*m_gridHeight, 1);
                m_age_grid.assign(m_gridWidth*m_gridHeight, 1);
            }

            std::vector<uint8_t> GetGrid() const {
                return m_current_grid;
            }
            
            void Empty() override{
                std::fill(m_current_grid.begin(), m_current_grid.end(), 0);
                std::fill(m_age_grid.begin(), m_age_grid.end(), 0);
            }

            void Fill() override{
                std::fill(m_current_grid.begin(), m_current_grid.end(), 1);
                std::fill(m_age_grid.begin(), m_age_grid.end(), 1);
            }

            void Randomize() override{
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    m_current_grid[i] = rand()%2;
                    m_age_grid[i] = m_current_grid[i];
                } 
            }

            unsigned int GetNrNeighbor(const size_t cell_index, const int range=1) const{
                unsigned int neighbor = 0;
                const int cell_line = cell_index/m_gridWidth; 
                const int cell_column = cell_index%m_gridWidth;
                
                for (int x = -range ; x <= range ; x++){
                    for (int y = -range ; y <= range ; y++){
                        if (x == 0 && y == 0)
                            continue;

                        const int neighbor_line = cell_line+y;
                        const int neighbor_column = cell_column+x;
                        if (neighbor_line < 0 || neighbor_line >= m_gridHeight || neighbor_column < 0 || neighbor_column >= m_gridWidth)
                            continue;
                        if (m_current_grid[neighbor_line * m_gridWidth + neighbor_column]) neighbor++;
                    }
                }
                return neighbor;
            }
            
            void Draw() const override {
                m_window.SetRenderColor(m_cellColor);
                SDL_Renderer* window_renderer = m_window.GetRenderer();
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    if (m_current_grid[i]){
                        const int line = i/m_gridWidth; 
                        const int column = i%m_gridWidth;
                        SDL_Rect cell_rect = {m_gridMargin.x + column*m_cellSize, m_gridMargin.y + line*m_cellSize, m_cellSize, m_cellSize};
                        SDL_RenderFillRect(window_renderer, &cell_rect);
                    }
                }
            }

            void Set(const PixelPosition& mouse) override {
                Grid2DPosition position = GetGridPositionFromMouse(mouse);
                if (IsGridPositionValid(position)){
                    const int index = position.y*m_gridWidth+position.x;
                    m_current_grid[index] ^= 1; // 2 states only
                    m_age_grid[index] = m_current_grid[index]; // 2 states only
                }
            }

            std::vector<uint8_t> GetAgeGrid() const{
                return m_age_grid;
            }
};

// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
class GameOfLife : public Grid2D 
{
    private:
        const int m_birth, m_underpopulation, m_overpopulation;

    public :
        GameOfLife(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cell_color, const int birth, const int underpopulation, const int overpopulation):
        Grid2D(window, gridWidth, gridHeight, cell_color), m_birth(birth), m_underpopulation(underpopulation), m_overpopulation(overpopulation)
        {
            Randomize();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                const int nr_neighbor = GetNrNeighbor(i);
                if (m_current_grid[i]){ // Living cell 
                    if (nr_neighbor < m_underpopulation || nr_neighbor > m_overpopulation){
                        m_next_grid[i] = 0;
                        //m_age_grid[i] = 0;
                    }else{
                        m_next_grid[i] = 1;
                        m_age_grid[i] += 1;
                    }
                }else{ // Dead cell
                    if (nr_neighbor == m_birth){
                        m_next_grid[i] = 1;
                        m_age_grid[i] += 1;
                    }
                    else m_next_grid[i] = 0; // Cell age is already 0 
                }
            }
            m_current_grid = m_next_grid;    
        }
};

// https://en.wikipedia.org/wiki/Langton%27s_ant
class LangtonAnt : public Grid2D
{
    private:
        std::vector<Grid2DPosition> m_ant_positions;
        std::vector<unsigned int> m_ant_directions;
        Grid2DPosition m_directions[4];
        
        void TurnRight(const unsigned int ant_index){
            m_ant_directions[ant_index] = (m_ant_directions[ant_index]+1)%4;
        }

        void TurnLeft(const unsigned int ant_index){
            m_ant_directions[ant_index] = (m_ant_directions[ant_index]+3)%4;
        }
        
    public:
        LangtonAnt(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cell_color, const Grid2DPosition initial_position, const unsigned int nr_ant=1):
        Grid2D(window, gridWidth, gridHeight, cell_color), m_ant_positions(nr_ant, initial_position)
        {
            m_directions[0] = {0, 1}; m_directions[1] = {1, 0}; m_directions[2] = {0, -1}; m_directions[3] = {-1, 0};
            for (unsigned int i = 0 ; i < nr_ant ; i++)
                m_ant_directions.push_back(i);
            Empty();
        }

        void Update() override {
            for (unsigned int i = 0 ; i < m_ant_positions.size() ; i++){
                Grid2DPosition& ant_position = m_ant_positions[i];
                const int index = ant_position.y * m_gridWidth + ant_position.x;
                if (m_current_grid[index]){
                    TurnRight(i);
                }else{
                    TurnLeft(i);
                }
                ant_position += m_directions[m_ant_directions[i]]; // Move forward one unit
                m_age_grid[i]++;
                m_current_grid[index] ^= 1; // Or m_next_grid
            }
        }
};