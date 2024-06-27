#include "6502.h"

/*struct CPU{
	unsigned char accumulator, x, y, processorStatus;
	unsigned char memMap[65536];
	unsigned char *programCounter;
	unsigned char *stackPointer;
};*/

void zeroFlag(struct CPU *cpu, unsigned char reg){
	if(reg == 0){
		cpu->processorStatus = cpu->processorStatus | 0b00000010;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b11111101;
	}
}

void negativeFlag(struct CPU *cpu, unsigned char reg){
	if((reg & 0b10000000) != 0){
		cpu->processorStatus = cpu->processorStatus | 0b10000000;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b01111111;
	}
}

void carryFlag(struct CPU *cpu, unsigned char reg){
	if((reg & 0b10000000) != 0){
		cpu->processorStatus = cpu->processorStatus | 0b00000001;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b11111110;
	}
}

void overFlag(struct CPU *cpu, unsigned char reg){
	if((reg & 0b01000000) != 0){
		cpu->processorStatus = cpu->processorStatus | 0b01000000;
	}
	else{
		cpu->processorStatus = cpu->processorStatus & 0b10111111;
	}
}

unsigned short absoluteAddress(struct CPU *cpu, unsigned char *startingPoint){	//this is kind of a bad name because it's used in indirect address mode as well
	unsigned short address = *(startingPoint + sizeof(unsigned char));	//putting the 2 arg byte in front because this is little eddien 
	address = address << 8;	//making room for the first arg
	address = address | *(startingPoint);	//this line assumes that the bits being shifted in are all zero. I dont know this to be the case
	return address;
}

unsigned short indirectXAddress(struct CPU *cpu){
	unsigned char indirectAddress = *(cpu->programCounter);
	indirectAddress = indirectAddress + cpu->x;
	return absoluteAddress(cpu, &(cpu->memMap[indirectAddress]));
}

unsigned short indirectYAddress(struct CPU *cpu){
	unsigned char indirectAddress = *(cpu->programCounter);
	return absoluteAddress(cpu, &(cpu->memMap[indirectAddress])) + cpu->y;
}

unsigned short indirectAddress(struct CPU *cpu){
	unsigned short indirectAddress = absoluteAddress(cpu, cpu->programCounter);
	return absoluteAddress(cpu, &(cpu->memMap[indirectAddress]));
}

void push(struct CPU *cpu, unsigned char val){
	cpu->stackPointer = cpu->stackPointer - sizeof(unsigned char);
	*cpu->stackPointer = val;
}

unsigned char pop(struct CPU *cpu){
	cpu->stackPointer = cpu->stackPointer + sizeof(unsigned char);
	return *(cpu->stackPointer - sizeof(unsigned char));
}

unsigned short popAbsoluteAddress(struct CPU *cpu){
	unsigned short address = 0;
	address |= (((unsigned short)pop(cpu)) << 8);	//this should take what is returned from pop and store it in the last 8 bits of address
	address |= pop(cpu);
	return address;
}

void adc(struct CPU *cpu){
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x69:{
			unsigned short temp = cpu->accumulator + *(cpu->programCounter) + (cpu->processorStatus & 0b00000001);
			if(temp > 0xff){
				cpu->processorStatus |= 0b00000001;
			}
			else{
				cpu->processorStatus &= 0b11111110;
			}
			cpu->accumulator = temp & 0xff;
		}
	}
	zeroFlag(cpu, cpu->accumulator);
	negativeFlag(cpu, cpu->accumulator);
}

void and(struct CPU *cpu){
	unsigned char arg;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x29:{	//immediate
			arg = *(cpu->programCounter);
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}
		
		case 0x25:{	//zero page
			arg = cpu->memMap[*(cpu->programCounter)];
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}

		case 0x35:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}
		case 0x2d:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator & arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);	//note absolute addressing take 3 bytes so the program counter will have to be interated twice for the absolute calls
			break;
		}
		
		case 0x3d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->x];
			cpu->accumulator = cpu->accumulator & arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		
		case 0x39:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->y];
			cpu->accumulator = cpu->accumulator & arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		case 0x21:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}
		case 0x31:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator & arg;
			break;
		
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	
	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);
}

