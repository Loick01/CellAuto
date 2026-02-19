#pragma once 

#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "type.hpp"
#include "window.hpp"

class Grid
{
    protected: 
        Window& m_window;
        SDL_Color m_cellColor;
        Grid2DPosition m_lastCell; // Last interacted cell with the mouse (should not nbe here)
        int m_cellSize;
        int m_gridWidth;
        int m_gridHeight;
        int m_density; // Used when randomize the grid

        Grid(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cell_color) :
        m_window(window), m_cellColor(cell_color), m_gridWidth(gridWidth), m_gridHeight(gridHeight), m_density(25)
        {
            ComputeCellSize();
        }
        
        Grid2DPosition GetGridPositionFromMouse(const PixelPosition& mouse, const PixelPosition& cameraPosition){
            const int column = (mouse.x + cameraPosition.x) / m_cellSize; 
            const int line = (mouse.y + cameraPosition.y) / m_cellSize;
            return Grid2DPosition{column, line};
        }

        bool IsGridPositionValid(const Grid2DPosition& position){
            if (position.x == m_lastCell.x && position.y == m_lastCell.y)
                return false;
            if (position.y < 0 || position.y >= m_gridHeight || position.x < 0 || position.x >= m_gridWidth)
                return false;
            m_lastCell = Grid2DPosition{position.x, position.y};
            return true;
        }

    public:
        virtual void Draw(const PixelPosition position) const = 0;
        virtual void Update() = 0;
        virtual void Set(const PixelPosition& mouse, const PixelPosition& cameraPosition) = 0;
        virtual void Empty() = 0;
        virtual void Fill() = 0;
        virtual void Randomize() = 0;
        virtual void Resize() = 0;
        virtual void SetGUI() = 0; // Parameters used in this function will be available in ImGui

        void ComputeCellSize(){ // Will be called when the grid size is modified with ImGui
            const int window_width = m_window.GetWidth();
            const int window_height = m_window.GetHeight();
            const int max_size_x = window_width/m_gridWidth; // Maximum cell size to fill window width
            const int max_size_y = window_height/m_gridHeight; // Maximum cell size to fill window height 
            if (max_size_x < max_size_y) 
                m_cellSize = max_size_x;
            else 
                m_cellSize = max_size_y;
        }
        
        PixelPosition GetSize() const { 
            return PixelPosition{m_gridWidth*m_cellSize, m_gridHeight*m_cellSize};
        }
        
        SDL_Color& GetCellColor() {
            return m_cellColor;
        }

        int& GetWidth() {
            return m_gridWidth;
        }

        int& GetHeight() {
            return m_gridHeight;
        }

        int& GetDensity() {
            return m_density;
        }
};

// One dimensional grid, drawed by stacking them in a 2D grid
// https://en.wikipedia.org/wiki/Elementary_cellular_automaton
class Grid1D : public Grid 
{
    protected: 
        std::vector<uint8_t> m_grid;
        uint8_t m_rule;
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

