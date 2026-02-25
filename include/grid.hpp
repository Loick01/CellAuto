#pragma once 

#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "type.hpp"
#include "window.hpp"

enum class Neighborhood 
{
    VonNeumann, Moore
};

class Grid
{
    protected: 
        Window& m_window;
        Grid2DPosition m_lastCell; // Last interacted cell with the mouse (should not nbe here)
        Neighborhood m_nbhType;
        int m_cellSize;
        int m_gridWidth;
        int m_gridHeight;
        int m_density; // Used when randomize the grid

        Grid(Window& window, const int gridWidth, const int gridHeight) :
        m_window(window), m_gridWidth(gridWidth), m_gridHeight(gridHeight), m_density(25)
        {
            ComputeCellSize();
        }
        
        Grid2DPosition GetGridPositionFromMouse(const PixelPosition& mouse, const PixelPosition& cameraPosition){
            const int column = (mouse.x + cameraPosition.x) / m_cellSize; 
            const int line = (mouse.y + cameraPosition.y) / m_cellSize;
            return Grid2DPosition{column, line};
        }

        size_t GetIndexFromPosition(const Grid2DPosition position) const {
            return position.y * m_gridWidth + position.x;
        }
        
        bool IsLastCell(const Grid2DPosition& position){
            return position.x == m_lastCell.x && position.y == m_lastCell.y;
        }

        bool IsPositionValid(const Grid2DPosition& position) const {
            return position.y >= 0 && position.y < m_gridHeight && position.x >= 0 && position.x < m_gridWidth;
        }

    public:
        virtual void Draw(const PixelPosition position) const = 0;
        virtual void Update() = 0;
        virtual void Set(const PixelPosition& mouse, const PixelPosition& cameraPosition) = 0;
        virtual void Empty() = 0;
        virtual void Fill() = 0;
        virtual void RandomizeGrid() = 0;
        virtual void Resize() = 0;
        virtual void SetAutomataGUI() {} // Parameters used in this function will be available in Tab: Automata
        virtual void SetColorGUI() {}; // Parameters used in this function will be available in Tab: Colors

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

        int& GetWidth() {
            return m_gridWidth;
        }

        int& GetHeight() {
            return m_gridHeight;
        }

        int& GetDensity() {
            return m_density;
        }
        
        int GetNeighborhood() const {
            return (int)m_nbhType;
        }
        
        void SetNeighborhood(const Neighborhood nbhType) {
            m_nbhType = nbhType;
        }
};

// One dimensional grid, drawed by stacking them in a 2D grid
// https://en.wikipedia.org/wiki/Elementary_cellular_automaton
class Grid1D : public Grid 
{
    private: 
        std::vector<uint8_t> m_grid;
        SDL_Color m_cellColor; // 2 states only, one color is enough
        uint8_t m_rule;
        size_t m_generation;

    public:
        Grid1D(Window& window, const int gridWidth, const int gridHeight, const SDL_Color& cellColor, const uint8_t rule):
        Grid(window, gridWidth, gridHeight), m_cellColor(cellColor), m_generation(0), m_rule(rule) {
            Resize();
            Initial();
        }

        void Resize() override {
            m_grid.assign(m_gridWidth*m_gridHeight, 1);
        }

        void Initial() {
            m_grid[m_generation * m_gridWidth + m_gridWidth / 2] = 1;
        }

