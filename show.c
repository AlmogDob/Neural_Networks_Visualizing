/* This program is inspired by The Tsoding Daily's video 'Visualizing Neural Networks in C (ML in C Ep.04)'.
You can find the video here:
https://youtu.be/-ii5SJCGjjU .*/

#define SETUP
#define UPDATE
#define RENDER
// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600
#include "display.c"
#include <stdio.h>

SDL_Rect rect;

void setup(void) 
{
}

void update(void)
{
    rect.h = 100;
    rect.w = 200;
    rect.x = current_window_width/2;
    rect.y = current_window_height/2;

}

void render(void)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}