        void Draw(const PixelPosition cameraPosition) const override {
            m_window.SetRenderColor(m_cellColor);
            SDL_Renderer* window_renderer = m_window.GetRenderer();
            for (std::size_t i = 0; i < m_grid.size(); i++) {
                if (m_grid[i]){
                    const int line = i/m_gridWidth; 
                    const int column = i%m_gridWidth;
                    SDL_Rect cell_rect = {column*m_cellSize - cameraPosition.x , line*m_cellSize - cameraPosition.y, m_cellSize, m_cellSize};
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

        void Set(const PixelPosition& mouse, const PixelPosition& cameraPosition) override {
            Grid2DPosition position = GetGridPositionFromMouse(mouse, cameraPosition);
            if (IsGridPositionValid(position)){
                const int index = position.y*m_gridWidth+position.x;
                m_grid[index] ^= 1; // 2 states only
                m_generation = position.y; // Evolution will resume from the modified line
            }
        }

        void Empty() override {
            std::fill(m_grid.begin(), m_grid.begin()+m_gridWidth, 0);
            m_generation = 0;
        }

        void Fill() override {
            std::fill(m_grid.begin(), m_grid.begin()+m_gridWidth, 1);
            m_generation = 0;
        }

        void Randomize() override {
            for (std::size_t i = 0; i < m_gridWidth; i++){
                unsigned int r = rand()%100;
                m_grid[i] = r < m_density ? 1 : 0;
            }
            m_generation = 0;
        }

        void SetGUI() override {
            int rule_int = static_cast<int>(m_rule);
            ImGui::SliderInt("Rule", &rule_int, 0, 255);
            m_rule = rule_int;
        }
};

class Grid2D : public Grid
{
    protected:    
        std::vector<uint8_t> m_current_grid;
        std::vector<uint8_t> m_next_grid;

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
            }

            std::vector<uint8_t> GetGrid() const {
                return m_current_grid;
            }
            
            void Empty() override{
                std::fill(m_current_grid.begin(), m_current_grid.end(), 0);
            }

            void Fill() override{
                std::fill(m_current_grid.begin(), m_current_grid.end(), 1);
            }

            void Randomize() override{
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    unsigned int r = rand()%100;
                    m_current_grid[i] = r < m_density ? 1 : 0;
                } 
            }

            unsigned int GetNrAliveNeighbor(const size_t cell_index, const int range=1) const{
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
            
            void Draw(const PixelPosition cameraPosition) const override {
                m_window.SetRenderColor(m_cellColor);
                SDL_Renderer* window_renderer = m_window.GetRenderer();
                for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                    if (m_current_grid[i]){
                        const int line = i/m_gridWidth; 
                        const int column = i%m_gridWidth;
                        SDL_Rect cell_rect = {column*m_cellSize - cameraPosition.x, line*m_cellSize - cameraPosition.y, m_cellSize, m_cellSize};
                        SDL_RenderFillRect(window_renderer, &cell_rect);
                    }
                }
            }

            void Set(const PixelPosition& mouse, const PixelPosition& cameraPosition) override {
                Grid2DPosition position = GetGridPositionFromMouse(mouse, cameraPosition);
                if (IsGridPositionValid(position)){
                    const int index = position.y*m_gridWidth+position.x;
                    m_current_grid[index] ^= 1; // 2 states only
                }
            }
};

// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
class GameOfLife : public Grid2D 
{
    private:
        std::array<bool, 9> m_birth{false}, m_survive{false}; 

    public :
        GameOfLife(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cell_color,
            std::vector<int> birth, std::vector<int> survive):
        Grid2D(window, gridWidth, gridHeight, cell_color)
        {
            for (int i : birth)
                m_birth[i] = true;
            for (int i : survive)
                m_survive[i] = true;
            
            Randomize();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                const int nr_alive = GetNrAliveNeighbor(i);
                if (m_current_grid[i]){ // Living cell 
                    if (m_survive[nr_alive]){
                        m_next_grid[i] = 1;
                    }else{
                        m_next_grid[i] = 0;
                    }
                }else{ // Dead cell
                    if (m_birth[nr_alive]){
                        m_next_grid[i] = 1;
                    }
                    else m_next_grid[i] = 0;
                }
            }
            m_current_grid = m_next_grid;    
        }

        void SetGUI() override {
            ImGui::Text("Current rule : B");
            for (unsigned int i = 0 ; i < 9 ; i++){
                if (m_birth[i]){
                    ImGui::SameLine(0, 0);
                    ImGui::Text("%d", i);
                }
            }
            ImGui::SameLine(0, 0);
            ImGui::Text("/S");
            for (unsigned int i = 0 ; i < 9 ; i++){
                if (m_survive[i]){
                    ImGui::SameLine(0, 0);
                    ImGui::Text("%d", i);
                }
            }
            
            ImGui::Text("Birth rule");
            for (int i = 0 ; i < 9 ; i++){
                ImGui::SameLine();
                if (ImGui::Button((std::to_string(i)+"##Birth").c_str()))
                    m_birth[i] = !m_birth[i];
            }
            ImGui::Text("Survive rule");
            for (int i = 0 ; i < 9 ; i++){
                ImGui::SameLine();
                if (ImGui::Button((std::to_string(i)+"##Survive").c_str()))
                    m_survive[i] = !m_survive[i];
            }
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

        void UpdatePosition(Grid2DPosition& antPosition, const Grid2DPosition& direction){
            Grid2DPosition newPosition = antPosition + direction;
            newPosition.x = (newPosition.x + m_gridWidth)  % m_gridWidth;
            newPosition.y = (newPosition.y + m_gridHeight) % m_gridHeight;
            antPosition = newPosition;
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
                UpdatePosition(ant_position, m_directions[m_ant_directions[i]]); // Move forward one unit
                m_current_grid[index] ^= 1; // Or m_next_grid
            }
        }
        void Resize() override {
            Grid2D::Resize();
            for (unsigned int i = 0 ; i < m_ant_positions.size() ; i++)
                m_ant_positions[i] = Grid2DPosition{m_gridWidth/2, m_gridHeight/2}; // Center the ant position when resizing the grid
        }

        void SetGUI() override {
            // Nothing yet
        }
};