        uint8_t GetNeighbors(const size_t cellIndex) const {
            size_t rightIndex = (cellIndex+1)%m_gridWidth;
            size_t leftIndex = (cellIndex+m_gridWidth-1)%m_gridWidth;
            uint8_t right = m_grid[m_generation * m_gridWidth + rightIndex];
            uint8_t current = m_grid[m_generation * m_gridWidth + cellIndex];
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
                    SDL_Rect cellRect = {column*m_cellSize - cameraPosition.x , line*m_cellSize - cameraPosition.y, m_cellSize, m_cellSize};
                    SDL_RenderFillRect(window_renderer, &cellRect);
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
            if (!IsLastCell(position) && IsPositionValid(position)){
                m_lastCell = position;
                m_grid[GetIndexFromPosition(position)] ^= 1; // 2 states only
                m_generation = position.y; // Evolution will resume from the last modified line
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

        void RandomizeGrid() override {
            for (std::size_t i = 0; i < m_gridWidth; i++){
                unsigned int r = rand()%100;
                m_grid[i] = r < m_density ? 1 : 0;
            }
            m_generation = 0;
        }

        void SetAutomataGUI() override {
            int rule_int = static_cast<int>(m_rule);
            ImGui::SliderInt("Rule", &rule_int, 0, 255);
            m_rule = rule_int;
        }
};

template<typename T>
class Grid2D : public Grid
{
    protected:    
        std::vector<SDL_Color> m_cellColors; // n colors for n+1 states. 0 is the state a cell has when it doesn't have to be rendered
        std::vector<T> m_current_grid;
        std::vector<T> m_next_grid;
        int m_nrState;

        void ImGuiColorPicker(const char* label, const size_t colorIndex) {
            SDL_Color& c = m_cellColors[colorIndex];
            float values[3] = {c.r/255.f, c.g/255.f, c.b/255.f};
            ImGui::ColorEdit3(label, values);
            c = {static_cast<Uint8>(values[0]*255), static_cast<Uint8>(values[1]*255), static_cast<Uint8>(values[2]*255)};
        }

    public:
        Grid2D(Window& window, Neighborhood nbhType, const int gridWidth, const int gridHeight, const int nrState) : 
        Grid(window, gridWidth, gridHeight), m_nrState(nrState)
        {
            m_nbhType = nbhType;
            m_cellColors.resize(m_nrState-1);
            RandomizeColors();
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
        
        void Empty() override {
            std::fill(m_current_grid.begin(), m_current_grid.end(), 0);
        }

        void Fill() override {
            std::fill(m_current_grid.begin(), m_current_grid.end(), 1);
        }

        void RandomizeGrid() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                unsigned int r = rand()%100;
                m_current_grid[i] = r < m_density ? rand()%m_nrState : 0; // 0 can also be generated (for example, cyclic CA produce better result with uniform distribution  ) 
            } 
        }

        void RandomizeColors() {
            for (unsigned int i = 0 ; i < m_cellColors.size() ; i++){
                SDL_Color& c = m_cellColors[i];
                c = {static_cast<Uint8>(rand()%255), static_cast<Uint8>(rand()%255), static_cast<Uint8>(rand()%255)};
            }
        }

        void InterpolateColors(const SDL_Color& srcColor, const SDL_Color& dstColor) {
            for (unsigned int i = 1 ; i < m_cellColors.size()-1 ; i++){
                const float t = (float)i/m_cellColors.size(); 
                m_cellColors[i].r = srcColor.r + (dstColor.r-srcColor.r)*t;
                m_cellColors[i].g = srcColor.g + (dstColor.g-srcColor.g)*t;
                m_cellColors[i].b = srcColor.b + (dstColor.b-srcColor.b)*t;
            }
        }

        uint8_t GetNeighborsInState(const size_t cellIndex, const T state) const {
            uint8_t nrNeighbor = 0;
            const int cellLine = cellIndex/m_gridWidth; 
            const int cellColumn = cellIndex%m_gridWidth;
            for (int x = -1 ; x <= 1 ; x++){
                for (int y = -1 ; y <= 1 ; y++){
                    
                    switch(m_nbhType) {
                        case Neighborhood::VonNeumann :
                            if (abs(x) == abs(y)) continue;
                        case Neighborhood::Moore :
                            if (x == 0 && y == 0) continue;
                    }

                    const Grid2DPosition neighborPosition = {cellColumn+x, cellLine+y};
                    if (!IsPositionValid(neighborPosition))
                        continue;
                    if (m_current_grid[GetIndexFromPosition(neighborPosition)] == state) 
                        nrNeighbor++;
                }
            }
            return nrNeighbor;
        }

        void Draw(const PixelPosition cameraPosition) const override {
            SDL_Renderer* window_renderer = m_window.GetRenderer();
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                const T cellState = m_current_grid[i];
                if (cellState != 0){
                    m_window.SetRenderColor(m_cellColors[cellState-1]);
                    const int line = i/m_gridWidth; 
                    const int column = i%m_gridWidth;
                    SDL_Rect cell_rect = {column*m_cellSize - cameraPosition.x, line*m_cellSize - cameraPosition.y, m_cellSize, m_cellSize};
                    SDL_RenderFillRect(window_renderer, &cell_rect);
                }
            }
        }

