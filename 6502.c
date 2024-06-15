#include "6502.h"

/*struct CPU{
	unsigned char accumulator, x, y, processorStatus;
	unsigned char memMap[65536];
	unsigned char *programCounter;
};*/

void zeroFlag(struct CPU *cpu, char reg){
	if(reg == 0){
		cpu->processorStatus = cpu->processorStatus | 0b00000010;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b11111101;
	}
}

void negativeFlag(struct CPU *cpu, char reg){
	if((reg & 0b10000000) != 0){
		cpu->processorStatus = cpu->processorStatus | 0b10000000;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b01111111;
	}

}

unsigned short absoluteAddress(struct CPU *cpu, unsigned char *startingPoint){	//this is kind of a bad name because it's used in indirect address mode as well
	unsigned short address = *(startingPoint + sizeof(unsigned char));	//putting the 2 arg byte in front because this is little eddien 
	address = address << 8;	//making room for the first arg
	address = address | *(startingPoint);	//this line assumes that the bits being shifted in are all zero. I dont know this to be the case
	return address;
}

void lda(struct CPU *cpu){
	unsigned char arg;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0xa9:{	//immediate
			arg = *(cpu->programCounter);
			cpu->accumulator = arg;
			break;
		}		

		case 0xa5:{	//zero page
			arg = cpu->memMap[*(cpu->programCounter)];
			cpu->accumulator = arg;
			break;
		}
		
		case 0xb5:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			arg = cpu->memMap[address];
			cpu->accumulator = arg;
			break;
		}
		
		case 0xad:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address];
			cpu->accumulator = arg;
			break;
		}

		case 0xbd:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->x];	//this is what makes it different from absolute
			cpu->accumulator = arg;
			break;
		}

		case 0xb9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->y];	//this is what makes it different from absolute
			cpu->accumulator = arg;
			break;
		}
		
		case 0xa1:{	//(indirect,X)
			unsigned char indirectAddress = *(cpu->programCounter);
			indirectAddress = indirectAddress + cpu->x;
			unsigned short address = absoluteAddress(cpu, &(cpu->memMap[indirectAddress]));
			arg = cpu->memMap[address];
			cpu->accumulator = arg;
			break;
		}

		case 0xb1:{	//(indirect,Y)
			unsigned char indirectAddress = *(cpu->programCounter);
			indirectAddress = indirectAddress + cpu->y;
			unsigned short address = absoluteAddress(cpu, &(cpu->memMap[indirectAddress]));
			arg = cpu->memMap[address];
			cpu->accumulator = arg;
			break;
		}

		default:{
			printf("instruction %x is not part of LDA\n", *(cpu->programCounter - sizeof(unsigned char)));
			break;
		}
			
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);	
}

void tax(struct CPU *cpu){
	cpu->x = cpu->accumulator;
	
	zeroFlag(cpu, cpu->x);
	
	negativeFlag(cpu, cpu->x);
	
}

void tay(struct CPU *cpu){
	cpu->y = cpu->accumulator;
	
	zeroFlag(cpu, cpu->y);
	
	negativeFlag(cpu, cpu->y);
	
}

void txa(struct CPU *cpu){
	cpu->accumulator = cpu->x;
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
	
}

void tya(struct CPU *cpu){
	cpu->accumulator = cpu->y;
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
	
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
			case 0xa5:
			case 0xb5:
			case 0xad:
			case 0xbd:
			case 0xb9:
			case 0xa1:
			case 0xb1:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				lda(cpu);
				break;
			}
			
			case 0xaa:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				tax(cpu);
				break;
			}
			
			case 0xa8:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				tay(cpu);
				break;
			}
			
			case 0x8a:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				txa(cpu);
				break;
			}

			case 0x98:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				tya(cpu);
				break;
			}			

			case 0xe8:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				inx(cpu);
				break;
			}

			default:{
				printf("%x instructions does not exist yet\n", *(cpu->programCounter));
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				break;
			}
		}
	}
}

int main(){	
	unsigned char instructions[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00, '\n'};
	
	unsigned char *stackPointer;

	struct CPU cpu = {0};

	cpu.programCounter = &(cpu.memMap[0x8000]);
	
	stackPointer = &(cpu.memMap[0x1ff]);	//stackPointer goes from [0x100-0x1ff] starting at the top and working its way down

	loadInstructions(&cpu, instructions);

	cpuLoop(&cpu);

	printf("accumulator = %x\n", cpu.accumulator);
	printf("x = %x\n", cpu.x);
	printf("processorStatus = %d\n", cpu.processorStatus);
	
}
