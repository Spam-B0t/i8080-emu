#include<stdio.h>
#include"cpu.h"
#include <SDL.h>//only in makefile, type "run" in playbtn with gears

typedef struct InvadersMachine{
    cpu8080 *cpu;
    uint8_t *vram;
    uint16_t last_interrupt;
    uint8_t in_port1;
    uint8_t shift0; //LSB of Space Invader's external shift hardware
    uint8_t shift1; //MSB
    uint8_t shift_offset;
}InvadersMachine;

void draw(InvadersMachine *arcade, SDL_Renderer *renderer){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    int i=0x2400, j, rx=0, ry=0; 
    for(i=0; i<224; i++){
        for(j=0; j<256; j+= 8){
            int p;
            uint8_t pxl = arcade->vram[(i*(256/8)) + j/8];
            for(p=0; p<8; p++){
                SDL_Rect r; r.x = rx; r.y = ry; r.w = 3; r.h = 3;
                if ( 0!= (pxl & (1<<p))){
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &r);}
                rx+=3;
            }
        } ry+=3; rx=0;
    } SDL_RenderPresent(renderer);
}

uint8_t MachineIN(InvadersMachine *arcade, uint8_t port){    
    uint8_t a=0;    
    switch(port){
        case 0: a=1; break;
        case 1: { SDL_Event event;
                  while(SDL_PollEvent(&event)){
                     switch(event.type){
                         case SDL_KEYDOWN:{
                             switch(event.key.keysym.sym){
                                 case SDLK_c: arcade->in_port1 |= 0x01; break;//coin
                                 case SDLK_LEFT: arcade->in_port1 |= 0x20; break;//left
                                 case SDLK_RIGHT: arcade->in_port1 |= 0x40; break;//right
                                 case SDLK_SPACE: arcade->in_port1 |= 0x10; break;//fire
                                 case SDLK_RETURN: arcade->in_port1 |= 0x04; break;//start
                                 default: break;
                             }
                         } break;
                         case SDL_KEYUP:{
                             switch(event.key.keysym.sym){
                                 case SDLK_c: arcade->in_port1 &= ~0x01; break;//coin
                                 case SDLK_LEFT: arcade->in_port1 &= ~0x20; break;//left
                                 case SDLK_RIGHT: arcade->in_port1 &= ~0x40; break;//right
                                 case SDLK_SPACE: arcade->in_port1 &= ~0x10; break;//fire
                                 case SDLK_RETURN: arcade->in_port1 &= ~0x04; break;//start
                                 default: break;
                             }
                        } break;
                     }
              } a=arcade->in_port1;} break;
        case 3:{uint16_t v = (arcade->shift1<<8) | arcade->shift0;    
                a = ((v >> (8-arcade->shift_offset)) & 0xff);} break;    
       }    
    return a;    
}

void MachineOUT(InvadersMachine *arcade, uint8_t port){    
    switch(port){    
        case 2: arcade->shift_offset = arcade->cpu->a & 0x7; break;    
        case 4: {arcade->shift0 = arcade->shift1;
                 arcade->shift1 = arcade->cpu->a;} break;    
    }    
}

void handleinput(cpu8080 *cpu, InvadersMachine *arcade){
        unsigned char *opcode = &cpu->memory[cpu->pc];
        switch(*opcode){
            case 0xdb: {uint8_t port = opcode[1];
                       cpu->a = MachineIN(arcade, port);} break;
            case 0xd3: {uint8_t port = opcode[1];
                       MachineOUT(arcade, port);} break;
            default: break;
        }
}

void processInterrupts(uint16_t last, cpu8080 *cpu){
    if(cpu->cc>=16667){
        cpu->cc-=16667;
        if(cpu->inte==1){
            cpu->memory[cpu->sp-1]=(cpu->pc >> 8) & 0xff;
            cpu->memory[cpu->sp-2]=(cpu->pc & 0xff);
            cpu->sp-=2;
            cpu->pc=last; cpu->inte=0;
        }
    }
}

int main (int argc, char** argv) {
    cpu8080 i8080; reset(&i8080);
    load(&i8080, "invaders.h", 0);
    load(&i8080, "invaders.g", 0x800);
    load(&i8080, "invaders.f", 0x1000);
    load(&i8080, "invaders.e", 0x1800);
    InvadersMachine arcade; arcade.cpu=&i8080;
    arcade.last_interrupt=0x08;
    arcade.vram=&i8080.memory[0x2400];
    uint16_t last=0x10;
    //graphics s
    SDL_Window* window = NULL;
    window = SDL_CreateWindow
    (
        "Space Invaders!", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        224,
        256,
        SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);//black bkgrnd
    //graphics e
    //input s
    arcade.in_port1=0x0;
    unsigned long t=0;
    //input e
    for(;;){
        t++;
        handleinput(&i8080, &arcade);
        emulate8080(&i8080);
        if(i8080.cc>=16667){
            processInterrupts(last, &i8080);
            last=last % 0x10+0x08;
            draw(&arcade, renderer);
        }
    }
}
