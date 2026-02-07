#pragma once

#include <SDL2/SDL.h>

#include "type.hpp"

// https://stackoverflow.com/questions/62379457/how-to-draw-a-grid-of-pixels-fast-with-sdl2
class TextureResult
{
    private:
        SDL_Texture* m_texture;
        SDL_Renderer* m_window_renderer;
        SDL_Rect m_dst;
        uint32_t* m_texture_buffer;
        const int m_width;
        const int m_height;

    public:
        TextureResult(SDL_Renderer* renderer, const std::pair<int, int> grid_margin, const int width, const int height, const int cell_size):
        m_window_renderer(renderer), m_width(width), m_height(height)
        {       
            // RGB888 should be enough for now
            m_texture = SDL_CreateTexture(m_window_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
            m_texture_buffer = new uint32_t[m_width*m_height];
            m_dst = {grid_margin.first, grid_margin.second, width*cell_size, m_height*cell_size};
            for (unsigned int i = 0 ; i <m_width*m_height ; i++){
                m_texture_buffer[i] = (0 << 16) | (0 << 8) | 0;
            }
        }

        void Draw() const {
            SDL_RenderCopy(m_window_renderer, m_texture, NULL, &m_dst);
        }

        void Update(const std::bitset<GRID_WIDTH*GRID_HEIGHT>& grid){
            for (unsigned int x = 0 ; x < m_width ; x++){
                
                unsigned int nr_alive_cell_column = 0;
                for (std::size_t i = 0; i < m_height; i++) {
                    if (grid[i * m_width + x]) nr_alive_cell_column++;
                }

                for (unsigned int y = 0 ; y < m_height ; y++){
                    unsigned int nr_alive_cell_line =0;
                    for (int j = 0 ; j < m_width ; j++){
                        if (grid[y * m_width + j]) nr_alive_cell_line++;
                    }
                    const int index = (m_height - 1 - y) * m_width + (m_width - 1 - x);
                    // const int r = nr_alive_cell_line*2+nr_alive_cell_column*2;
                    // m_texture_buffer[index] =  (r << 16) | (0 << 8) | 0;
                    m_texture_buffer[index] =  (nr_alive_cell_line*4 << 16) | (nr_alive_cell_column*4 << 8) | 0;
                }
            }
            SDL_UpdateTexture(m_texture , NULL, m_texture_buffer, m_width*sizeof(uint32_t));
        }
};