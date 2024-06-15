#include <stdio.h>
#include <assert.h>
#include "6502.h"

void testLDAImmediate(struct CPU *cpu){
	unsigned char testProgram[] = {0xa9, 0x0f};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x0f);
	assert(cpu->processorStatus == 0);
}

void testLDAZeroPage(struct CPU *cpu){
	cpu->memMap[0x0f] = 0x1f;
	unsigned char testProgram[] = {0xa5, 0x0f};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x1f);
	assert(cpu->processorStatus == 0);
	
	
}

void testLDAZeroPageX(struct CPU *cpu){	
	cpu->memMap[0x10] = 0x1f;
	cpu->x = 1;
	unsigned char testProgram[] = {0xb5, 0x0f};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x1f);
	assert(cpu->processorStatus == 0);
}

void testLDAAbsolute(struct CPU *cpu){
	cpu->memMap[0x10f] = 0x1f;
	unsigned char testProgram[] = {0xad, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x1f);
	assert(cpu->processorStatus == 0);

}

void testLDAAbsoluteX(struct CPU *cpu){	
	cpu->x = 1;
	cpu->memMap[0x0110] = 0x1f;
	unsigned char testProgram[] = {0xbd, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x1f);
	assert(cpu->processorStatus == 0);
}

void testLDAAbsoluteY(struct CPU *cpu){	
	cpu->y = 1;
	cpu->memMap[0x0110] = 0x1f;
	unsigned char testProgram[] = {0xb9, 0x0f, 0x01};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x1f);
	assert(cpu->processorStatus == 0);
}

void testLDAIndirectX(struct CPU *cpu){
	cpu->x = 16;
	cpu->memMap[0x10] = 0x11;
	cpu->memMap[0x11] = 0x22;
	cpu->memMap[0x2211] = 0x1f;
	unsigned char testProgram[] = {0xa1, 0x00};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x1f);
	assert(cpu->processorStatus == 0);
}

void testLDAIndirectY(struct CPU *cpu){
	cpu->y = 16;
	cpu->memMap[0x10] = 0x11;
	cpu->memMap[0x11] = 0x22;
	cpu->memMap[0x2211] = 0x1f;
	unsigned char testProgram[] = {0xb1, 0x00};
	cpu->programCounter = &testProgram[1];
	lda(cpu);
	assert(cpu->accumulator == 0x1f);
	assert(cpu->processorStatus == 0);
}
int main(){
	struct CPU cpu = {0};
	testLDAImmediate(&cpu);
	testLDAZeroPage(&cpu);
	testLDAZeroPageX(&cpu);
	testLDAAbsolute(&cpu);
	testLDAAbsoluteX(&cpu);
	testLDAAbsoluteY(&cpu);
	//testLDAIndirectX(&cpu);
	testLDAIndirectY(&cpu);
	printf("GGEZ\n");
	return 0;
}
