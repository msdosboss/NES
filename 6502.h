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

void zeroFlag(struct CPU *cpu, unsigned char reg);
void negativeFlag(struct CPU *cpu, unsigned char reg);
void carryFlag(struct CPU *cpu, unsigned char reg);
unsigned short absoluteAddress(struct CPU *cpu, unsigned char *startingPoint);
unsigned short indirectYAddress(struct CPU *cpu);
unsigned short indirectXAddress(struct CPU *cpu);
void asl(struct CPU *cpu);
void and(struct CPU *cpu);
void lda(struct CPU *cpu);
void tax(struct CPU *cpu);
void inx(struct CPU *cpu);
void loadInstruction(struct CPU *cpu, char *instruction);
void cpuLoop(struct CPU *cpu);


#endif