void asl(struct CPU *cpu){
	switch(*(cpu->programCounter - sizeof(unsigned char))){	//note with these shifts I am still assuming that when you shift a bit in from the left it will always be 0, if that is not the case this will not work
		case 0x0a:{	//accumulator
			carryFlag(cpu, cpu->accumulator);
			cpu->accumulator = cpu->accumulator << 1;
			negativeFlag(cpu, cpu->accumulator);
			zeroFlag(cpu, cpu->accumulator);
			break;
		}

		case 0x06:{	//zero page
			carryFlag(cpu, cpu->memMap[*(cpu->programCounter)]);
			cpu->memMap[*(cpu->programCounter)] = cpu->memMap[*(cpu->programCounter)] << 1;
			negativeFlag(cpu, cpu->memMap[*(cpu->programCounter)]);
			zeroFlag(cpu, cpu->memMap[*(cpu->programCounter)]);
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		
		case 0x16:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			carryFlag(cpu, cpu->memMap[address]);
			cpu->memMap[address] = cpu->memMap[address] << 1;
			negativeFlag(cpu, cpu->memMap[address]);
			zeroFlag(cpu, cpu->memMap[address]);
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

		case 0x0e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			carryFlag(cpu, cpu->memMap[address]);
			cpu->memMap[address] = cpu->memMap[address] << 1;
			negativeFlag(cpu, cpu->memMap[address]);
			zeroFlag(cpu, cpu->memMap[address]);
			cpu->programCounter = cpu->programCounter + (sizeof(unsigned char) * 2);
			break;
		}

		case 0x1e:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			address = address + cpu->x; 
			carryFlag(cpu, cpu->memMap[address]);
			cpu->memMap[address] = cpu->memMap[address] << 1;
			negativeFlag(cpu, cpu->memMap[address]);
			zeroFlag(cpu, cpu->memMap[address]);
			cpu->programCounter = cpu->programCounter + (sizeof(unsigned char) * 2);
			break;
		}
	}
}

