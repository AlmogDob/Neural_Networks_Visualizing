/* This program is inspired by The Tsoding Daily's video 'Visualizing Neural Networks in C (ML in C Ep.04)'.
You can find the video here:
https://youtu.be/-ii5SJCGjjU .*/

#define SETUP
#define UPDATE
#define RENDER
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#include "display.c"
#include <stdio.h>
#include <time.h>

#define NN_IMPLEMENTATION
#include "nn.h"


SDL_Rect rect;

void fill_circle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color);
SDL_Color Hex2SDL_color(uint32_t x);  
float liniar_map(float s, float min_in, float max_in, float min_out, float max_out);

void setup(void) 
{
    // to_clear_the_screnn = 0;
    // to_show_fps = 0;
    I_am_rendering = 1;

    SDL_SetRenderDrawColor(renderer, Hex2RGBA(0xFF181818));
    SDL_RenderClear(renderer);

    srand(time(0));
    size_t arch[] = {4, 4, 2, 1};
    NN nn = nn_alloc(arch, ARRAY_LEN(arch));
    nn_rand(nn, -1, 1);
    
    SDL_Color low_color = Hex2SDL_color(0xFF0000FF);
    SDL_Color high_color = Hex2SDL_color(0xFF00FF00);
    int neuron_radius = 25;
    int layer_border_vpad = 50;
    int nn_height = current_window_height - 2*layer_border_vpad;
    int layer_border_hpad = 50;
    int nn_width = current_window_width - 2*layer_border_hpad;
    int layer_hpad = nn_width/(nn.count + 1);
    int nn_x = current_window_width/2 - nn_width/2;
    int nn_y = current_window_height/2 - nn_height/2;
    for (size_t l = 0; l < nn.count + 1; l++) {
        int layer_vpad1 = nn_height/arch[l];
        for (size_t i = 0; i < arch[l]; i++) {
            int cx1 = nn_x + l*layer_hpad + layer_hpad/2;
            int cy1 = nn_y + i*layer_vpad1 + layer_vpad1/2;
            if (l+1 < nn.count+1) {
                int layer_vpad2 = nn_height/arch[l+1];
                for (size_t j = 0; j < arch[l+1]; j++) {
                    int cx2 = nn_x + (l+1)*layer_hpad + layer_hpad/2;
                    int cy2 = nn_y + j*layer_vpad2 + layer_vpad2/2;
                    float s = sigmoidf(MAT_AT(nn.ws[l], i, j));
                    SDL_Color new_color = {.a = low_color.a*(1-s) + high_color.a*s,
                                           .b = low_color.b*(1-s) + high_color.b*s,
                                           .g = low_color.g*(1-s) + high_color.g*s,
                                           .r = low_color.r*(1-s) + high_color.r*s};           
                    SDL_SetRenderDrawColor(renderer, new_color.r, new_color.g, new_color.b, new_color.a);
                    SDL_RenderDrawLine(renderer, cx1, cy1, cx2, cy2);
                }
            }
            if (l > 0) {
                float s = sigmoidf(MAT_AT(nn.bs[l-1], 0, i));
                SDL_Color new_color = {.a = low_color.a*(1-s) + high_color.a*s,
                                       .b = low_color.b*(1-s) + high_color.b*s,
                                       .g = low_color.g*(1-s) + high_color.g*s,
                                       .r = low_color.r*(1-s) + high_color.r*s};
                fill_circle(renderer, cx1, cy1, neuron_radius, new_color);
            } else {
                fill_circle(renderer, cx1, cy1, neuron_radius, Hex2SDL_color(0xFF505050));
            }
        }
    }    
    SDL_RenderPresent(renderer);

}

void update(void)
{
}

void render(void)
{
}

void fill_circle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

SDL_Color Hex2SDL_color(uint32_t x) 
{
    SDL_Color color = {
    color.r = (x>>(8*0)&0xFF),
    color.g = (x>>(8*1)&0xFF),
    color.b = (x>>(8*2)&0xFF),
    color.a = (x>>(8*3)&0xFF)
    };
    return color;
}

float liniar_map(float s, float min_in, float max_in, float min_out, float max_out)
{
    return (min_out + ((s-min_in)*(max_out-min_out))/(max_in-min_in));
}