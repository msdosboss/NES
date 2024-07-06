/*Test written by ChatGPT*/

#include "6502.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>


void test_adc() {
    struct CPU cpu;
    
    // Clear memMap and registers
    for (int i = 0; i < 0x10000; i++) {
        cpu.memMap[i] = 0;
    }
    cpu.accumulator = 0;
    cpu.x = 0;
    cpu.processorStatus = 0;
    cpu.programCounter = &cpu.memMap[1];

    // Test case: immediate mode (opcode 0x69)
    cpu.memMap[0] = 0x69; // ADC opcode
    cpu.memMap[1] = 0xd0; // Immediate value
    cpu.accumulator = 0x90;
    adc(&cpu);
	assert(cpu.accumulator == 0x60);
	assert((cpu.processorStatus & 0b01000000) != 0);
	assert((cpu.processorStatus & 0b00000001) != 0);
    //assert(cpu.programCounter == 2);

    // Test case: zero page (opcode 0x65)
    cpu.programCounter = &cpu.memMap[1];
    cpu.memMap[0] = 0x65; // ADC opcode
    cpu.memMap[1] = 0x10; // Zero-page address
    cpu.memMap[0x10] = 0x10; // Value at zero-page address
    cpu.accumulator = 0x20;
    adc(&cpu);
    assert(cpu.accumulator == 0x31);
   // assert(cpu.programCounter == 2);

    // Test case: zero page, X (opcode 0x75)
    cpu.programCounter = &cpu.memMap[1];
    cpu.memMap[0] = 0x75; // ADC opcode
    cpu.memMap[1] = 0x10; // Zero-page address
    cpu.memMap[0x15] = 0x10; // Value at zero-page address + X
    cpu.accumulator = 0x20;
    cpu.x = 0x05;
    adc(&cpu);
    assert(cpu.accumulator == 0x30);
    //assert(cpu.programCounter == 2);

    // Add more test cases for absolute, absolute,X, absolute,Y, indirect,X, and indirect,Y

    printf("All tests passed!\n");
}

int main() {
    test_adc();
    return 0;
}
