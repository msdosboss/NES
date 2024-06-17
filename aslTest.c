#include <stdio.h>
#include <assert.h>
#include "6502.h"

void testASLAccumlator(struct CPU *cpu){
	cpu->accumulator = 0b1;
	unsigned char testProgram[] = {0x0a, 0x00};
	cpu->programCounter = &testProgram[1];
	asl(cpu);
	assert(cpu->accumulator == 0b10);
	assert(cpu->processorStatus == 0);
}

void testASLZeroPage(struct CPU *cpu){
	cpu->memMap[0x0f] = 0b1;
	unsigned char testProgram[] = {0x06, 0x0f};
	cpu->programCounter = &testProgram[1];
	asl(cpu);
	assert(cpu->memMap[0x0f] == 0b10);
	assert(cpu->processorStatus == 0);
	
	
}

void testASLZeroPageX(struct CPU *cpu){	
	cpu->memMap[0x10] = 0b10;
	cpu->x = 1;
	unsigned char testProgram[] = {0x16, 0x0f};
	cpu->programCounter = &testProgram[1];
	asl(cpu);
	assert(cpu->memMap[0x10] == 0b100);
	assert(cpu->processorStatus == 0);
}

void testASLAbsolute(struct CPU *cpu){
	cpu->memMap[0x10f] = 0b1000;
	unsigned char testProgram[] = {0x0e, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	asl(cpu);
	assert(cpu->memMap[0x10f] == 0b10000);
	assert(cpu->processorStatus == 0);

}

void testASLAbsoluteX(struct CPU *cpu){	
	cpu->x = 1;
	cpu->memMap[0x0110] = 0b111;
	unsigned char testProgram[] = {0x1e, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	asl(cpu);
	assert(cpu->memMap[0x110] == 0b1110);
	assert(cpu->processorStatus == 0);
}

int main(){
	struct CPU cpu = {0};
	testASLAccumlator(&cpu);
	testASLZeroPage(&cpu);
	testASLZeroPageX(&cpu);
	testASLAbsolute(&cpu);
	testASLAbsoluteX(&cpu);
	printf("GGEZ\n");
	return 0;
}
