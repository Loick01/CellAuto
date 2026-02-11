#pragma once

#include <cstring>

#include <SDL2/SDL.h>

#include "type.hpp"

class TextureResult
{
    private:
        std::vector<float> m_density;
        SDL_Texture* m_texture;
        SDL_Renderer* m_window_renderer;
        SDL_Rect m_dst;
        uint32_t* m_current_texture_buffer;
        uint32_t* m_next_texture_buffer;
        uint32_t* m_frame_texture_buffer;
        const int m_width;
        const int m_height;

    public:
        TextureResult(const Window& window, const PixelPosition grid_margin, const int width, const int height, const int cell_size):
        m_window_renderer(window.GetRenderer()), m_width(width), m_height(height)
        {       
            m_texture = SDL_CreateTexture(m_window_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
            //SDL_SetTextureScaleMode(m_texture, SDL_ScaleModeLinear);
            m_current_texture_buffer = new uint32_t[m_width*m_height];
            m_next_texture_buffer = new uint32_t[m_width*m_height];
            m_frame_texture_buffer = new uint32_t[m_width*m_height];
            m_dst = {grid_margin.x+window.GetWidth()/2, grid_margin.y, m_width*cell_size, m_height*cell_size};
            for (unsigned int i = 0 ; i <m_width*m_height ; i++){
                WriteColorCurrentBuffer({0, 0, 0}, i);
                WriteColorNextBuffer({0, 0, 0}, i);
            }
            m_density = std::vector<float>(m_width*m_height, 0.f);
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
            NextFrameTimeInterpolation(t);
            // NextFrameRaw();
            SDL_RenderCopy(m_window_renderer, m_texture, NULL, &m_dst);
        }

        SDL_Color GetColorFromUint32(const uint32_t pixel) const{
            SDL_Color c = {0, 0, 0, 255};
            c.r = (pixel >> 16) & 0xFF;
            c.g = (pixel >> 8) & 0xFF;
            c.b = pixel & 0xFF;
            return c;
        }

        void NextFrameRaw(){
            SDL_UpdateTexture(m_texture , NULL, m_next_texture_buffer, m_width*sizeof(uint32_t)); 
        }
        
        void NextFrameTimeInterpolation(const float t){
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

        /*
        void Update(const Grid2D& grid){

            const std::bitset<GRID_WIDTH*GRID_HEIGHT>& bitset = grid.GetGrid();
            std::swap(m_current_texture_buffer, m_next_texture_buffer);

            for (unsigned int x = 0 ; x < m_width ; x++){
                
                unsigned char nr_alive_cell_column = 0;
                for (std::size_t i = 0; i < m_height; i++) {
                    if (bitset[i * m_width + x]) nr_alive_cell_column++;
                }

                for (unsigned int y = 0 ; y < m_height ; y++){
                    unsigned char nr_alive_cell_line =0;
                    for (int j = 0 ; j < m_width ; j++){
                        if (bitset[y * m_width + j]) nr_alive_cell_line++;
                    }
                    const int index = (m_height - 1 - y) * m_width + (m_width - 1 - x);
                    // const unsigned char r = nr_alive_cell_line*2+nr_alive_cell_column*2;
                    // WriteColorNextBuffer({r, r, r}, index);
                    WriteColorNextBuffer({static_cast<Uint8>(nr_alive_cell_column*4), static_cast<Uint8>(nr_alive_cell_line*4), 0}, index);
                }
            }
        } */
        
        void Update(const Grid2D& grid){

            std::swap(m_current_texture_buffer, m_next_texture_buffer);

            for (unsigned int x = 0 ; x < m_width ; x++){
                for (unsigned int y = 0 ; y < m_height ; y++){
                    const int indexBuffer = (m_height - 1 - y) * m_width + (m_width - 1 - x);
                    const int indexSrc = y * m_width + x;
                    const unsigned char alive_neighbor = grid.GetNrNeighbor(indexSrc, 1); // [0, 8]
                    
                    m_density[indexBuffer] = 0.9f * m_density[indexBuffer] + 0.1f *  alive_neighbor / 8.f;
                    SDL_Color c0 = {0, 0, 0}; // Low density
                    SDL_Color c1 = {230, 120, 50}; // High density
                    float t = m_density[indexBuffer];
                    float r = c0.r + t * (c1.r - c0.r);
                    float g = c0.g + t * (c1.g - c0.g);
                    float b = c0.b + t * (c1.b - c0.b);
                    SDL_Color c = {(Uint8)r,(Uint8)g,(Uint8)b};
                    WriteColorNextBuffer(c, indexBuffer);
                }
            }

            // for (unsigned int x = 0 ; x < m_width ; x++){
            //     for (unsigned int y = 0 ; y < m_height ; y++){
            //         const int indexBuffer = (m_height - 1 - y) * m_width + (m_width - 1 - x);
            //         const int indexSrc = y * m_width + x;
            //         const unsigned char alive_neighbor = grid.GetNrNeighbor(indexSrc, 2);
            //         float t = alive_neighbor/24.;
            //         SDL_Color r = {static_cast<unsigned char>(t*255), 0, static_cast<unsigned char>((1.-t)*255)};
            //         WriteColorNextBuffer(r, indexBuffer);
            //     }
            // }
        }
        
        /*
        void Update(const Grid2D& grid){
            
            std::swap(m_current_texture_buffer, m_next_texture_buffer);
            std::vector<unsigned char> age_grid = grid.GetAgeGrid();

            for (unsigned int i = 0 ; i < age_grid.size() ; i++){
                const unsigned char age = age_grid[i];
                float s = sin(age/4.)*0.5f+0.5f; // [0,1]
                float c = cos(age/4.)*0.5f+0.5f; // [0,1]
                WriteColorNextBuffer(SDL_Color{(Uint8)(s*255),(Uint8)(c*255),(Uint8)(s*c*255)},i);
            }

            // for (unsigned int i = 0 ; i < age_grid.size() ; i++){
            //     const unsigned char age = age_grid[i];

            //     SDL_Color c1 = {20, 10, 5, 255};
            //     SDL_Color c2 = {180, 110, 40, 255};
            //     SDL_Color c3 = {240, 230, 200, 255};

            //     float t = sin(age/12.0f)*0.5f+0.5f;

            //     float r = lerp(lerp(c1.r, c2.r, t), c3.r, t*t);
            //     float g = lerp(lerp(c1.g, c2.g, t), c3.g, t*t);
            //     float b = lerp(lerp(c1.b, c2.b, t), c3.b, t*t);

            //     WriteColorNextBuffer(SDL_Color{(Uint8)r, (Uint8)g, (Uint8)b},i);
            // }
        }*/
};