void bcc(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000001) == 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void bcs(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000001) != 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void beq(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000010) != 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void bit(struct CPU *cpu){
	unsigned char result;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x24:{
			result = *(cpu->programCounter) & cpu->accumulator;
			overFlag(cpu, *(cpu->programCounter));
			negativeFlag(cpu, *(cpu->programCounter));
			break;
		}

		case 0x2c:{
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			result = cpu->memMap[address] & cpu->accumulator;
			overFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	zeroFlag(cpu, result);
}

void bmi(struct CPU *cpu){
	if((cpu->processorStatus & 0b10000000) == 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void bne(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000010) == 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void bpl(struct CPU *cpu){
	if((cpu->processorStatus & 0b10000000) == 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void bvc(struct CPU *cpu){
	if((cpu->processorStatus & 0b01000000) == 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void bvs(struct CPU *cpu){
	if((cpu->processorStatus & 0b01000000) != 0){
		cpu->programCounter = cpu->programCounter + (char)*(cpu->programCounter);
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
}

void clc(struct CPU *cpu){
	cpu->processorStatus = cpu->processorStatus & 0b11111110;
}

void cld(struct CPU *cpu){
	cpu->processorStatus = cpu->processorStatus & 0b11110111;
}

void cli(struct CPU *cpu){
	cpu->processorStatus = cpu->processorStatus & 0b11111011;
}

void clv(struct CPU *cpu){
	cpu->processorStatus = cpu->processorStatus & 0b10111111;
}

void cmp(struct CPU *cpu){
	unsigned char result;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0xc9:{	//immediate
			result = cpu->accumulator - *(cpu->programCounter);
			if(cpu->accumulator >= *(cpu->programCounter)){
				if(cpu->accumulator == *(cpu->programCounter)){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}

		case 0xc5:{	//zero page
			result = cpu->accumulator - cpu->memMap[*(cpu->programCounter)];
			if(cpu->accumulator >= cpu->memMap[*(cpu->programCounter)]){
				if(cpu->accumulator == cpu->memMap[*(cpu->programCounter)]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}

		case 0xd5:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			result = cpu->accumulator - cpu->memMap[address];
			if(cpu->accumulator >= cpu->memMap[address]){
				if(cpu->accumulator == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}
		case 0xcd:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			result = cpu->accumulator - cpu->memMap[address];
			if(cpu->accumulator >= cpu->memMap[address]){
				if(cpu->accumulator == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		case 0xdd:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter) + cpu->x;
			result = cpu->accumulator - cpu->memMap[address];
			if(cpu->accumulator >= cpu->memMap[address]){
				if(cpu->accumulator == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		case 0xd9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter) + cpu->y;
			result = cpu->accumulator - cpu->memMap[address];
			if(cpu->accumulator >= cpu->memMap[address]){
				if(cpu->accumulator == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		case 0xc1:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			result = cpu->accumulator - cpu->memMap[address];
			if(cpu->accumulator >= cpu->memMap[address]){
				if(cpu->accumulator == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}

		case 0xd1:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			result = cpu->accumulator - cpu->memMap[address];
			if(cpu->accumulator >= cpu->memMap[address]){
				if(cpu->accumulator == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	negativeFlag(cpu, result);
}

void cpx(struct CPU *cpu){
	unsigned char result;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0xe0:{	//immediate
			result = cpu->x - *(cpu->programCounter);
			if(cpu->x >= *(cpu->programCounter)){
				if(cpu->x == *(cpu->programCounter)){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}

		case 0xe4:{	//zero page
			result = cpu->x - cpu->memMap[*(cpu->programCounter)];
			if(cpu->x >= cpu->memMap[*(cpu->programCounter)]){
				if(cpu->x == cpu->memMap[*(cpu->programCounter)]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}

		case 0xec:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			result = cpu->x - cpu->memMap[address];
			if(cpu->x >= cpu->memMap[address]){
				if(cpu->x == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	negativeFlag(cpu, result);
}

void cpy(struct CPU *cpu){
	unsigned char result;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0xc0:{	//immediate
			result = cpu->x - *(cpu->programCounter);
			if(cpu->y >= *(cpu->programCounter)){
				if(cpu->y == *(cpu->programCounter)){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}

		case 0xc4:{	//zero page
			result = cpu->y - cpu->memMap[*(cpu->programCounter)];
			if(cpu->y >= cpu->memMap[*(cpu->programCounter)]){
				if(cpu->y == cpu->memMap[*(cpu->programCounter)]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			break;
		}

		case 0xcc:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			result = cpu->y - cpu->memMap[address];
			if(cpu->y >= cpu->memMap[address]){
				if(cpu->y == cpu->memMap[address]){
					zeroFlag(cpu, 0);	//set zero flag on
				}
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	negativeFlag(cpu, result);
}

void dec(struct CPU *cpu){
	unsigned char result;
	switch(*(cpu->programCounter) - sizeof(unsigned char)){
		case 0xc6:{
			result = --cpu->memMap[*(cpu->programCounter)];
			break;
		}

		case 0xd6:{
			result = --cpu->memMap[(*(cpu->programCounter) + cpu->x)];
			break;
		}

		case 0xce:{
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			result = --cpu->memMap[address];
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

		case 0xde:{
			unsigned short address = absoluteAddress(cpu, cpu->programCounter) + cpu->x;
			result = --cpu->memMap[address];
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	zeroFlag(cpu, result);
	negativeFlag(cpu, result);
}

void dex(struct CPU *cpu){
	cpu->x--;
	zeroFlag(cpu, cpu->x);
	negativeFlag(cpu, cpu->x);
}

void dey(struct CPU *cpu){
	cpu->y--;
	zeroFlag(cpu, cpu->y);
	negativeFlag(cpu, cpu->y);
}

void eor(struct CPU *cpu){
	unsigned char arg;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x49:{	//immediate
			arg = *(cpu->programCounter);
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}
		
		case 0x45:{	//zero page
			arg = cpu->memMap[*(cpu->programCounter)];
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}

		case 0x55:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}
		case 0x4d:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator ^ arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);	//note absolute addressing take 3 bytes so the program counter will have to be interated twice for the absolute calls
			break;
		}
		
		case 0x5d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->x];
			cpu->accumulator = cpu->accumulator ^ arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		
		case 0x59:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->y];
			cpu->accumulator = cpu->accumulator ^ arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		case 0x41:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}
		case 0x51:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	
	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);
}

void inc(struct CPU *cpu){
	unsigned char result;
	switch(*(cpu->programCounter) - sizeof(unsigned char)){
		case 0xe6:{
			result = ++cpu->memMap[*(cpu->programCounter)];
			break;
		}

		case 0xf6:{
			result = ++cpu->memMap[(*(cpu->programCounter) + cpu->x)];
			break;
		}

		case 0xee:{
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			result = ++cpu->memMap[address];
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

		case 0xfe:{
			unsigned short address = absoluteAddress(cpu, cpu->programCounter) + cpu->x;
			result = ++cpu->memMap[address];
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	zeroFlag(cpu, result);
	negativeFlag(cpu, result);
}

void inx(struct CPU *cpu){
	cpu->x++;

	zeroFlag(cpu, cpu->x);

	negativeFlag(cpu, cpu->x);
}

void iny(struct CPU *cpu){
	cpu->y++;

	zeroFlag(cpu, cpu->y);

	negativeFlag(cpu, cpu->y);
}

void jmp(struct CPU *cpu){
	switch(*(cpu->programCounter) - sizeof(unsigned char)){
		case 0x4c:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			cpu->programCounter = &cpu->memMap[address];
			break;
		}
		case 0x6c:{	//indirect
			unsigned short address = indirectAddress(cpu);
			cpu->programCounter = &cpu->memMap[address]; 
			break;
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char) * 2;
}

void jsr(struct CPU *cpu){
	unsigned short index = cpu->programCounter - (cpu->memMap + 1);
	push(cpu, (unsigned char)(index & 0xff00));	//pushes the first 8 bits of the address to stack
	index = index >> 8;
	push(cpu, (unsigned char)(index & 0xff00));
	unsigned short address = absoluteAddress(cpu, cpu->programCounter);
	cpu->programCounter = &cpu->memMap[address];
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char) * 2;
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
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

		case 0xbd:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->x];	//this is what makes it different from absolute
			cpu->accumulator = arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

		case 0xb9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->y];	//this is what makes it different from absolute
			cpu->accumulator = arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		
		case 0xa1:{	//(indirect,X)
			unsigned short address = indirectXAddress(cpu);
			arg = cpu->memMap[address];
			cpu->accumulator = arg;
			break;
		}

		case 0xb1:{	//(indirect,Y)
			unsigned short address = indirectYAddress(cpu);
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

void ldx(struct CPU *cpu){
	unsigned char arg;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0xa2:{	//immediate
			arg = *(cpu->programCounter);
			cpu->x = arg;
			break;
		}		

		case 0xa6:{	//zero page
			arg = cpu->memMap[*(cpu->programCounter)];
			cpu->x = arg;
			break;
		}
		
		case 0xb6:{	//zero page,Y
			unsigned char address = *(cpu->programCounter) + cpu->y;
			arg = cpu->memMap[address];
			cpu->x = arg;
			break;
		}
		
		case 0xae:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address];
			cpu->x = arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

		case 0xbe:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->y];	//this is what makes it different from absolute
			cpu->x = arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}	

		default:{
			printf("instruction %x is not part of LDX\n", *(cpu->programCounter - sizeof(unsigned char)));
			break;
		}
			
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	
	zeroFlag(cpu, cpu->x);
	
	negativeFlag(cpu, cpu->x);	
}

void ldy(struct CPU *cpu){
	unsigned char arg;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0xa2:{	//immediate
			arg = *(cpu->programCounter);
			cpu->y = arg;
			break;
		}		

		case 0xa6:{	//zero page
			arg = cpu->memMap[*(cpu->programCounter)];
			cpu->y = arg;
			break;
		}
		
		case 0xb6:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			arg = cpu->memMap[address];
			cpu->y = arg;
			break;
		}
		
		case 0xae:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address];
			cpu->y = arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}

		case 0xbe:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->x];	//this is what makes it different from absolute
			cpu->y = arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}	

		default:{
			printf("instruction %x is not part of LDY\n", *(cpu->programCounter - sizeof(unsigned char)));
			break;
		}
			
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	
	zeroFlag(cpu, cpu->y);
	
	negativeFlag(cpu, cpu->y);	
}

void lsr(struct CPU *cpu){
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x4a:{	//accumulator
			carryFlag(cpu, (cpu->accumulator << 7));	//The carryFlag func checks if the last bit is on and then sets the flag accordingly. In this use of it we are trying to see if the bit that is being shifted out (0th bit) is on.
			cpu->accumulator = cpu->accumulator >> 1;
			zeroFlag(cpu, cpu->accumulator);
			negativeFlag(cpu, cpu->accumulator);
			break;
		}
		
		case 0x46:{	//zero page
			carryFlag(cpu, (cpu->memMap[*(cpu->programCounter)] << 7));
			cpu->memMap[*(cpu->programCounter)] = cpu->memMap[*(cpu->programCounter)] >> 1;
			zeroFlag(cpu, cpu->memMap[*(cpu->programCounter)]);
			negativeFlag(cpu, cpu->memMap[*(cpu->programCounter)]);
			cpu->programCounter += sizeof(unsigned char);
			break;
		}

		case 0x56:{	//zero page,X
			carryFlag(cpu, (cpu->memMap[*(cpu->programCounter) + cpu->x] << 7));
			cpu->memMap[*(cpu->programCounter) + cpu->x] = cpu->memMap[*(cpu->programCounter) + cpu->x] >> 1;
			zeroFlag(cpu, cpu->memMap[*(cpu->programCounter) + cpu->x]);
			negativeFlag(cpu, cpu->memMap[*(cpu->programCounter) + cpu->x]);
			cpu->programCounter += sizeof(unsigned char);
			break;
		}

		case 0x4e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			carryFlag(cpu, (cpu->memMap[address] << 7));
			cpu->memMap[address] >>= 1;
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char) * 2;
		}

		case 0x5e:{	//absoult,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter) + cpu->x;
			carryFlag(cpu, (cpu->memMap[address] << 7));
			cpu->memMap[address] >>= 1;
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char) * 2;
		}
	}
}

void ora(struct CPU *cpu){
	unsigned char arg;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x09:{	//immediate
			arg = *(cpu->programCounter);
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}
		
		case 0x05:{	//zero page
			arg = cpu->memMap[*(cpu->programCounter)];
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}

		case 0x15:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}
		case 0x0d:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator | arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);	//note absolute addressing take 3 bytes so the program counter will have to be interated twice for the absolute calls
			break;
		}
		
		case 0x1d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->x];
			cpu->accumulator = cpu->accumulator | arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		
		case 0x19:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			arg = cpu->memMap[address + cpu->y];
			cpu->accumulator = cpu->accumulator | arg;
			cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
			break;
		}
		case 0x01:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}
		case 0x11:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			arg = cpu->memMap[address];
			cpu->accumulator = cpu->accumulator | arg;
			break;
		
		}
	}
	cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
	
	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);
}

void pha(struct CPU *cpu){
	push(cpu, cpu->accumulator);
}

void php(struct CPU *cpu){
	push(cpu, cpu->processorStatus);
}

void pla(struct CPU *cpu){
	cpu->accumulator = pop(cpu);

	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
}

void plp(struct CPU *cpu){
	cpu->processorStatus = pop(cpu);
}

void rol(struct CPU *cpu){
	unsigned char preShiftVal;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x2a:{	//accumulator
			preShiftVal = cpu->accumulator;
			cpu->accumulator <<= 1;
			cpu->accumulator |= (cpu->processorStatus & 0b00000001);	//accumulator should be being ored by 0b0000000X with the x being the first bit (carry bit) from processorStatus
			carryFlag(cpu, preShiftVal);	//storing the 7th bit of the accumulator in carry flag from before the accumulator was shifted
			zeroFlag(cpu, cpu->accumulator);
			negativeFlag(cpu, cpu->accumulator);
		}

		case 0x26:{	//zero page
			unsigned char address = *(cpu->programCounter);
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] <<= 1;
			cpu->memMap[address] |= (cpu->processorStatus & 0b00000001);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char);
		}

		case 0x36:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] <<= 1;
			cpu->memMap[address] |= (cpu->processorStatus & 0b00000001);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char);
		}

		case 0x2e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] <<= 1;
			cpu->memMap[address] |= (cpu->processorStatus & 0b00000001);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char) * 2;
		}

		case 0x3e:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] <<= 1;
			cpu->memMap[address] |= (cpu->processorStatus & 0b00000001);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char) * 2;
		}
	}
}