        void Set(const PixelPosition& mouse, const PixelPosition& cameraPosition) override {
            Grid2DPosition position = GetGridPositionFromMouse(mouse, cameraPosition);
            if (!IsLastCell(position) && IsPositionValid(position)){
                m_lastCell = position;
                const T current = m_current_grid[GetIndexFromPosition(position)];
                m_current_grid[GetIndexFromPosition(position)] = current==0; // 0 --> 1 ; 1 --> 0 ; 2 --> 0
            }
        }

        void SetColorGUI() override {
            ImGuiColorPicker("Source color for interpolation", 0);
            ImGuiColorPicker("Destination color for interpolation", m_cellColors.size()-1);

            if (ImGui::Button("Interpolate cell colors")) // Background color (used when cell state = 0) should be the source color ?
                InterpolateColors(m_cellColors.front(), m_cellColors.back());

            if (ImGui::Button("Randomize cell colors"))
                RandomizeColors();

            for (unsigned int i = 0 ; i < m_cellColors.size() ; i++)
                ImGuiColorPicker(("State " + std::to_string(i+1) + " color").c_str(), i);
        }
};

// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
class GameOfLife : public Grid2D<uint8_t> 
{
    private:
        std::array<bool, 9> m_birth{false}, m_survive{false}; 

    public :
        GameOfLife(Window& window, const int gridWidth, const int gridHeight,
            std::vector<int> birth, std::vector<int> survive):
        Grid2D(window, Neighborhood::Moore, gridWidth, gridHeight, 2)
        {
            for (int i : birth)
                m_birth[i] = true;
            for (int i : survive)
                m_survive[i] = true;
            
            RandomizeGrid();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                const int nr_alive = GetNeighborsInState(i, 1);
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

        void SetAutomataGUI() override {
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
class LangtonAnt : public Grid2D<uint8_t> 
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
        LangtonAnt(Window& window, const int gridWidth, const int gridHeight, const Grid2DPosition initial_position, const unsigned int nr_ant=1):
        Grid2D(window, Neighborhood::VonNeumann, gridWidth, gridHeight, 2), m_ant_positions(nr_ant, initial_position)
        {
            m_directions[0] = {0, 1}; m_directions[1] = {1, 0}; m_directions[2] = {0, -1}; m_directions[3] = {-1, 0};
            for (unsigned int i = 0 ; i < nr_ant ; i++)
                m_ant_directions.push_back(i);
            Empty();
        }

        void Update() override {
            for (unsigned int i = 0 ; i < m_ant_positions.size() ; i++){
                Grid2DPosition& antPosition = m_ant_positions[i];
                const int index = GetIndexFromPosition(antPosition);
                if (m_current_grid[index]){
                    TurnRight(i);
                }else{
                    TurnLeft(i);
                }
                UpdatePosition(antPosition, m_directions[m_ant_directions[i]]); // Move forward one unit
                m_current_grid[index] ^= 1; // Or m_next_grid
            }
        }

        void Resize() override {
            Grid2D::Resize();
            for (unsigned int i = 0 ; i < m_ant_positions.size() ; i++)
                m_ant_positions[i] = Grid2DPosition{m_gridWidth/2, m_gridHeight/2}; // Center the ant position when resizing the grid
        }
};

// https://en.wikipedia.org/wiki/Greenberg%E2%80%93Hastings_cellular_automaton
class GreenbergHastings : public Grid2D<uint8_t> 
{       
    public:
        GreenbergHastings(Window& window, const int gridWidth, const int gridHeight):
        Grid2D(window, Neighborhood::VonNeumann, gridWidth, gridHeight, 3)
        {
            Empty();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                if (m_current_grid[i] == 1)
                    m_next_grid[i] = 2;
                else if (m_current_grid[i] == 2)
                    m_next_grid[i] = 0;
                else {
                    if (GetNeighborsInState(i, 1) >= 1) 
                        m_next_grid[i] = 1;
                    else m_next_grid[i] = 0;
                }
            }
            m_current_grid = m_next_grid;    
        }
};

// https://en.wikipedia.org/wiki/Forest-fire_model
class ForestFire : public Grid2D<uint8_t> 
{       
    private:
        float m_p, m_f; // Probability (%) for an empty cell to become a tree (m_p) and for a tree to ignite (m_f)
        
