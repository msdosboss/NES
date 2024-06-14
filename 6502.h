#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>

struct CPU {
    unsigned char *programCounter;
    unsigned char accumulator;
    unsigned char x;
    unsigned char y;
    unsigned char processorStatus;
    unsigned char memMap[65536];  // 64KB memory
};

void zeroFlag(struct CPU *cpu, char reg);
void negativeFlag(struct CPU *cpu, char reg);
unsigned short absoluteAddress(struct CPU *cpu, unsigned char *startingPoint);
void lda(struct CPU *cpu);
void tax(struct CPU *cpu);
void inx(struct CPU *cpu);
void loadInstruction(struct CPU *cpu, char *instruction);
void cpuLoop(struct CPU *cpu);


#endif