void ror(struct CPU *cpu){
	unsigned char preShiftVal;
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x6a:{	//accumulator
			preShiftVal = cpu->accumulator;
			cpu->accumulator >>= 1;
			cpu->accumulator |= (cpu->processorStatus << 7);	//accumulator should be being ored by 0bX0000000 with the x being the first bit (carry bit) from processorStatus
			carryFlag(cpu, preShiftVal);	//storing the 7th bit of the accumulator in carry flag from before the accumulator was shifted
			zeroFlag(cpu, cpu->accumulator);
			negativeFlag(cpu, cpu->accumulator);
		}

		case 0x66:{	//zero page
			unsigned char address = *(cpu->programCounter);
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] >>= 1;
			cpu->memMap[address] |= (cpu->processorStatus << 7);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char);
		}

		case 0x76:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] >>= 1;
			cpu->memMap[address] |= (cpu->processorStatus << 7);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char);
		}

		case 0x6e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] >>= 1;
			cpu->memMap[address] |= (cpu->processorStatus << 7);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char) * 2;
		}

		case 0x7e:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			preShiftVal = cpu->memMap[address];
			cpu->memMap[address] >>= 1;
			cpu->memMap[address] |= (cpu->processorStatus << 7);
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, cpu->memMap[address]);
			negativeFlag(cpu, cpu->memMap[address]);
			cpu->programCounter += sizeof(unsigned char) * 2;
		}
	}
}

