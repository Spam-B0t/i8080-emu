#include<stdio.h>
#include"cpu.c"
#include<unistd.h>//for sleep()
#include<time.h>//for time_t, clock() and interrupts handling
#include <SDL.h>//only in makefile, type "run" in playbtn with gears

typedef struct InvadersMachine{
    cpu8080 *cpu;
    uint8_t *vram;
    //uint8_t screen[256][224];
    double lastTimer;
    time_t lastInterrupt;
    double nextInterrupt;
    int whichInterrupt;
    //NSTimer *emulatorTimer;
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
            //int adr = (255-j)*(224*4) + (i*4);
            //unsigned int*p1 = (unsigned int*)(&buf[adr]);
            
            for(p=0; p<8; p++){
                SDL_Rect r; r.x = rx; r.y = ry; r.w = 3; r.h = 3;
                if ( 0!= (pxl & (1<<p))){
                    //printf("#");
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderFillRect(renderer, &r);}
                    //arcade->screen[j][i]=1;
                    //*p1=0xFFFFFFFFL;
                //else
                    //printf(" ");
                    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    //arcade->screen[j][i]=0;
                    //*p1=0x00000000L;
                //p1-=224;  //next line
                //printf("\n");
                rx+=3;
                //SDL_RenderFillRect(renderer, &r);
                //SDL_RenderPresent(renderer);
            }
        } ry+=3; rx=0;//printf("\n");
    } SDL_RenderPresent(renderer);
}

uint8_t MachineIN(InvadersMachine *arcade, uint8_t port){    
    uint8_t a=0;    
    switch(port){
        case 0: a=1;
        case 1: a=0;
        case 3:{uint16_t v = (arcade->shift1<<8) | arcade->shift0;    
                a = ((v >> (8-arcade->shift_offset)) & 0xff);} break;    
       }    
    return a;    
}

void MachineOUT(InvadersMachine *arcade, uint8_t port){    
    switch(port){    
        case 2: arcade->shift_offset = arcade->cpu->a & 0x7; break;    
        case 4: arcade->shift0 = arcade->shift1;
                arcade->shift1 = arcade->cpu->a; break;    
    }    
}

void GenerateInterrupt(cpu8080* cpu, int interrupt_num){    
    cpu->memory[cpu->sp-1]=(cpu->pc >> 8) & 0xff;
    cpu->memory[cpu->sp-2]=(cpu->pc & 0xff);
    cpu->sp-=2;
    cpu->pc = 8*interrupt_num;
    cpu->inte=0;
}

int main (int argc, char** argv) {//printf("begin");
    cpu8080 i8080; reset(&i8080);
    load(&i8080, "invaders.h", 0);
    load(&i8080, "invaders.g", 0x800);
    load(&i8080, "invaders.f", 0x1000);
    load(&i8080, "invaders.e", 0x1800);
    InvadersMachine arcade; arcade.cpu=&i8080;
    arcade.vram=&i8080.memory[0x2400];
    arcade.lastInterrupt=clock();
    time_t time;
    long t=0;
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
    for(;;){t++; //if(t>45000)draw(&arcade,buf);
        emulate8080(&i8080);
        uint8_t *opcode = &i8080.memory[i8080.pc];
        switch(*opcode){
            case 0xdb: {uint8_t port = opcode[1];
                       i8080.a = MachineIN(&arcade, port);} break;
            case 0xd3: {uint8_t port = opcode[1];
                       MachineOUT(&arcade, port);} break;
            default: break;
        }
        time=clock();
        if(time - arcade.lastInterrupt>1.0/60.0){
            if(t>40000)draw(&arcade, renderer);
            if(arcade.cpu->inte==1){
                GenerateInterrupt(&i8080, 2);
                arcade.lastInterrupt = time;
            }
        }
    }
}
