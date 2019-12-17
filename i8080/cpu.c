#include<stdio.h>
#include<stdlib.h>//used for malloc()
#include "disassembler.h"

#define RET cpu->pc=((cpu->memory[cpu->sp+1]<<8) | cpu->memory[cpu->sp])-1; cpu->sp+=2;
#define CALL(adr) uint16_t ret = cpu->pc+2;cpu->memory[cpu->sp-1]=(ret >> 8) & 0xff;cpu->memory[cpu->sp-2]=(ret & 0xff);cpu->sp-=2;cpu->pc=(adr)-1;
#define JMP cpu->pc=((opcode[2] << 8) | opcode[1])-1

typedef struct cpu8080{
    uint8_t *memory;
    uint8_t a, b, c, d, e, h, l; //registers (a - accumulator)
    uint16_t sp, pc;//stack pointer, program counter
    uint8_t z:1, s:1, p:1, cy:1, ac:1;//flags: zero,sign,parity,
                                            //carry,auxillary carry
} cpu8080;

void reset(cpu8080 *cpu){
    cpu->memory=malloc(0x10000); 
    cpu->a=0; cpu->b=0; cpu->c=0;
    cpu->d=0; cpu->e=0; cpu->h=0;
    cpu->l=0; cpu->pc=0; cpu->sp=0xf000;
    cpu->z=0; cpu->s=0; cpu->p=0; cpu->cy=0;
}

void load(cpu8080 *cpu, char* fname, uint32_t adr){
    FILE *f=fopen(fname, "rb");
    if(f){
        fseek(f, 0L, SEEK_END);
        int fsize = ftell(f);
        fseek(f, 0L, SEEK_SET);
        fread(&cpu->memory[adr], fsize, 1, f);
        fclose(f);
    }
    else printf("OOPS! SOMETHING WENT WRONG");
}

uint8_t setZspac(cpu8080 *cpu, uint16_t ans){
    cpu->z=((ans & 0xff) == 0);
    cpu->s=((ans & 0x80) != 0); 
    cpu->p=(ans % 2 == 0);
    //cpu->ac=
    return ans & 0xff;
}

uint8_t setCy(cpu8080 *cpu, uint16_t ans){
    cpu->cy=(ans > 0xff);
    return ans & 0xff;
}

uint8_t setFlags(cpu8080 *cpu, uint16_t ans){
    setZspac(cpu, ans);
    setCy(cpu, ans);
    return ans & 0xff;
}

