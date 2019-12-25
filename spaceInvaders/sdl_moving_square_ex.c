#include <stdio.h>
#include <math.h>
#include "SDL.h"


typedef struct point{
    int x, y;
}point;
/*
SDL_Event event;
int quit=0;
while(quit!=1) {
    while(SDL_PollEvent(&event){
        if(event.type==SDL_KEYDOWN){
            switch(event.key.keysym.sym){
                case SDLK_a: m_x -= 30; break;
                case SDLK_d: m_x += 30; break;
                case SDLK_w: m_y -= 30; break;
                case SDLK_s: m_y += 30; break;
            }
        }
        else if(event.type==SDL_QUIT){quit=1;}
    }
}
*/
int main (int argc, char** argv){
    point p; p.x=0; p.y=0;
    //graphics s
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        "SDL moving square", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        224,
        256,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//black bkgrnd
    //graphics e
    SDL_Event event;
    int quit=0;
    while(quit!=1){
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_a: p.x -= 10; break;
                    case SDLK_d: p.x += 10; break;
                    case SDLK_w: p.y -= 10; break;
                    case SDLK_s: p.y += 10; break;
                }
            }
            else if(event.type==SDL_QUIT)quit=1;
        }
        SDL_Rect r; r.x = p.x; r.y = p.y; r.w = 10; r.h = 10;
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &r);
        SDL_RenderPresent(renderer);
    }
}