    public:
        ForestFire(Window& window, const int gridWidth, const int gridHeight, const float p, const float f):
        Grid2D(window, Neighborhood::VonNeumann, gridWidth, gridHeight, 3), m_p(p), m_f(f)
        {
            Empty();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                if (m_current_grid[i] == 0 && rand() < m_p * (RAND_MAX/100.0))
                    m_next_grid[i] = 1;
                else if (m_current_grid[i] == 1)
                    if (GetNeighborsInState(i, 2) >= 1 || rand() < m_f * (RAND_MAX/100.0)) 
                        m_next_grid[i] = 2;
                    else m_next_grid[i] = 1;
                else {
                    m_next_grid[i] = 0;
                }
            }
            m_current_grid = m_next_grid;    
        }

        void SetAutomataGUI() override {
            ImGui::SliderFloat("P", &m_p, 0., 1.);
            ImGui::SliderFloat("F", &m_f, 0., 1.);
        }
};

// https://en.wikipedia.org/wiki/Cyclic_cellular_automaton
class Cyclic : public Grid2D<uint8_t> 
{      
    private:
        int m_threshold;

    public:
        Cyclic(Window& window, const int gridWidth, const int gridHeight, const int nrState, const int threshold):
        Grid2D(window, Neighborhood::VonNeumann, gridWidth, gridHeight, nrState), m_threshold(threshold)
        {
            Empty();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                const uint8_t currentState = m_current_grid[i];
                const uint8_t nextState = (currentState+1)%m_nrState;
                if (GetNeighborsInState(i, nextState) >= m_threshold)
                    m_next_grid[i] = nextState;
                else
                    m_next_grid[i] = currentState;
            }
            m_current_grid = m_next_grid;   
        }

        void SetAutomataGUI() override {
            if (ImGui::SliderInt("Number of states", &m_nrState, 1, 25)){ // If the max state becomes too big, the grid may no longer evolve
                m_cellColors.resize(m_nrState-1);
                RandomizeColors();
                Empty();
            }
            ImGui::SliderInt("Threshold", &m_threshold, 0, 8);
        }
};

// https://webbox.lafayette.edu/~reiterc/mvq/hodgepodge/withj_hodgepodge.pdf
// https://softologyblog.wordpress.com/2017/02/04/the-belousov-zhabotinsky-reaction-and-the-hodgepodge-machine/
class Hodgepodge : public Grid2D<uint8_t> 
{      
    private:
        int m_k1, m_k2, m_g;

        unsigned int GetSumStateNeighborhood(const size_t cellIndex){ // Should not be here
            unsigned int sum = 0;

            const int cellLine = cellIndex/m_gridWidth; 
            const int cellColumn = cellIndex%m_gridWidth;
            for (int x = -1 ; x <= 1 ; x++){
                for (int y = -1 ; y <= 1 ; y++){
                    const int neighbor_line = cellLine+y;
                    const int neighbor_column = cellColumn+x;
                    if (neighbor_line < 0 || neighbor_line >= m_gridHeight || neighbor_column < 0 || neighbor_column >= m_gridWidth)
                        continue;
                    sum += m_current_grid[neighbor_line * m_gridWidth + neighbor_column];
                }
            }
            return sum;
        }

    public:
        Hodgepodge(Window& window, const int gridWidth, const int gridHeight, const int nrState, const int k1, const int k2, const int g):
        Grid2D(window, Neighborhood::Moore, gridWidth, gridHeight, nrState), m_k1(k1), m_k2(k2), m_g(g)
        {
            Empty();
        }

