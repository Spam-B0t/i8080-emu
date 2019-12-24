#include<stdio.h>
#include"cpu.c"
#include<unistd.h>//for sleep()
#include<time.h>//for time_t, clock() and interrupts handling
//#include <SDL.h>//only in makefile, type "run" in playbtn with gears

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

void draw(InvadersMachine *arcade/*, uint8_t *buf, int h, int w*/){
    int i=0x2400, j;
    for(i=0; i<224; i++){
        for(j=0; j<256; j+= 8){
            int p;
            uint8_t pxl = arcade->vram[(i*(256/8)) + j/8];
            //int adr = (255-j)*(224*4) + (i*4);
            //unsigned int*p1 = (unsigned int*)(&buf[adr]);
            
            for(p=0; p<8; p++){
                if ( 0!= (pxl & (1<<p)))
                    printf("#");
                    //arcade->screen[j][i]=1;
                    //*p1=0xFFFFFFFFL;
                else
                    printf(" ");
                    //arcade->screen[j][i]=0;
                    //*p1=0x00000000L;
                //p1-=224;  //next line
                //printf("\n");
            }
        } printf("\n");
    } //int t;sleep(1); for(t=0;t<300;t++)printf("\n");
    /*for(i=0; i<256; i++){for(j=0; j<224; j+=8){
    if(arcade->screen[i][j]==1)printf("#");
    else printf(" ");} printf("\n");}*/
}

uint8_t MachineIN(InvadersMachine *arcade, uint8_t port){    
    uint8_t a;    
    switch(port){
        case 0: return 1;
        case 1: return 0;
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
    //Set the PC to the low memory vector.
    //This is identical to an "RST interrupt_num" instruction.
    cpu->pc = interrupt_num;
}

int main(int argc, char* argv[]){//printf("begin");
    cpu8080 i8080; reset(&i8080);
    load(&i8080, "invaders.h", 0);
    load(&i8080, "invaders.g", 0x800);
    load(&i8080, "invaders.f", 0x1000);
    load(&i8080, "invaders.e", 0x1800);
    InvadersMachine arcade; arcade.cpu=&i8080;
    arcade.vram=&i8080.memory[0x2400];
    arcade.lastInterrupt=clock();
    time_t time;
    int t=0; //uint8_t *buf = malloc(4 * 224*256);
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
            if(t>45000)draw(&arcade/*,buf*/);
            if(arcade.cpu->inte==1){
                GenerateInterrupt(&i8080, 2);
                arcade.lastInterrupt = time;
            }
        }
    }
}
