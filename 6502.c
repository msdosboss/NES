#include <stdio.h>
#include <stdlib.h>

struct CPU{
	unsigned char accumulator, x, y, processorStatus;
	unsigned char memMap[65536];
	unsigned char *programCounter;
};

void zeroFlag(struct CPU *cpu, char reg){
	if(reg == 0){
		cpu->processorStatus = cpu->processorStatus | 0b00000010;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b11111101;
	}
}

void negativeFlag(struct CPU *cpu, char reg){
	if(reg & 0b10000000 != 0){
		cpu->processorStatus = cpu->processorStatus | 0b10000000;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b01111111;
	}

}

void lda(struct CPU *cpu){
	unsigned char arg = *(cpu->programCounter);
	cpu->accumulator = arg;
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);	
}

void tax(struct CPU *cpu){
	cpu->x = cpu->accumulator;
	
	zeroFlag(cpu, cpu->x);
	
	negativeFlag(cpu, cpu->x);
	
}

void inx(struct CPU *cpu){
	cpu->x++;

	zeroFlag(cpu, cpu->x);

	negativeFlag(cpu, cpu->x);
}

void loadInstructions(struct CPU *cpu, char *instructions){
	for(int i = 0; instructions[i] != '\n'; i++){
		(cpu->programCounter[i]) = instructions[i];
	}
}

void cpuLoop(struct CPU *cpu){
	while(*(cpu->programCounter) != 0){
		switch(*(cpu->programCounter)){
			case 0xa9:
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				lda(cpu);
			
			case 0xaa:
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				tax(cpu);
			
			case 0xe8:
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				inx(cpu);
			
			default:
				printf("%x instructions does not exist yet\n", *(cpu->programCounter));
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
		}
	}
}

int main(){	
	unsigned char instructions[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00, '\n'};
	
	unsigned char *stackPointer;

	struct CPU cpu;	

	cpu.programCounter = &(cpu.memMap[0x600]);
	
	stackPointer = &(cpu.memMap[0x1ff]);	//stackPointer goes from [0x100-0x1ff] starting at the top and working its way down

	loadInstructions(&cpu, instructions);

	cpuLoop(&cpu);

	printf("accumulator = %x\n", cpu.accumulator);
	printf("x = %x\n", cpu.x);
}
