#include "cpu.c"
#include "disassembler.h"


void printstats(cpu8080 *cpu){
    printf("a=%02x bc=%02x%02x de=%02x%02x ",
            cpu->a,cpu->b,cpu->c,cpu->d,cpu->e);
    printf("hl=%02x%02x pc=%04x sp=%04x \n",
            cpu->h,cpu->l,cpu->pc,cpu->sp);
    printf("z=%d s=%d p=%d cy=%d \n",
            cpu->z,cpu->s,cpu->p,cpu->cy);
}

void jumpn(cpu8080 *cpu){
    int i, n;
    printf("Input jump size "); scanf("%d",&n); printf("\n");
    for(i=0; i<n; i++){
        disassemble8080(cpu->memory, cpu->pc);
        emulate8080(cpu);
        printstats(cpu);
        printf("\n");
    }
}

void debug(cpu8080 *cpu){
    for(;;){
        disassemble8080(cpu->memory, cpu->pc);
        emulate8080(cpu);
        printstats(cpu);
        if(getchar()=='j')jumpn(cpu);
    }
}
