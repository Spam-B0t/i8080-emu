#include "cpu.h"
#include "disassembler.h"


void printstats(cpu8080 *cpu){ //printf("probreg:%02x%02x\n",cpu->memory[0x23fd], cpu->memory[0x23fc]);
    printf("a=%02x bc=%02x%02x de=%02x%02x ",
            cpu->a,cpu->b,cpu->c,cpu->d,cpu->e);
    printf("hl=%02x%02x pc=%04x sp=%04x \n",
            cpu->h,cpu->l,cpu->pc,cpu->sp);
    printf("z=%d s=%d p=%d i=%d cy=%d cycles=%d \n",
            cpu->z,cpu->s,cpu->p,cpu->inte,cpu->cy,cpu->cc);
}
//void tst(cpu8080 *cpu){if(cpu->cc>=16667) cpu->cc-=16667;}
void processInterrupts(uint16_t last, cpu8080 *cpu){
    if(cpu->cc>=16667){
        cpu->cc-=16667;
        //last=last % 0x10+0x08;
        if(cpu->inte==1){
            cpu->memory[cpu->sp-1]=(cpu->pc >> 8) & 0xff;
            cpu->memory[cpu->sp-2]=(cpu->pc & 0xff);
            cpu->sp-=2; //cpu->cc+=11;
            cpu->pc=last; cpu->inte=0;
        }
    }
}
void jumpn(uint16_t last, cpu8080 *cpu){
    int i, n;
    printf("Input jump size "); scanf("%d",&n); printf("\n");
    for(i=0; i<n; i++){
        disassemble8080(cpu->memory, cpu->pc);
        emulate8080(cpu);
        processInterrupts(last, cpu);
        printstats(cpu);
        printf("\n");
    }
}

void debug(cpu8080 *cpu){
    uint16_t last=0x08; //23fd 23fe
    /*int pmem[100][3]; int i=0,j; long t;
    pmem[0][0]=cpu->memory[0x23fd]<<8 | cpu->memory[0x23fc];
    pmem[0][1]=0; pmem[0][2]=0;*/
    for(;;){ //int ocode=cpu->memory[cpu->pc];
        disassemble8080(cpu->memory, cpu->pc);
        emulate8080(cpu);
        processInterrupts(last, cpu);
        printstats(cpu);
        if(getchar()=='j')jumpn(last, cpu);
        /*uint16_t x=cpu->memory[0x23fd]<<8 | cpu->memory[0x23fc];
        if(x!=pmem[i][0]){pmem[i+1][0]=x;pmem[i+1][2]=t;
        pmem[i+1][1]=ocode; i++;}*/
    }
    /*for(j=0;j<i;j++)printf("%04x op:%02x t:%d\n"
    ,pmem[j][0],pmem[j][1], pmem[j][2]);*/
}