        void Update() override {
            for (std::size_t i = 0; i < m_current_grid.size(); i++) {
                const uint8_t currentState = m_current_grid[i];
                if (currentState == m_nrState-1)
                    m_next_grid[i] = 0;
                else if (currentState == 0){
                    const uint8_t nrHealthy = GetNeighborsInState(i, 0); 
                    const uint8_t nrIll = GetNeighborsInState(i, m_nrState-1); 
                    const uint8_t nrInfected = 8 - nrHealthy - nrIll;
                    m_next_grid[i] = std::floor(nrInfected/(float)m_k1) + std::floor(nrIll/(float)m_k2);
                } else {
                    const uint8_t nrHealthy = GetNeighborsInState(i, 0); 
                    const uint8_t nrIll = GetNeighborsInState(i, m_nrState-1); 
                    const uint8_t nrInfected = 8 - nrHealthy - nrIll;
                    const unsigned int s = GetSumStateNeighborhood(i);
                    m_next_grid[i] = std::floor(s/(nrInfected+nrIll+1))+m_g;
                }
            }
            m_current_grid = m_next_grid;   
        }

        void SetAutomataGUI() override {
            if (ImGui::SliderInt("Number of states", &m_nrState, 1, 256)){
                m_cellColors.resize(m_nrState-1);
                RandomizeColors();
                Empty();
            }
            ImGui::SliderInt("K1", &m_k1, 1, 9);
            ImGui::SliderInt("K2", &m_k2, 1, 9);
            ImGui::SliderInt("G", &m_g, 0, 100);
        }
};

// https://en.wikipedia.org/wiki/Abelian_sandpile_model
class AbelianSandpile : public Grid2D<unsigned int> // m_next_grid will not be used
{      
    private:
        Grid2DPosition m_addingPosition;
        int m_threshold; // A cell is toppled when the amount of grains is >= m_threshold
        bool m_isRandomAdded; // false -> grains are added to the center of the grid, true -> grains are randomly added

    public:
        AbelianSandpile(Window& window, const int gridWidth, const int gridHeight, const int threshold):
        Grid2D(window, Neighborhood::VonNeumann, gridWidth, gridHeight, threshold+1), m_threshold(threshold), m_isRandomAdded(false)
        {
            m_addingPosition = {m_gridWidth/2, m_gridHeight/2};
            Empty();
        }

        void Resize() override {
            Grid2D::Resize();
            m_addingPosition = {m_gridWidth/2, m_gridHeight/2};
        }

        void Update() override {
            if (m_isRandomAdded)
                m_addingPosition = {rand()%m_gridWidth, rand()%m_gridHeight}; 

            const unsigned int addingIndex = GetIndexFromPosition(m_addingPosition);
            m_current_grid[addingIndex]++;

            std::queue<Grid2DPosition> topplingCellPosition;
            if (m_current_grid[addingIndex] >= m_threshold)
                topplingCellPosition.push(m_addingPosition);

            int cnt = 0;
            while (!topplingCellPosition.empty()){
                const Grid2DPosition currentPosition = topplingCellPosition.front();
                topplingCellPosition.pop();

                if (m_current_grid[GetIndexFromPosition(currentPosition)] < m_threshold)
                    continue;

                m_current_grid[GetIndexFromPosition(currentPosition)] -= m_threshold;
                
                // This for-loop should not be here (same as Grid2D::GetNeighborsInState)
                for (int x = -1 ; x <= 1 ; x++){ 
                    for (int y = -1 ; y <= 1 ; y++){
                        if (abs(x) == abs(y)) continue; // VonNeumann neighborhood

                        const Grid2DPosition neighborPosition = currentPosition + Grid2DPosition{x,y};
                        if (!IsPositionValid(neighborPosition))
                            continue;

                        const unsigned int neighborIndex = GetIndexFromPosition(neighborPosition);
                        m_current_grid[neighborIndex]++;
                        if (m_current_grid[neighborIndex] >= m_threshold)
                            topplingCellPosition.push(neighborPosition);
                    }
                }
            }
        }

        void SetAutomataGUI() override {
            // A toppling cell must lose exactly as many grains as it distributes to its neighbors. For now I remove this slider
            // if (ImGui::SliderInt("Threshold", &m_threshold, 4, 8)){ // The threshold is maxState
            //     m_cellColors.resize(m_threshold);
            //     RandomizeColors();
            //     Empty();
            // }

            if (ImGui::RadioButton("Add in the middle of the grid", !m_isRandomAdded)){
                m_isRandomAdded = false;
                m_addingPosition = {m_gridWidth/2, m_gridHeight/2};
            }
            if (ImGui::RadioButton("Add randomly", m_isRandomAdded))
                m_isRandomAdded = true;
        }
};