void rti(struct CPU *cpu){
	cpu->processorStatus = pop(cpu);
	cpu->programCounter = &cpu->memMap[popAbsoluteAddress(cpu)];
}

void rts(struct CPU *cpu){
	cpu->programCounter = &cpu->memMap[popAbsoluteAddress(cpu) - 1];
}

void sbc(struct CPU *cpu){
	return	//still needs to be written
}

void sec(struct CPU *cpu){
	cpu->processorStatus |= 0b00000001;	//turn carryFlag on
}

void sed(struct CPU *cpu){
	cpu->processorStatus |= 0b00001000;	//turn decimal Flag on
}

void sei(struct CPU *cpu){
	cpu->processorStatus |= 0b10000100;	//turn interrupt flag on
}

void sta(struct CPU *cpu){
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x85:{	//zero page
			unsigned char address = *(cpu->programCounter);
			cpu->memMap[address] = cpu->accumulator;
		}

		case 0x95:{	//zero page,X
			unsigned char address = *(cpu->programCounter) + cpu->x;
			cpu->memMap[address] = cpu->accumulator;
		}

		case 0x8d:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			cpu->memMap[address] = cpu->accumulator;
			cpu->programCounter += sizeof(unsigned char);
		}

		case 0x9d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->programCounter) + cpu->x;
			cpu->memMap[address] = cpu->accumulator;
			cpu->programCounter += sizeof(unsigned char);
		}

		case 0x99:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->programCounter) + cpu->y;
			cpu->memMap[address] = cpu->accumulator;
			cpu->programCounter += sizeof(unsigned char);
		}

		case 0x81:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			cpu->memMap[address] = cpu->accumulator;
		}

		case 0x91:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			cpu->memMap[address] = cpu->accumulator;
		}

	}
	cpu->programCounter += sizeof(unsigned char);
}

