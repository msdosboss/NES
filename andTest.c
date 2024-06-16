#include <stdio.h>
#include <assert.h>
#include "6502.h"

void testANDImmediate(struct CPU *cpu){
	cpu->accumulator = 0xab;
	unsigned char testProgram[] = {0x29, 0x00};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0);
	assert(cpu->processorStatus == 2);
}

void testANDZeroPage(struct CPU *cpu){
	cpu->accumulator = 0x01;
	cpu->memMap[0x0f] = 0x05;
	unsigned char testProgram[] = {0x25, 0x0f};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0x01);
	assert(cpu->processorStatus == 0);
	
	
}

void testANDZeroPageX(struct CPU *cpu){	
	cpu->accumulator = 0x01;
	cpu->memMap[0x10] = 0x05;
	cpu->x = 1;
	unsigned char testProgram[] = {0x35, 0x0f};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0x01);
	assert(cpu->processorStatus == 0);
}

void testANDAbsolute(struct CPU *cpu){
	cpu->accumulator = 0x05;
	cpu->memMap[0x10f] = 0x01;
	unsigned char testProgram[] = {0x2d, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0x01);
	assert(cpu->processorStatus == 0);

}

void testANDAbsoluteX(struct CPU *cpu){	
	cpu->x = 1;
	cpu->accumulator = 0x05;
	cpu->memMap[0x0110] = 0x01;
	unsigned char testProgram[] = {0x3d, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0x01);
	assert(cpu->processorStatus == 0);
}

void testANDAbsoluteY(struct CPU *cpu){	
	cpu->y = 1;
	cpu->accumulator = 0x05;
	cpu->memMap[0x0110] = 0x01;
	unsigned char testProgram[] = {0x39, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0x01);
	assert(cpu->processorStatus == 0);
}

void testANDIndirectX(struct CPU *cpu){
	cpu->x = 16;
	cpu->accumulator = 0x07;
	cpu->memMap[0x10] = 0x11;
	cpu->memMap[0x11] = 0x22;
	cpu->memMap[0x2211] = 0x05;
	unsigned char testProgram[] = {0x21, 0x00};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0x05);
	assert(cpu->processorStatus == 0);
}

void testANDIndirectY(struct CPU *cpu){
	cpu->y = 16;
	cpu->accumulator = 0x0f;
	cpu->memMap[0x10] = 0x11;
	cpu->memMap[0x11] = 0x22;
	cpu->memMap[0x2211] = 0x05;
	unsigned char testProgram[] = {0x31, 0x00};
	cpu->programCounter = &testProgram[1];
	and(cpu);
	assert(cpu->accumulator == 0x05);
	assert(cpu->processorStatus == 0);
}
int main(){
	struct CPU cpu = {0};
	testANDImmediate(&cpu);
	testANDZeroPage(&cpu);
	testANDZeroPageX(&cpu);
	testANDAbsolute(&cpu);
	testANDAbsoluteX(&cpu);
	testANDAbsoluteY(&cpu);
	//testANDIndirectX(&cpu);
	//testANDIndirectY(&cpu);
	printf("GGEZ\n");
	return 0;
}
