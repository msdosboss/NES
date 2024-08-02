#ifndef CPU_H
#define CPU_H

#define RAMOFFSET 0

#include <stdio.h>
#include <stdlib.h>
#include "bus.h"
//#include "log.h"
#include "opcode.h"

struct CPU {
	unsigned short PC;
	unsigned char *stackPointer;
	unsigned char accumulator;
	unsigned char x;
	unsigned char y;
	unsigned char processorStatus;
	struct Bus bus;
};

void zeroFlag(struct CPU *cpu, unsigned char reg);
void negativeFlag(struct CPU *cpu, unsigned char reg);
void carryFlag(struct CPU *cpu, unsigned char reg);
void overFlag(struct CPU *cpu, unsigned char reg);
unsigned short absoluteAddress(struct CPU *cpu, unsigned short startingPoint);
unsigned short rollOverAbsoluteAddress(struct CPU *cpu, unsigned char startingPoint);
unsigned short indirectYAddress(struct CPU *cpu);
unsigned short indirectXAddress(struct CPU *cpu);
void push(struct CPU *cpu, unsigned char val);
unsigned char pop(struct CPU *cpu);
unsigned short popAbsoluteAddress(struct CPU *cpu);
void adc(struct CPU *cpu);
void asl(struct CPU *cpu);
void and(struct CPU *cpu);
void bcc(struct CPU *cpu);
void bcs(struct CPU *cpu);
void beq(struct CPU *cpu);
void bit(struct CPU *cpu);
void bmi(struct CPU *cpu);
void bne(struct CPU *cpu);
void bpl(struct CPU *cpu);
void bvc(struct CPU *cpu);
void bvs(struct CPU *cpu);
void clc(struct CPU *cpu);
void cld(struct CPU *cpu);
void cli(struct CPU *cpu);
void clv(struct CPU *cpu);
void cmp(struct CPU *cpu);
void cpx(struct CPU *cpu);
void cpy(struct CPU *cpu);
void dec(struct CPU *cpu);
void dex(struct CPU *cpu);
void dey(struct CPU *cpu);
void eor(struct CPU *cpu);
void inc(struct CPU *cpu);
void inx(struct CPU *cpu);
void iny(struct CPU *cpu);
void jmp(struct CPU *cpu);
void jsr(struct CPU *cpu);
void lda(struct CPU *cpu);
void ldx(struct CPU *cpu);
void ldy(struct CPU *cpu);
void lsr(struct CPU *cpu);
void ora(struct CPU *cpu);
void pha(struct CPU *cpu);
void php(struct CPU *cpu);
void pla(struct CPU *cpu);
void plp(struct CPU *cpu);
void rol(struct CPU *cpu);
void ror(struct CPU *cpu);
void rti(struct CPU *cpu);
void rts(struct CPU *cpu);
void sbc(struct CPU *cpu);
void sec(struct CPU *cpu);
void sed(struct CPU *cpu);
void sei(struct CPU *cpu);
void sta(struct CPU *cpu);
void stx(struct CPU *cpu);
void sty(struct CPU *cpu);
void tax(struct CPU *cpu);
void tay(struct CPU *cpu);
void tsx(struct CPU *cpu);
void txa(struct CPU *cpu);
void txs(struct CPU *cpu);
void tya(struct CPU *cpu);
void loadInstruction(struct CPU *cpu, char *instruction, int instructionsLen);
void cpuLoop(struct CPU *cpu);
void initCPU(struct CPU *cpu, unsigned char *instructions, int instructionsLen);

#endif