void emulate8080(cpu8080 *cpu){
    unsigned char *opcode = &cpu->memory[cpu->pc];
    switch(*opcode){
        case 0x00: break;
        case 0x01: cpu->c=opcode[1]; cpu->b=opcode[2]; cpu->pc+=2; break;
        case 0x02: {uint16_t adr=(cpu->b<<8) | (cpu->c);
                    cpu->memory[adr]=cpu->a;} break;
        case 0x03: {uint16_t bc=(cpu->b<<8) | (cpu->c); bc++;
                   cpu->b=(bc>>8) & 0xff; cpu->c=bc & 0xff;} break;
        case 0x04: cpu->b=setZspac(cpu, ((uint16_t)cpu->b+1)); break;
        case 0x05: cpu->b=setZspac(cpu, ((uint16_t)cpu->b-1)); break;
        case 0x06: cpu->b=opcode[1]; cpu->pc+=1; break;
        case 0x07: {uint8_t ans = cpu->a;
                    cpu->a = ((ans & (1<<7)) >> 7) | (ans << 1);
                    cpu->cy = (1 == (ans&(1<<7)));} break;
        case 0x08: break;
        case 0x09: {uint16_t hl=(cpu->h<<8) | (cpu->l);
                    uint16_t bc=(cpu->b<<8) | (cpu->c);
                    hl+=bc; setCy(cpu, hl);
                    cpu->h=(hl>>8) & 0xff; cpu->l=hl & 0xff;} break;
        case 0x0a: {uint16_t adr=(cpu->b<<8) | (cpu->c);
                    cpu->a=cpu->memory[adr];} break;
        case 0x0b: {uint16_t bc=(cpu->b<<8) | (cpu->c); bc--;
                   cpu->b=(bc>>8) & 0xff; cpu->c=bc & 0xff;} break;
        case 0x0c: cpu->c=setZspac(cpu, ((uint16_t)cpu->c+1)); break;
        case 0x0d: cpu->c=setZspac(cpu, ((uint16_t)cpu->c-1)); break;
        case 0x0e: cpu->c=opcode[1]; cpu->pc+=1; break;
        case 0x0f: {uint8_t ans = cpu->a;
                    cpu->a = ((ans & 1) << 7) | (ans >> 1);
                    cpu->cy = (1 == (ans&1));} break;
        case 0x10: break;
        case 0x11: cpu->e=opcode[1]; cpu->d=opcode[2]; cpu->pc+=2; break;
        case 0x12: {uint16_t adr=(cpu->d<<8) | (cpu->e);
                    cpu->memory[adr]=cpu->a;} break;
        case 0x13: {uint16_t de=(cpu->d<<8) | (cpu->e); de++;
                   cpu->d=(de>>8) & 0xff; cpu->e=de & 0xff;} break;
        case 0x14: cpu->d=setZspac(cpu, ((uint16_t)cpu->d+1)); break;
        case 0x15: cpu->d=setZspac(cpu, ((uint16_t)cpu->d-1)); break;
        case 0x16: cpu->d=opcode[1]; cpu->pc+=1; break;
        case 0x17: {uint8_t ans = cpu->a;
                    cpu->a = (cpu->cy) | (ans << 1);
                    cpu->cy = (1 == (ans&(1<<7)));} break;
        case 0x18: break;
        case 0x19: {uint16_t hl=(cpu->h<<8) | (cpu->l);
                    uint16_t de=(cpu->d<<8) | (cpu->e); 
                    hl+=de; setCy(cpu, hl);
                    cpu->h=(hl>>8) & 0xff; cpu->l=hl & 0xff;} break;
        case 0x1a: {uint16_t adr=(cpu->d<<8) | (cpu->e);
                    cpu->a=cpu->memory[adr];} break;
        case 0x1b: {uint16_t de=(cpu->d<<8) | (cpu->e); de--;
                   cpu->d=(de>>8) & 0xff; cpu->e=de & 0xff;} break;
        case 0x1c: cpu->e=setZspac(cpu, ((uint16_t)cpu->e+1)); break;
        case 0x1d: cpu->e=setZspac(cpu, ((uint16_t)cpu->e-1)); break;
        case 0x1e: cpu->e=opcode[1]; cpu->pc+=1; break;
        case 0x1f: {uint8_t ans = cpu->a;
                    cpu->a = (((ans>>7) & 1) << 7) | (ans >> 1);
                    cpu->cy = (1 == (ans&1));} break;
        case 0x20: break;
        case 0x21: cpu->l=opcode[1]; cpu->h=opcode[2]; cpu->pc+=2; break;
        case 0x22: {uint16_t adr=(opcode[2] << 8) | opcode[1];
                    cpu->memory[adr]=cpu->l; cpu->memory[adr+1]=cpu->h;
                    cpu->pc+=2;} break;
        case 0x23: {uint16_t hl=(cpu->h<<8) | (cpu->l); hl++;
                   cpu->h=(hl>>8) & 0xff; cpu->l=hl & 0xff;} break;
        case 0x24: cpu->h=setZspac(cpu, ((uint16_t)cpu->h+1)); break;
        case 0x25: cpu->h=setZspac(cpu, ((uint16_t)cpu->h-1)); break;
        case 0x26: cpu->h=opcode[1]; cpu->pc+=1; break;
        //case 0x27: break; spec
        case 0x28: break;
        case 0x29: {uint16_t hl=(cpu->h<<8) | (cpu->l);
                    hl+=hl; setCy(cpu, hl);
                    cpu->h=(hl>>8) & 0xff; cpu->l=hl & 0xff;} break;
        case 0x2a: {uint16_t adr=(opcode[2] << 8) | opcode[1];
                    cpu->l=cpu->memory[adr]; cpu->h=cpu->memory[adr+1];
                    cpu->pc+=2;} break;
        case 0x2b: {uint16_t hl=(cpu->h<<8) | (cpu->l); hl--;
                   cpu->h=(hl>>8) & 0xff; cpu->l=hl & 0xff;} break;
        case 0x2c: cpu->l=setZspac(cpu, ((uint16_t)cpu->l+1)); break;
        case 0x2d: cpu->l=setZspac(cpu, ((uint16_t)cpu->l-1)); break;
        case 0x2e: cpu->l=opcode[1]; cpu->pc+=1; break;
        case 0x2f: cpu->a=~cpu->a; break;
        case 0x30: break;
        case 0x31: cpu->sp=(opcode[2] << 8) | opcode[1]; cpu->pc+=2; break;
        case 0x32: {uint16_t adr=(opcode[2] << 8) | opcode[1];
                   cpu->memory[adr]=cpu->a; cpu->pc+=2;} break;
        case 0x33: cpu->sp++; break;
        case 0x34: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint16_t ans=(uint16_t)cpu->memory[adr]+1;
                    cpu->memory[adr]=setZspac(cpu, ans);} break;
        case 0x35: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint16_t ans=(uint16_t)cpu->memory[adr]-1;
                    cpu->memory[adr]=setZspac(cpu, ans);} break;
        case 0x36: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=opcode[1]; cpu->pc+=1;} break;
        case 0x37: cpu->cy=1; break;
        case 0x38: break;
        case 0x39: {uint16_t hl=(cpu->h<<8) | (cpu->l);
                    hl+=cpu->sp; setCy(cpu, hl);
                    cpu->h=(hl>>8) & 0xff; cpu->l=hl & 0xff;} break;
        case 0x3a: {uint16_t adr=(opcode[2] << 8) | opcode[1];
                   cpu->a=cpu->memory[adr]; cpu->pc+=2;} break;
        case 0x3b: cpu->sp--; break;
        case 0x3c: cpu->a=setZspac(cpu, ((uint16_t)cpu->a+1)); break;
        case 0x3d: cpu->a=setZspac(cpu, ((uint16_t)cpu->a-1)); break;
        case 0x3e: cpu->a=opcode[1]; cpu->pc+=1; break;
        case 0x3f: cpu->cy=~cpu->cy; break;
        case 0x40: break; //cpu->b=cpu->b; break;
        case 0x41: cpu->b=cpu->c; break;
        case 0x42: cpu->b=cpu->d; break;
        case 0x43: cpu->b=cpu->e; break;
        case 0x44: cpu->b=cpu->h; break;
        case 0x45: cpu->b=cpu->l; break;
        case 0x46: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->b=cpu->memory[adr];} break;
        case 0x47: cpu->b=cpu->a; break;
        case 0x48: cpu->c=cpu->b; break;
        case 0x49: break; //cpu->c=cpu->c; break;
        case 0x4a: cpu->c=cpu->d; break;
        case 0x4b: cpu->c=cpu->e; break;
        case 0x4c: cpu->c=cpu->h; break;
        case 0x4d: cpu->c=cpu->l; break;
        case 0x4e: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->c=cpu->memory[adr];} break;
        case 0x4f: cpu->c=cpu->a; break;
        case 0x50: cpu->d=cpu->b; break;
        case 0x51: cpu->d=cpu->c; break;
        case 0x52: break; //cpu->d=cpu->d; break;
        case 0x53: cpu->d=cpu->e; break;
        case 0x54: cpu->d=cpu->h; break;
        case 0x55: cpu->d=cpu->l; break;
        case 0x56: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->d=cpu->memory[adr];} break;
        case 0x57: cpu->d=cpu->a; break;
        case 0x58: cpu->e=cpu->b; break;
        case 0x59: cpu->e=cpu->c; break;
        case 0x5a: cpu->e=cpu->d; break;
        case 0x5b: break; //cpu->e=cpu->e; break;
        case 0x5c: cpu->e=cpu->h; break;
        case 0x5d: cpu->e=cpu->l; break;
        case 0x5e: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->e=cpu->memory[adr];} break;
        case 0x5f: cpu->e=cpu->a; break;
        case 0x60: cpu->h=cpu->b; break;
        case 0x61: cpu->h=cpu->c; break;
        case 0x62: cpu->h=cpu->d; break;
        case 0x63: cpu->h=cpu->e; break;
        case 0x64: break; //cpu->h=cpu->h; break;
        case 0x65: cpu->h=cpu->l; break;
        case 0x66: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->h=cpu->memory[adr];} break;
        case 0x67: cpu->h=cpu->a; break;
        case 0x68: cpu->l=cpu->b; break;
        case 0x69: cpu->l=cpu->c; break;
        case 0x6a: cpu->l=cpu->d; break;
        case 0x6b: cpu->l=cpu->e; break;
        case 0x6c: cpu->l=cpu->h; break;
        case 0x6d: break; //cpu->l=cpu->l; break;
        case 0x6e: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->l=cpu->memory[adr];} break;
        case 0x6f: cpu->l=cpu->a; break;
        case 0x70: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=cpu->b;} break;
        case 0x71: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=cpu->c;} break;
        case 0x72: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=cpu->d;} break;
        case 0x73: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=cpu->e;} break;
        case 0x74: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=cpu->h;} break;
        case 0x75: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=cpu->l;} break;
        //case 0x76: break; HALT
        case 0x77: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->memory[adr]=cpu->a;} break;
        case 0x78: cpu->a=cpu->b; break;
        case 0x79: cpu->a=cpu->c; break;
        case 0x7a: cpu->a=cpu->d; break;
        case 0x7b: cpu->a=cpu->e; break;
        case 0x7c: cpu->a=cpu->h; break;
        case 0x7d: cpu->a=cpu->l; break;
        case 0x7e: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    cpu->a=cpu->memory[adr];} break;
        case 0x7f: break; //cpu->a=cpu->a; break;
        case 0x80: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->b;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x81: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->c;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x82: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->d;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x83: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->e;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x84: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->h;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x85: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->l;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x86: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->memory[adr];
                    cpu->a=setFlags(cpu, ans);} break;
        case 0x87: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->a;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x88: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->b+(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x89: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->c+(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x8a: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->d+(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x8b: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->e+(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x8c: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->h+(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x8d: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->l+(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x8e: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->memory[adr]+(uint16_t)cpu->cy;
                    cpu->a=setFlags(cpu, ans);} break;
        case 0x8f: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)cpu->a+(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x90: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->b;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x91: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->c;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x92: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->d;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x93: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->e;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x94: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->h;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x95: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->l;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x96: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->memory[adr];
                    cpu->a=setFlags(cpu, ans);} break;
        case 0x97: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->a;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x98: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->b-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x99: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->c-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x9a: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->d-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x9b: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->e-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x9c: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->h-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x9d: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->l-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0x9e: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->memory[adr]-(uint16_t)cpu->cy;
                    cpu->a=setFlags(cpu, ans);} break;
        case 0x9f: {uint16_t ans=-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa0: {uint8_t ans=cpu->a & cpu->b;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa1: {uint8_t ans=cpu->a & cpu->c;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa2: {uint8_t ans=cpu->a & cpu->d;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa3: {uint8_t ans=cpu->a & cpu->e;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa4: {uint8_t ans=cpu->a & cpu->h;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa5: {uint8_t ans=cpu->a & cpu->l;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa6: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint8_t ans=cpu->a & cpu->memory[adr];
                    cpu->a=setFlags(cpu, ans);} break;
        case 0xa7: {uint8_t ans=cpu->a & cpu->a;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa8: {uint8_t ans=cpu->a ^ cpu->b;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xa9: {uint8_t ans=cpu->a ^ cpu->c;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xaa: {uint8_t ans=cpu->a ^ cpu->d;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xab: {uint8_t ans=cpu->a ^ cpu->e;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xac: {uint8_t ans=cpu->a ^ cpu->h;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xad: {uint8_t ans=cpu->a ^ cpu->l;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xae: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint8_t ans=cpu->a ^ cpu->memory[adr];
                    cpu->a=setFlags(cpu, ans);} break;
        case 0xaf: {uint8_t ans=cpu->a ^ cpu->a;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb0: {uint8_t ans=cpu->a | cpu->b;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb1: {uint8_t ans=cpu->a | cpu->c;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb2: {uint8_t ans=cpu->a | cpu->d;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb3: {uint8_t ans=cpu->a | cpu->e;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb4: {uint8_t ans=cpu->a | cpu->h;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb5: {uint8_t ans=cpu->a | cpu->l;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb6: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint8_t ans=cpu->a | cpu->memory[adr];
                    cpu->a=setFlags(cpu, ans);} break;
        case 0xb7: {uint8_t ans=cpu->a | cpu->a;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xb8: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->b;
                   setFlags(cpu, ans); break;}
        case 0xb9: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->c;
                   setFlags(cpu, ans); break;}
        case 0xba: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->d;
                   setFlags(cpu, ans); break;}
        case 0xbb: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->e;
                   setFlags(cpu, ans); break;}
        case 0xbc: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->h;
                   setFlags(cpu, ans); break;}
        case 0xbd: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->l;
                   setFlags(cpu, ans); break;}
        case 0xbe: {uint16_t adr=(cpu->h<<8) | (cpu->l);
                    uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->memory[adr];
                    setFlags(cpu, ans);} break;
        case 0xbf: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)cpu->a;
                   cpu->a=setFlags(cpu, ans); break;}
        case 0xc0: if(cpu->z==0){RET} break;
        case 0xc1: {cpu->c=cpu->memory[cpu->sp];
                    cpu->b=cpu->memory[cpu->sp+1];
                    cpu->sp+=2;} break;
        case 0xc2: if(cpu->z==0)cpu->pc=((opcode[2] << 8) | opcode[1])-1; 
                   else cpu->pc+=2; break;
        case 0xc3: cpu->pc=((opcode[2] << 8) | opcode[1])-1; break;//JMP
        case 0xc4: if(cpu->z==0){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xc5: {cpu->memory[cpu->sp-2]=cpu->c;
                    cpu->memory[cpu->sp-1]=cpu->b;
                    cpu->sp-=2;} break;
        case 0xc6: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)opcode[1];
                    cpu->a=setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xc7: {CALL((uint16_t)0)} break;
        case 0xc8: if(cpu->z==1){RET} break;
        case 0xc9: {cpu->pc=((cpu->memory[cpu->sp+1]<<8) | cpu->memory[cpu->sp])-1;
                    cpu->sp+=2;} break;//RET
        case 0xca: if(cpu->z==1)JMP; 
                   else cpu->pc+=2; break;
        case 0xcb: break;
        case 0xcc: if(cpu->z==1){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xcd: {uint16_t ret = cpu->pc+2;//CALL
                    cpu->memory[cpu->sp-1] = (ret >> 8) & 0xff;
                    cpu->memory[cpu->sp-2] = (ret & 0xff);
                    cpu->sp-=2;//is &0xff necessary here?
                    cpu->pc = ((opcode[2] << 8) | opcode[1])-1;} break;
        case 0xce: {uint16_t ans=(uint16_t)cpu->a+(uint16_t)opcode[1]+(uint16_t)cpu->cy;
                    cpu->a=setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xcf: {CALL((uint16_t)8)} break;
        case 0xd0: if(cpu->cy==0){RET} break;
        case 0xd1: {cpu->e=cpu->memory[cpu->sp];
                    cpu->d=cpu->memory[cpu->sp+1];
                    cpu->sp+=2;} break;
        case 0xd2: if(cpu->cy==0)JMP; 
                      else cpu->pc+=2; break;
        //case 0xd3: break; spec
        case 0xd4: if(cpu->cy==0){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xd5: {cpu->memory[cpu->sp-2]=cpu->e;
                    cpu->memory[cpu->sp-1]=cpu->d;
                    cpu->sp-=2;} break;
        case 0xd6: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)opcode[1];
                    cpu->a=setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xd7: {CALL((uint16_t)0x10)} break;
        case 0xd8: if(cpu->cy==1){RET} break;
        case 0xd9: break;
        case 0xda: if(cpu->cy!=0)JMP; 
                      else cpu->pc+=2; break;
        //case 0xdb: break; spec
        case 0xdc: if(cpu->cy==1){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xdd: break;
        case 0xde: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)opcode[1]-(uint16_t)cpu->cy;
                    cpu->a=setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xdf: {CALL((uint16_t)0x18)} break;
        case 0xe0: if(cpu->p==0){RET} break;
        case 0xe1: {cpu->l=cpu->memory[cpu->sp];
                    cpu->h=cpu->memory[cpu->sp+1];
                    cpu->sp+=2;} break;
        case 0xe2: if(cpu->p==0)JMP; 
                      else cpu->pc+=2; break;
        case 0xe3: {uint8_t x=cpu->memory[cpu->sp];
                      cpu->memory[cpu->sp]=cpu->l;
                      cpu->l=x;
                      x=cpu->memory[cpu->sp+1];
                      cpu->memory[cpu->sp+1]=cpu->h;
                      cpu->h=x;} break;
        case 0xe4: if(cpu->p==0){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xe5: {cpu->memory[cpu->sp-2]=cpu->l;
                    cpu->memory[cpu->sp-1]=cpu->h;
                    cpu->sp-=2;} break;
        case 0xe6: {uint8_t ans=cpu->a & opcode[1];
                   cpu->a=setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xe7: {CALL((uint16_t)0x20)} break;
        case 0xe8: if(cpu->p==1){RET} break;
        case 0xe9: cpu->pc=cpu->h<<8 | cpu->l; break;
        case 0xea: if(cpu->p==1)JMP; 
                      else cpu->pc+=2; break;
        case 0xeb: {uint8_t x=cpu->h; cpu->h=cpu->d; cpu->d=x;
                    x=cpu->l; cpu->l=cpu->e; cpu->e=x;} break;
        case 0xec: if(cpu->p==1){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xed: break;
        case 0xee: {uint8_t ans=cpu->a ^ opcode[1];
                   cpu->a=setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xef: {CALL((uint16_t)0x28)} break;
        case 0xf0: if(cpu->s==0){RET} break;
        case 0xf1: {cpu->a = cpu->memory[cpu->sp+1];
                    uint8_t psw = cpu->memory[cpu->sp];
                    cpu->z = (0x01 == (psw & 0x01));
                    cpu->s = (0x02 == (psw & 0x02));
                    cpu->p = (0x04 == (psw & 0x04));
                    cpu->cy = (0x08 == (psw & 0x08));
                    cpu->ac = (0x10 == (psw & 0x10));
                    cpu->sp += 2;} break;
        case 0xf2: if(cpu->s==0)JMP; 
                      else cpu->pc+=2; break;
        //case 0xf3: break; spec
        case 0xf4: if(cpu->s==0){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xf5: {cpu->memory[cpu->sp-1] = cpu->a;
                    uint8_t psw=(cpu->z | cpu->s<<1 | cpu->p<<2 | cpu->cy<<3 | cpu->ac<<4);
                    cpu->memory[cpu->sp-2] = psw;
                    cpu->sp -= 2;} break;
        case 0xf6: {uint8_t ans=cpu->a | opcode[1];
                   cpu->a=setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xf7: {CALL((uint16_t)0x30)} break;
        case 0xf8: if(cpu->s==1){RET} break;
        case 0xf9: cpu->sp=(cpu->h<<8) | (cpu->l); break;
        case 0xfa: if(cpu->s==1)JMP; 
                      else cpu->pc+=2; break;
        //case 0xfb: break; spec
        case 0xfc: if(cpu->s==1){CALL((opcode[2]<<8) | opcode[1])} break;
        case 0xfd: break;
        case 0xfe: {uint16_t ans=(uint16_t)cpu->a-(uint16_t)opcode[1];
                    setFlags(cpu, ans); cpu->pc+=1;} break;
        case 0xff: {CALL((uint16_t)0x38)} break;
    } 
    cpu->pc+=1;
}


int main() {
    cpu8080 i8080;
    reset(&i8080);
    load(&i8080, "invaders.h", 0);
    load(&i8080, "invaders.g", 0x800);
    load(&i8080, "invaders.f", 0x1000);
    load(&i8080, "invaders.e", 0x1800);
    while(1==1){
        disassemble8080(i8080.memory, i8080.pc);
        emulate8080(&i8080);
        printf("a=%02x bc=%02x%02x de=%02x%02x ",
          i8080.a,i8080.b,i8080.c,i8080.d,i8080.e);
        printf("hl=%02x%02x pc=%04x sp=%04x \n",
          i8080.h,i8080.l,i8080.pc,i8080.sp);
        printf("z=%d s=%d p=%d cy=%d \n",
          i8080.z,i8080.s,i8080.p,i8080.cy);
        getchar();
    }
}
