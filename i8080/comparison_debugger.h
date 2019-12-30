#include "debugger.h"
#include "8080emu.c"


int main() {//j 1500
    cpu8080 i8080;
    reset(&i8080);i8080.sp=0x00;
    //miller srt
    int vblankcycles = 0; uint8_t i_num=2;
	  State8080* state = Init8080();	
  	ReadFileIntoMemoryAt(state, "invaders.h", 0);
  	ReadFileIntoMemoryAt(state, "invaders.g", 0x800);
  	ReadFileIntoMemoryAt(state, "invaders.f", 0x1000);
  	ReadFileIntoMemoryAt(state, "invaders.e", 0x1800);
  	//miller end
    load(&i8080, "invaders.h", 0);
    load(&i8080, "invaders.g", 0x800);
    load(&i8080, "invaders.f", 0x1000);
    load(&i8080, "invaders.e", 0x1800);
    //debug(&i8080);
    int frag=0; long t=0; uint16_t last=0x10;
    for(;;){
        Emulate8080Op(state);
        printf("PC %04x\n", state->pc);
        //printf("mmil:%04x\n",((state->memory[0x23fb]<<8) | state->memory[0x23fa]));
        printf("------------------------\n");
        disassemble8080(i8080.memory, i8080.pc);
        emulate8080(&i8080);
        //printf("mme:%04x\n",((i8080.memory[0x23fb]<<8) | i8080.memory[0x23fa]));
        printstats(&i8080);
        printf("------------------------\n");
        if(i8080.cc>=16667){
            GenerateInterrupt(state, i_num);
            i_num= i_num%2+1;
            processInterrupts(last, &i8080);
            last=last % 0x10+0x08;
        }
        if(i8080.a!=state->a){printf("a is bad\n"); frag=1;}
        if(i8080.b!=state->b){printf("b is bad\n"); frag=1;}
        if(i8080.c!=state->c){printf("c is bad\n"); frag=1;}
        if(i8080.d!=state->d){printf("d is bad\n"); frag=1;}
        if(i8080.e!=state->e){printf("e is bad\n"); frag=1;}
        if(i8080.h!=state->h){printf("h is bad\n"); frag=1;}
        if(i8080.l!=state->l){printf("l is bad\n"); frag=1;}
        if(i8080.sp!=state->sp){printf("sp is bad\n"); frag=1;}
        if(i8080.pc!=state->pc){printf("pc is bad\n"); frag=1;}
        if(i8080.z!=state->cc.z){printf("z is bad\n"); frag=1;}
        if(i8080.s!=state->cc.s){printf("s is bad\n"); frag=1;}
        if(i8080.p!=state->cc.p){printf("p is bad\n"); frag=1;}
        if(i8080.cy!=state->cc.cy){printf("cy is bad\n"); frag=1;}
        t++;
        /*if(((i8080.memory[0x23fb]<<8) | i8080.memory[0x23fa])!=
        ((state->memory[0x23fb]<<8) | state->memory[0x23fa]))
        {printf("CORRUPTED MEM\n");
        printf("mmil:%04x\n",((state->memory[0x23fb]<<8) | state->memory[0x23fa]));
        printf("mme:%04x\n",((i8080.memory[0x23fb]<<8) | i8080.memory[0x23fa]));
        break;}*/
        if(frag!=0 || t==100000)break;
    }printf("\nt=%d", t);
}
