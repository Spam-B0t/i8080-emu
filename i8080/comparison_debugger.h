#include "debugger.h"
#include "8080emu.c"

typedef struct imme{
    cpu8080 *cpu;
    uint8_t in_port1;
    uint8_t shift0;
    uint8_t shift1;
    uint8_t shift_offset;
    uint8_t c;
}imme;

typedef struct immil{
    State8080 *state;
    uint8_t in_port1;
    uint8_t shift0;
    uint8_t shift1;
    uint8_t shift_offset;
    uint8_t c;
}immil;

void resetIM(imme *im, immil *im2){
    im->in_port1=0x0; im->shift0=0x0; im->shift1=0x0;
    im->shift_offset=0x0; im->c=0;
    im2->in_port1=0x0; im2->shift0=0x0; im2->shift1=0x0;
    im2->shift_offset=0x0; im2->c=0;
}

void handleinput(imme *im, cpu8080 *cpu, immil *im2, State8080 *state){
        unsigned char *opcode = &cpu->memory[cpu->pc]; uint8_t out=0;
        switch(*opcode){
            case 0xdb: {uint8_t port = opcode[1];
                        switch(port){
                            case 0: out=1; break;
                            case 1: if(im->c==0){out=im->in_port1 |= 0x01;im->c=1;}//coin down
                                    else if(im->c==1){out=im->in_port1 &= ~0x01;im->c=2;} //coin up
                                    else if(im->c==2){out=im->in_port1 |= 0x04;im->c=3;} //start down
                                    else if(im->c==3){out=im->in_port1 &= ~0x04;im->c=0;} break;//start up
                            case 3:{uint16_t v = (im->shift1<<8) | im->shift0;
                                    out = ((v >> (8-im->shift_offset)) & 0xff);} break;
                        }
                        cpu->a = out;} break;
            case 0xd3: {uint8_t port = opcode[1];
                        switch(port){
                           case 2: im->shift_offset = cpu->a & 0x7; break;
                           case 4: {im->shift0 = im->shift1;
                                    im->shift1 = cpu->a;} break;
                        }
                       } break;
            default: break;
        }
        unsigned char *opcode2 = &state->memory[state->pc]; uint8_t out2=0;
        switch(*opcode){
            case 0xdb: {uint8_t port = opcode[1]; uint8_t out2=0;
                        switch(port){
                           case 0: out2=1; break;
                           case 1: if(im2->c==0){out2=im2->in_port1 |= 0x01;im2->c=1;}
                                   else if(im2->c==1){out2=im2->in_port1 &= ~0x01;im2->c=2;}
                                   else if(im2->c==2){out2=im2->in_port1 |= 0x04;im2->c=3;}
                                   else if(im2->c==3){out2=im2->in_port1 &= ~0x04;im2->c=0;} break;
                           case 3:{uint16_t v = (im2->shift1<<8) | im2->shift0;
                                   out2 = ((v >> (8-im2->shift_offset)) & 0xff);} break;
                        }
                        state->a = out2;} break;
            case 0xd3: {uint8_t port = opcode[1];
                        switch(port){
                           case 2: im2->shift_offset = state->a & 0x7; break;
                           case 4: {im2->shift0 = im2->shift1;
                                    im2->shift1 = state->a;} break;
                        }
                       } break;
            default: break;
        }
}

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
    //machines strt
    imme im; immil im2; resetIM(&im, &im2);
    im.cpu=&i8080; im2.state=state;
    //machines end
    int frag=0; long t=0; uint16_t last=0x10;
    for(;;){
        handleinput(&im, &i8080, &im2, state);
        Emulate8080Op(state);
        printf("PC %04x\n", state->pc);
        printf("mmil:%04x\n",((state->memory[0x23fb]<<8) | state->memory[0x23fa]));
        printf("------------------------\n");
        disassemble8080(i8080.memory, i8080.pc);
        emulate8080(&i8080);
        printf("mme:%04x\n",((i8080.memory[0x23fb]<<8) | i8080.memory[0x23fa]));
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
        if(im.in_port1!=im2.in_port1){printf("port1 is bad\n"); frag=1;}
        if(im.shift0!=im2.shift0){printf("shift0 is bad\n"); frag=1;}
        if(im.shift1!=im2.shift1){printf("shift1 is bad\n"); frag=1;}
        if(im.shift_offset!=im2.shift_offset){printf("shift_offset is bad\n"); frag=1;}
        t++;
        if(((i8080.memory[0x23fb]<<8) | i8080.memory[0x23fa])!=
        ((state->memory[0x23fb]<<8) | state->memory[0x23fa]))
        {printf("CORRUPTED MEM\n");
        printf("mmil:%04x\n",((state->memory[0x23fb]<<8) | state->memory[0x23fa]));
        printf("mme:%04x\n",((i8080.memory[0x23fb]<<8) | i8080.memory[0x23fa]));
        break;}
        if(frag!=0 || t==200000)break;
    }printf("\nt=%d", t);
}