void stx(struct CPU *cpu){
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x86:{	//zero page
			unsigned char address = *(cpu->programCounter);
			cpu->memMap[address] = cpu->x;
		}

		case 0x96:{	//zero page,Y
			unsigned char address = *(cpu->programCounter) + cpu->x;
			cpu->memMap[address] = cpu->x;
		}

		case 0x8e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			cpu->memMap[address] = cpu->x;
			cpu->programCounter += sizeof(unsigned char);
		}
	}
	cpu->programCounter += sizeof(unsigned char);
}

void sty(struct CPU *cpu){
	switch(*(cpu->programCounter - sizeof(unsigned char))){
		case 0x84:{	//zero page
			unsigned char address = *(cpu->programCounter);
			cpu->memMap[address] = cpu->x;
		}

		case 0x94:{	//zero page,Y
			unsigned char address = *(cpu->programCounter) + cpu->x;
			cpu->memMap[address] = cpu->x;
		}

		case 0x8c:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->programCounter);
			cpu->memMap[address] = cpu->x;
			cpu->programCounter += sizeof(unsigned char);
		}
	}
	cpu->programCounter += sizeof(unsigned char);
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

void tsx(struct CPU *cpu){
	cpu->accumulator = *(cpu->stackPointer);

	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
	
}

void tya(struct CPU *cpu){
	cpu->accumulator = cpu->y;
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
}

