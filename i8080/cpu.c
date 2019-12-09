#include<stdio.h>

typedef struct cpu8080{
    uint8_t *memory;
    uint8_t a, b, c, d, e, h, l; //registers (a - accumulator)
    uint16_t sp, pc;//stack pointer, program counter
    uint8_t z:1, s:1, p:1, cy:1, ac:1;//flags: zero,sign,parity,
                                            //carry,auxillary carry
} cpu8080;

uint8_t setZspac(cpu8080 *cpu, uint16_t ans){
    cpu->z=((ans & 0xff) == 0);
    cpu->s=((ans & 0x80) != 0); 
    cpu->p=(ans % 2 != 1);
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
        //case 0x02: break;
        //case 0x03: break;
        case 0x04: cpu->b++; break;
        case 0x05: cpu->b--; break;
        case 0x06: cpu->b=opcode[1]; cpu->pc+=1; break;
        //case 0x07: break;
        case 0x08: break;
        //case 0x09: break;
        //case 0x0a: break;
        //case 0x0b: break;
        case 0x0c: cpu->c++; break;
        case 0x0d: cpu->c--; break;
        case 0x0e: cpu->c=opcode[1]; cpu->pc+=1; break;
        //case 0x0f: break;
        case 0x10: break;
        case 0x11: cpu->e=opcode[1]; cpu->d=opcode[2]; cpu->pc+=2; break;
        //case 0x12: break;
        //case 0x13: break;
        case 0x14: cpu->d++; break;
        case 0x15: cpu->d--; break;
        case 0x16: cpu->d=opcode[1]; cpu->pc+=1; break;
        //case 0x17: break;
        case 0x18: break;
        //case 0x19: break;
        //case 0x1a: break;
        //case 0x1b: break;
        case 0x1c: cpu->e++; break;
        case 0x1d: cpu->e--; break;
        case 0x1e: cpu->e=opcode[1]; cpu->pc+=1; break;
        //case 0x1f: break;
        case 0x20: break;
        case 0x21: cpu->l=opcode[1]; cpu->h=opcode[2]; cpu->pc+=2; break;
        //case 0x22: break;
        //case 0x23: break;
        case 0x24: cpu->h++; break;
        case 0x25: cpu->h--; break;
        case 0x26: cpu->h=opcode[1]; cpu->pc+=1; break;
        //case 0x27: break;
        case 0x28: break;
        //case 0x29: break;
        //case 0x2a: break;
        //case 0x2b: break;
        case 0x2c: cpu->l++; break;
        case 0x2d: cpu->l--; break;
        case 0x2e: cpu->l=opcode[1]; cpu->pc+=1; break;
        //case 0x2f: break;
        case 0x30: break;
        //case 0x31: break;
        //case 0x32: break;
        //case 0x33: break;
        //case 0x34: break;
        //case 0x35: break;
        //case 0x36: break;
        //case 0x37: break;
        case 0x38: break;
        //case 0x39: break;
        //case 0x3a: break;
        //case 0x3b: break;
        case 0x3c: cpu->a++; break;
        case 0x3d: cpu->a--; break;
        case 0x3e: cpu->a=opcode[1]; cpu->pc+=1; break;
        //case 0x3f: break;
        case 0x40: break; //cpu->b=cpu->b; break;
        case 0x41: cpu->b=cpu->c; break;
        case 0x42: cpu->b=cpu->d; break;
        case 0x43: cpu->b=cpu->e; break;
        case 0x44: cpu->b=cpu->h; break;
        case 0x45: cpu->b=cpu->l; break;
        //case 0x46: cpu->b=cpu->m; break;
        case 0x47: cpu->b=cpu->a; break;
        case 0x48: cpu->c=cpu->b; break;
        case 0x49: break; //cpu->c=cpu->c; break;
        case 0x4a: cpu->c=cpu->d; break;
        case 0x4b: cpu->c=cpu->e; break;
        case 0x4c: cpu->c=cpu->h; break;
        case 0x4d: cpu->c=cpu->l; break;
        //case 0x4e: cpu->c=cpu->m; break;
        case 0x4f: cpu->c=cpu->a; break;
        case 0x50: cpu->d=cpu->b; break;
        case 0x51: cpu->d=cpu->c; break;
        case 0x52: break; //cpu->d=cpu->d; break;
        case 0x53: cpu->d=cpu->e; break;
        case 0x54: cpu->d=cpu->h; break;
        case 0x55: cpu->d=cpu->l; break;
        //case 0x56: cpu->d=cpu->m; break;
        case 0x57: cpu->d=cpu->a; break;
        case 0x58: cpu->e=cpu->b; break;
        case 0x59: cpu->e=cpu->c; break;
        case 0x5a: cpu->e=cpu->d; break;
        case 0x5b: break; //cpu->e=cpu->e; break;
        case 0x5c: cpu->e=cpu->h; break;
        case 0x5d: cpu->e=cpu->l; break;
        //case 0x5e: cpu->e=cpu->m; break;
        case 0x5f: cpu->e=cpu->a; break;
        case 0x60: cpu->h=cpu->b; break;
        case 0x61: cpu->h=cpu->c; break;
        case 0x62: cpu->h=cpu->d; break;
        case 0x63: cpu->h=cpu->e; break;
        case 0x64: break; //cpu->h=cpu->h; break;
        case 0x65: cpu->h=cpu->l; break;
        //case 0x66: cpu->h=cpu->m; break;
        case 0x67: cpu->h=cpu->a; break;
        case 0x68: cpu->l=cpu->b; break;
        case 0x69: cpu->l=cpu->c; break;
        case 0x6a: cpu->l=cpu->d; break;
        case 0x6b: cpu->l=cpu->e; break;
        case 0x6c: cpu->l=cpu->h; break;
        case 0x6d: break; //cpu->l=cpu->l; break;
        //case 0x6e: cpu->l=cpu->m; break;
        case 0x6f: cpu->l=cpu->a; break;
        //case 0x70: cpu->h=cpu->b; break;
        //case 0x71: cpu->h=cpu->c; break;
        //case 0x72: cpu->h=cpu->d; break;
        //case 0x73: cpu->h=cpu->e; break;
        //case 0x74: break; //cpu->h=cpu->h; break;
        //case 0x75: cpu->h=cpu->l; break;
        //case 0x76: break;
        //case 0x77: cpu->h=cpu->a; break;
        case 0x78: cpu->a=cpu->b; break;
        case 0x79: cpu->a=cpu->c; break;
        case 0x7a: cpu->a=cpu->d; break;
        case 0x7b: cpu->a=cpu->e; break;
        case 0x7c: cpu->a=cpu->h; break;
        case 0x7d: cpu->a=cpu->l; break;
        //case 0x7e: cpu->a=cpu->m; break;
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
        //case 0x86: break;
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
        //case 0x8e: break;
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
        //case 0x96: break;
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
        //case 0x9e: break;
        case 0x9f: {uint16_t ans=-(uint16_t)cpu->cy;
                   cpu->a=setFlags(cpu, ans); break;}
        
    }
    cpu->pc+=1;
}

int main() {
    FILE *f=fopen("invaders", "rb");
    if(f){
        fseek(f, 0L, SEEK_END);
        int fsize = ftell(f), pc=0;
        fseek(f, 0L, SEEK_SET);
        unsigned char data[fsize];
        //unsigned char *data=malloc(fsize);
        fread(data, sizeof(data), 1, f);
        fclose(f);
    }
    else printf("OOPS! SOMETHING WENT WRONG");
}
