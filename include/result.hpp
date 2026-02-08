#pragma once

#include <cstring>

#include <SDL2/SDL.h>

#include "type.hpp"

class TextureResult
{
    private:
        SDL_Texture* m_texture;
        SDL_Renderer* m_window_renderer;
        SDL_Rect m_dst;
        uint32_t* m_current_texture_buffer;
        uint32_t* m_next_texture_buffer;
        uint32_t* m_frame_texture_buffer;
        const int m_width;
        const int m_height;

    public:
        TextureResult(SDL_Renderer* renderer, const std::pair<int, int> grid_margin, const int width, const int height, const int cell_size):
        m_window_renderer(renderer), m_width(width), m_height(height)
        {       
            // RGB888 should be enough for now
            m_texture = SDL_CreateTexture(m_window_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
            m_current_texture_buffer = new uint32_t[m_width*m_height];
            m_next_texture_buffer = new uint32_t[m_width*m_height];
            m_frame_texture_buffer = new uint32_t[m_width*m_height];
            m_dst = {grid_margin.first, grid_margin.second, width*cell_size, m_height*cell_size};
            for (unsigned int i = 0 ; i <m_width*m_height ; i++){
                WriteColorCurrentBuffer({0, 0, 0}, i);
                WriteColorNextBuffer({0, 0, 0}, i);
            }
        }

        ~TextureResult(){
            delete[] m_current_texture_buffer;
            delete[] m_next_texture_buffer;
            delete[] m_frame_texture_buffer;
            SDL_DestroyTexture(m_texture);
        }

        void WriteColorCurrentBuffer(const SDL_Color& color, const int index){
            m_current_texture_buffer[index] = (color.r << 16) | (color.g << 8) | color.b;
        }

        void WriteColorNextBuffer(const SDL_Color& color, const int index){
            m_next_texture_buffer[index] = (color.r << 16) | (color.g << 8) | color.b;
        }

        void Draw(const float t){
            NextFrame(t);
            SDL_RenderCopy(m_window_renderer, m_texture, NULL, &m_dst);
        }

        SDL_Color GetColorFromUint32(const uint32_t pixel) const{
            SDL_Color c = {0, 0, 0, 255};
            c.r = (pixel >> 16) & 0xFF;
            c.g = (pixel >> 8) & 0xFF;
            c.b = pixel & 0xFF;
            return c;
        }

        void NextFrame(const float t){
            for (unsigned int i = 0 ; i < m_width*m_height ; i++){
                SDL_Color src = GetColorFromUint32(m_current_texture_buffer[i]);
                SDL_Color dst = GetColorFromUint32(m_next_texture_buffer[i]);
                SDL_Color result_color = {0, 0, 0, 255};
                result_color.r = src.r + (dst.r - src.r)*t;
                result_color.g = src.g + (dst.g - src.g)*t;
                result_color.b = src.b + (dst.b - src.b)*t;
                m_frame_texture_buffer[i] = (result_color.r << 16) | (result_color.g << 8) | result_color.b;
            } 

            SDL_UpdateTexture(m_texture , NULL, m_frame_texture_buffer, m_width*sizeof(uint32_t));
        }

        void Update(const std::bitset<GRID_WIDTH*GRID_HEIGHT>& grid){

            std::swap(m_current_texture_buffer, m_next_texture_buffer);

            for (unsigned int x = 0 ; x < m_width ; x++){
                
                unsigned char nr_alive_cell_column = 0;
                for (std::size_t i = 0; i < m_height; i++) {
                    if (grid[i * m_width + x]) nr_alive_cell_column++;
                }

                for (unsigned int y = 0 ; y < m_height ; y++){
                    unsigned char nr_alive_cell_line =0;
                    for (int j = 0 ; j < m_width ; j++){
                        if (grid[y * m_width + j]) nr_alive_cell_line++;
                    }
                    const int index = (m_height - 1 - y) * m_width + (m_width - 1 - x);
                    // const unsigned char r = nr_alive_cell_line*2+nr_alive_cell_column*2;
                    // WriteColorNextBuffer({r, r, r}, index);
                    WriteColorNextBuffer({static_cast<Uint8>(nr_alive_cell_line*4), static_cast<Uint8>(nr_alive_cell_column*4), 0}, index);
                }
            }
        }
};