void loadInstructions(struct CPU *cpu, char *instructions){
	for(int i = 0; instructions[i] != '\n'; i++){
		(cpu->programCounter[i]) = instructions[i];
	}
}

void cpuLoop(struct CPU *cpu){	//asl still needs to be added to the loop
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
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);	//I do understand that this is equivalent to cpu->programCounter++; because sizeof(unsigned char) == 1 I think this is more explict though
				lda(cpu);
				break;
			}
		
			case 0x29:
			case 0x25:			
			case 0x35:			
			case 0x2d:			
			case 0x3d:			
			case 0x39:			
			case 0x21:			
			case 0x31:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				and(cpu);
				break;
			}			
			
			case 0x0a:
			case 0x06:
			case 0x16:
			case 0x0e:
			case 0x1e:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				asl(cpu);
				break;
			}

			case 0x90:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				bcc(cpu);
				break;	
			}
			
			case 0xb0:{
				cpu->programCounter = cpu->programCounter + sizeof(unsigned char);
				bcs(cpu);
				break;			
			}		

			case 0xea:{	//NOP instruction
				cpu->programCounter += sizeof(unsigned char);
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
	
	struct CPU cpu = {0};

	cpu.programCounter = &(cpu.memMap[0x8000]);
	
	cpu.stackPointer = &(cpu.memMap[0x1ff]);	//stackPointer goes from [0x100-0x1ff] starting at the top and working its way down

	loadInstructions(&cpu, instructions);

	cpuLoop(&cpu);

	printf("accumulator = %x\n", cpu.accumulator);
	printf("x = %x\n", cpu.x);
	printf("processorStatus = %d\n", cpu.processorStatus);
	
}
