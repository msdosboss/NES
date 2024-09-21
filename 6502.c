#include "6502.h"

/*struct CPU{
	unsigned char accumulator, x, y, processorStatus;
	unsigned short PC;
	unsigned char *stackPointer;
	struct Bus bus;

	int extraCycles;
};*/

void nmiInt(struct CPU *cpu){
	unsigned short index = cpu->PC + 1;	//Pushes the PC on to the stack
	unsigned short orcaIndex = index;
	index = index >> 8;
	push(cpu, (unsigned char)(index));
	push(cpu, (unsigned char)(orcaIndex));	//pushes the first 8 bits of the address to stack

	unsigned char flag = cpu->processorStatus;
	flag &= 0b11101111;	//turn off break flag
	flag |= 0b00100000;	//This bit is always pushed as 1 https://www.nesdev.org/wiki/Status_flags

	push(cpu, flag);

	cpu->processorStatus |= 0b00000100;	//turn on int flag

	busTick(&(cpu->bus), 2);

	cpu->PC = absoluteAddress(cpu, 0xfffa);
	
}

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

unsigned short absoluteAddress(struct CPU *cpu, unsigned short startingPoint){	//this is kind of a bad name because it's used in indirect address mode as well
	unsigned short address = busRead(&(cpu->bus), startingPoint + 1);	//putting the 2 arg byte in front because this is little eddien 
	address = address << 8;	//making room for the first arg
	address = address | busRead(&(cpu->bus), startingPoint);	//this line assumes that the bits being shifted in are all zero. I dont know this to be the case
	return address;
}

unsigned short rollOverAbsoluteAddress(struct CPU *cpu, unsigned char startingPoint){
	startingPoint++;
	unsigned short address = busRead(&(cpu->bus), startingPoint);	//putting the 2 arg byte in front because this is little eddien 
	startingPoint--;
	address = address << 8;	//making room for the first arg
	address = address | busRead(&(cpu->bus), startingPoint);	//this line assumes that the bits being shifted in are all zero. I dont know this to be the case
	return address;

}

unsigned short indirectXAddress(struct CPU *cpu){
	unsigned char indirectAddress = busRead(&(cpu->bus), cpu->PC);
	indirectAddress = indirectAddress + cpu->x;
	return rollOverAbsoluteAddress(cpu, indirectAddress);
}

unsigned short indirectYAddress(struct CPU *cpu){
	unsigned char indirectAddress = busRead(&(cpu->bus), cpu->PC);
	//return rollOverAbsoluteAddress(cpu, indirectAddress) + cpu->y;	needed to change this so I could detect page rollovers for extra clock cycles
	return rollOverAbsoluteAddress(cpu, indirectAddress);
}

unsigned short indirectAddress(struct CPU *cpu){
	unsigned short indirectAddress = absoluteAddress(cpu, cpu->PC);
	unsigned char firstByte = (indirectAddress & 0b11111111);
	firstByte++;
	unsigned short address = busRead(&(cpu->bus), ((indirectAddress & 0b1111111100000000) | firstByte));	//this is the only thing that makes it different from just running absoluteAddress(cpu, indirectAddress)
	address = address << 8;
	address = address | busRead(&(cpu->bus), indirectAddress);
	return address;
}

void push(struct CPU *cpu, unsigned char val){
	*cpu->stackPointer = val;
	cpu->stackPointer = cpu->stackPointer - sizeof(unsigned char);
}

unsigned char pop(struct CPU *cpu){
	cpu->stackPointer = cpu->stackPointer + sizeof(unsigned char);
	return *(cpu->stackPointer);
}

unsigned short popAbsoluteAddress(struct CPU *cpu){
	unsigned short address = 0;
	address |= pop(cpu);
	address |= (((unsigned short)pop(cpu)) << 8);	//this should take what is returned from pop and store it in the last 8 bits of address
	return address;
}

int isPageCrossed(unsigned short baseAddress, unsigned char reg){
	if(((baseAddress + reg) & 0xff) <= (baseAddress & 0xff)){	//testing if page cross
		return 1;
	}
	return 0;
}

void adc(struct CPU *cpu){
	unsigned short result;
	unsigned char val;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x69:{	//immediate
			val = busRead(&(cpu->bus), cpu->PC);
			break;
		}
		
		case 0x65:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			val = busRead(&(cpu->bus), address);
			break;
		}

		case 0x75:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			val = busRead(&(cpu->bus), address);
			break;
		}
		
		case 0x6d:{	//absolute
			val = busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC));
			cpu->PC++;
			break;
		}

		case 0x7d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x);
			val = busRead(&(cpu->bus), address + cpu->x);
			cpu->PC++;
			break;
		}
		
		case 0x79:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			val = busRead(&(cpu->bus), address + cpu->y);
			cpu->PC++;
			break;
		}
		
		case 0x61:{	//indirect,X
			val = busRead(&(cpu->bus), indirectXAddress(cpu));
			break;
		}

		case 0x71:{
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			val = busRead(&(cpu->bus), address + cpu->y);
			break;
		}

	}
	
	result = cpu->accumulator + val + (cpu->processorStatus & 0b00000001);
	
	if(result > 0xff){	//Set carry flag
		cpu->processorStatus |= 0b00000001;
	}
	else{
		cpu->processorStatus &= 0b11111110;
	}
	
	if(((cpu->accumulator ^ val) & 0b10000000) == 0 && ((cpu->accumulator ^ result) & 0b10000000) != 0){	//set overflow flag
		cpu->processorStatus |= 0b01000000;
	}
	else{
		cpu->processorStatus &= 0b10111111;
	}
	
	cpu->accumulator = result & 0xff;

	zeroFlag(cpu, cpu->accumulator);
	negativeFlag(cpu, cpu->accumulator);
	cpu->PC++;
}

void ahx(struct CPU *cpu){	//unofficial instruction
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	cpu->x &= cpu->accumulator;
	cpu->x &= 7;	//I could be misunderstanding this https://www.nesdev.org/undocumented_opcodes.txt check AXA
	switch(opCode){
		case 0x9f:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			busWrite(&(cpu->bus), address, cpu->x);
			cpu->PC++;
			break;
		}
		case 0x93:{	//indirect,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			busWrite(&(cpu->bus), address, cpu->x);
			break;
		}
	}
	cpu->PC++;
}

void alr(struct CPU *cpu){	//unofficial instruction
	unsigned char arg = busRead(&(cpu->bus), cpu->PC);
	cpu->accumulator &= arg;
	if((cpu->accumulator & 0b00000001) != 0){	//turns carry bit on
		cpu->processorStatus |= 0b00000001;
	}
	else{	//turns carry bit off
		cpu->processorStatus &= 0b11111110;	
	}
	cpu->accumulator >>= 1;

	zeroFlag(cpu, cpu->accumulator);
	negativeFlag(cpu, cpu->accumulator);
	cpu->PC++;	
}

void anc(struct CPU *cpu){	//unofficial instruction
	unsigned char arg = busRead(&(cpu->bus), cpu->PC);
	cpu->accumulator &= arg;
	
	negativeFlag(cpu, cpu->accumulator);
	zeroFlag(cpu, cpu->accumulator);
	carryFlag(cpu, cpu->accumulator);

	cpu->PC++;
}

void and(struct CPU *cpu){
	unsigned char arg;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x29:{	//immediate
			arg = busRead(&(cpu->bus), cpu->PC);
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}
		
		case 0x25:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address );
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}

		case 0x35:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}
		case 0x2d:{	//absolute
			arg = busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC));
			cpu->accumulator = cpu->accumulator & arg;
			cpu->PC++;
			break;
		}
		
		case 0x3d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x); 
			arg = busRead(&(cpu->bus), address + cpu->x);
			cpu->accumulator = cpu->accumulator & arg;
			cpu->PC++;
			break;
		}
		
		case 0x39:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y); 
			arg = busRead(&(cpu->bus), address + cpu->y);
			cpu->accumulator = cpu->accumulator & arg;
			cpu->PC++;
			break;
		}
		case 0x21:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator & arg;
			break;
		}
		case 0x31:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator & arg;
			break;
		
		}
	}
	cpu->PC++;
	
	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);
}

void arr(struct CPU *cpu){	//unofficial instruction
	unsigned char arg = busRead(&(cpu->bus), cpu->PC);
	cpu->accumulator &= arg;
	cpu->accumulator >>= 1;
	cpu->accumulator |= (cpu->processorStatus << 7);
	
	if((cpu->accumulator & 0b01000000) != 0){
		cpu->processorStatus |= 0b00000001;	//turn on carry bit
	}
	else{
		cpu->processorStatus &= 0b11111110;	//turn off carry bit
	}

	if(((cpu->accumulator & 0b01000000) != 0) ^ ((cpu->accumulator & 0b00100000) != 0)){
		cpu->processorStatus |= 0b01000000;	//turn on overflow bit
	}
	else{
		cpu->processorStatus &= 0b10111111;	//turn off overflow bit
	}

	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);

	cpu->PC++;
}

void asl(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){	//note with these shifts I am still assuming that when you shift a bit in from the left it will always be 0, if that is not the case this will not work
		case 0x0a:{	//accumulator
			carryFlag(cpu, cpu->accumulator);
			cpu->accumulator = cpu->accumulator << 1;
			negativeFlag(cpu, cpu->accumulator);
			zeroFlag(cpu, cpu->accumulator);
			break;
		}

		case 0x06:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}
		
		case 0x16:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}

		case 0x0e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}

		case 0x1e:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			address = address + cpu->x; 
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}
	}
}

void axs(struct CPU *cpu){
	unsigned char arg = busRead(&(cpu->bus), cpu->PC);
	cpu->x &= cpu->accumulator;
	cpu->x -= arg;

	negativeFlag(cpu, cpu->x);
	carryFlag(cpu, cpu->x);
	zeroFlag(cpu, cpu->x);

	cpu->PC++;
}

void bcc(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000001) == 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bcs(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000001) != 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void beq(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000010) != 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bit(struct CPU *cpu){
	unsigned char result;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x24:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			result = busRead(&(cpu->bus), address) & cpu->accumulator;
			overFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&cpu->bus, address));
			break;
		}

		case 0x2c:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			result = busRead(&(cpu->bus), address) & cpu->accumulator;
			overFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}
	}
	cpu->PC++;
	zeroFlag(cpu, result);
}

void bmi(struct CPU *cpu){
	if((cpu->processorStatus & 0b10000000) != 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bne(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000010) == 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bpl(struct CPU *cpu){
	if((cpu->processorStatus & 0b10000000) == 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bvc(struct CPU *cpu){
	if((cpu->processorStatus & 0b01000000) == 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bvs(struct CPU *cpu){
	if((cpu->processorStatus & 0b01000000) != 0){
		cpu->extraCycles = 1;
		cpu->extraCycles += isPageCrossed(cpu->PC, (char)(busRead(&(cpu->bus), cpu->PC)));	//this might not work becuase the 2nd arg is suppose to be an unsigned char
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
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
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xc9:{	//immediate
			result = cpu->accumulator - busRead(&(cpu->bus), cpu->PC);
			if(cpu->accumulator >= busRead(&(cpu->bus), cpu->PC)){	//I could do this more cleverly with carryFlag(cpu, (cpu->accumulator >= *cpu->programCounter) * 0b10000000) but I think that is harder to read so I prefer this
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), cpu->PC)){	//I could do this more cleverly with zeroFlag(cpu, !(cpu->accumulator == *(cpu->programCounter))); but I think that is harder to read so I prefer this
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xc5:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xd5:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xcd:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			cpu->PC++;
			break;
		}

		case 0xdd:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x);
			address += cpu->x; 
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			cpu->PC++;
			break;
		}

		case 0xd9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			cpu->PC++;
			break;
		}

		case 0xc1:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xd1:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}
	}
	cpu->PC++;
	negativeFlag(cpu, result);
}

void cpx(struct CPU *cpu){
	unsigned char result;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xe0:{	//immediate
			result = cpu->x - busRead(&(cpu->bus), cpu->PC);
			if(cpu->x >= busRead(&(cpu->bus), cpu->PC)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->x == busRead(&(cpu->bus), cpu->PC)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);
			}
			break;
		}

		case 0xe4:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			result = cpu->x - busRead(&(cpu->bus), address);
			if(cpu->x >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->x == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);
			}
			break;
		}

		case 0xec:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			result = cpu->x - busRead(&(cpu->bus), address);
			if(cpu->x >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->x == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);
			}
			cpu->PC++;
			break;
		}

	}
	cpu->PC++;
	negativeFlag(cpu, result);
}

void cpy(struct CPU *cpu){
	unsigned char result;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xc0:{	//immediate
			result = cpu->y - busRead(&(cpu->bus), cpu->PC);
			if(cpu->y >= busRead(&(cpu->bus), cpu->PC)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->y == busRead(&(cpu->bus), cpu->PC)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);
			}
			break;
		}

		case 0xc4:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			result = cpu->y - busRead(&(cpu->bus), address);
			if(cpu->y >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->y == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);
			}
			break;
		}

		case 0xcc:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			result = cpu->y - busRead(&(cpu->bus), address);
			if(cpu->y >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->y == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);
			}
			cpu->PC++;
			break;
		}

	}
	cpu->PC++;
	negativeFlag(cpu, result);
}

void dcp(struct CPU *cpu){	//unofficial instruction
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	unsigned char arg;
	unsigned char result;
	switch(opCode){
		case 0xc7:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, arg - 1);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xd7:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, arg - 1);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xcf:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, arg - 1);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			cpu->PC++;
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xdf:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, arg - 1);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			cpu->PC++;
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xdb:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, arg - 1);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			cpu->PC++;
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}

		case 0xc3:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, arg - 1);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;

		}

		case 0xd3:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu) + cpu->y;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, arg - 1);
			result = cpu->accumulator - busRead(&(cpu->bus), address);
			if(cpu->accumulator >= busRead(&(cpu->bus), address)){
				carryFlag(cpu, 0b10000000);	//set carry flag on
			}
			else{
				carryFlag(cpu, 0);
			}
			if(cpu->accumulator == busRead(&(cpu->bus), address)){
				zeroFlag(cpu, 0);	//set zero flag on
			}
			else{
				zeroFlag(cpu, 1);	
			}
			break;
		}	
	}
	negativeFlag(cpu, result);

	cpu->PC++;
}

void dec(struct CPU *cpu){
	unsigned char result;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xc6:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) - 1);
			result = busRead(&(cpu->bus), address);
			break;
		}

		case 0xd6:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) - 1);
			result = busRead(&(cpu->bus), address);
			break;
		}

		case 0xce:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) - 1);
			result = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0xde:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) - 1);
			result = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}
	}
	cpu->PC++;
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

void dop(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x80:
		case 0x82:
		case 0x89:
		case 0xe2:
		case 0xc2:{	//immediate
			break;
		}

		case 0x04:
		case 0x44:
		case 0x64:{	//zero page
			break;
		}

		case 0x14:
		case 0x34:
		case 0x54:
		case 0x74:
		case 0xd4:
		case 0xf4:{	//zero page,X
			break;
		}
	}
	cpu->PC++;
}

void eor(struct CPU *cpu){
	unsigned char arg;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x49:{	//immediate
			arg = busRead(&(cpu->bus), cpu->PC);
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}
		
		case 0x45:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}

		case 0x55:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}
		case 0x4d:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			cpu->PC++;
			break;
		}
		
		case 0x5d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x);
			address += cpu->x; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			cpu->PC++;
			break;
		}
		
		case 0x59:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			cpu->PC++;
			break;
		}
		case 0x41:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		}
		case 0x51:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			break;
		
		}
	}
	cpu->PC++;
	
	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);
}

void inc(struct CPU *cpu){
	unsigned char result;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xe6:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) + 1);
			result = busRead(&(cpu->bus), address);
			break;
		}

		case 0xf6:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) + 1);
			result = busRead(&(cpu->bus), address);
			break;
		}

		case 0xee:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) + 1);
			result = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0xfe:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) + 1);
			result = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}
	}
	cpu->PC++;
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

void isb(struct CPU *cpu){
	unsigned short result;
	unsigned char arg;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xe7:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, ++arg);
			break;
		}
		case 0xf7:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, ++arg);
			break;
		}
		case 0xef:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, ++arg);
			cpu->PC++;
			break;
		}
		case 0xff:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, ++arg);
			cpu->PC++;
			break;	
		}
		case 0xfb:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, ++arg);
			cpu->PC++;
			break;
		}
		case 0xe3:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, ++arg);
			break;
		}
		case 0xf3:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu) + cpu->y;
			arg = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, ++arg);
			break;
		}
	}
	result = cpu->accumulator - arg - !(cpu->processorStatus & 0b00000001);
	if(result <= 0xff){	//Set carry flag
		cpu->processorStatus |= 0b00000001;
	}
	else{
		cpu->processorStatus &= 0b11111110;
	}
	
	if(((cpu->accumulator ^ arg) & 0b10000000) != 0 && ((cpu->accumulator ^ result) & 0b10000000) != 0){	//set overflow flag
		cpu->processorStatus |= 0b01000000;
	}
	else{
		cpu->processorStatus &= 0b10111111;
	}
	
	cpu->accumulator = result & 0xff;

	zeroFlag(cpu, cpu->accumulator);
	negativeFlag(cpu, cpu->accumulator);
	cpu->PC++;
}

void jmp(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x4c:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->PC = address;
			break;
		}
		case 0x6c:{	//indirect
			unsigned short address = indirectAddress(cpu);
			cpu->PC = address; 
			break;
		}
	}
}

void jsr(struct CPU *cpu){
	unsigned short index = cpu->PC + 1;	//I really dont know why the + 1 is here but it works so yeah...
	unsigned short orcaIndex = index;
	index = index >> 8;
	push(cpu, (unsigned char)(index));
	push(cpu, (unsigned char)(orcaIndex));	//pushes the first 8 bits of the address to stack
	unsigned short address = absoluteAddress(cpu, cpu->PC);
	cpu->PC = address;
}

void las(struct CPU *cpu){	//unofficial instruction
	unsigned char result;
	unsigned short address = absoluteAddress(cpu, cpu->PC);	//don't ask me why this is the only address mode for this instruction
	cpu->extraCycles = isPageCrossed(address, cpu->y);
	address += cpu->y;
	
	result = ((cpu->stackPointer - cpu->bus.prgRam) - 0x100) & busRead(&(cpu->bus), address);
	
	cpu->accumulator = result;
	cpu->x = result;
	cpu->stackPointer = result + cpu->bus.prgRam + 0x100;
	
	cpu->PC += 2;
}

void lax(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xa7:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			cpu->accumulator = busRead(&(cpu->bus), address);
			cpu->x = busRead(&(cpu->bus), address);
			break;
		}

		case 0xb7:{	//zero page,Y
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->y;
			cpu->accumulator = busRead(&(cpu->bus), address);
			cpu->x = busRead(&(cpu->bus), address);
			break;
		}

		case 0xaf:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->accumulator = busRead(&(cpu->bus), address);
			cpu->x = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0xbf:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y;
			cpu->accumulator = busRead(&(cpu->bus), address);
			cpu->x = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0xa3:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			cpu->accumulator = busRead(&(cpu->bus), address);
			cpu->x = busRead(&(cpu->bus), address);
			break;
		}

		case 0xb3:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y;
			cpu->accumulator = busRead(&(cpu->bus), address);
			cpu->x = busRead(&(cpu->bus), address);
			break;
		}
	}

	zeroFlag(cpu, cpu->accumulator);
	negativeFlag(cpu, cpu->accumulator);

	cpu->PC++;
}

void lda(struct CPU *cpu){
	unsigned char arg;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xa9:{	//immediate
			arg = busRead(&(cpu->bus), cpu->PC);
			cpu->accumulator = arg;
			break;
		}		

		case 0xa5:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			break;
		}
		
		case 0xb5:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			break;
		}
		
		case 0xad:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			cpu->PC++;
			break;
		}

		case 0xbd:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x);
			address += cpu->x; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			cpu->PC++;
			break;
		}

		case 0xb9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			cpu->PC++;
			break;
		}
		
		case 0xa1:{	//(indirect,X)
			unsigned short address = indirectXAddress(cpu);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			break;
		}

		case 0xb1:{	//(indirect,Y)
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			break;
		}

		default:{
			printf("instruction %x is not part of LDA\n", cpu->PC - 1);
			break;
		}
			
	}
	cpu->PC++;
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);	
}

void ldx(struct CPU *cpu){
	unsigned char arg;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xa2:{	//immediate
			arg = busRead(&(cpu->bus), cpu->PC);
			cpu->x = arg;
			break;
		}		

		case 0xa6:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->x = arg;
			break;
		}
		
		case 0xb6:{	//zero page,Y
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->y;
			arg = busRead(&(cpu->bus), address);
			cpu->x = arg;
			break;
		}
		
		case 0xae:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->x = arg;
			cpu->PC++;
			break;
		}

		case 0xbe:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			arg = busRead(&(cpu->bus), address);
			cpu->x = arg;
			cpu->PC++;
			break;
		}	

		default:{
			printf("instruction %x is not part of LDX\n", busRead(&(cpu->bus), cpu->PC));
			break;
		}
			
	}
	cpu->PC++;
	
	zeroFlag(cpu, cpu->x);
	
	negativeFlag(cpu, cpu->x);	
}

void ldy(struct CPU *cpu){
	unsigned char arg;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0xa0:{	//immediate
			arg = busRead(&(cpu->bus), cpu->PC);
			cpu->y = arg;
			break;
		}		

		case 0xa4:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->y = arg;
			break;
		}
		
		case 0xb4:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->y = arg;
			break;
		}
		
		case 0xac:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->y = arg;
			cpu->PC++;
			break;
		}

		case 0xbc:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x);
			address += cpu->x; 
			arg = busRead(&(cpu->bus), address);
			cpu->y = arg;
			cpu->PC++;
			break;
		}	

		default:{
			printf("instruction %x is not part of LDY\n", busRead(&(cpu->bus), cpu->PC));
			break;
		}
			
	}
	cpu->PC++;
	
	zeroFlag(cpu, cpu->y);
	
	negativeFlag(cpu, cpu->y);	
}

void lsr(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x4a:{	//accumulator
			carryFlag(cpu, (cpu->accumulator << 7));	//The carryFlag func checks if the last bit is on and then sets the flag accordingly. In this use of it we are trying to see if the bit that is being shifted out (0th bit) is on.
			cpu->accumulator = cpu->accumulator >> 1;
			zeroFlag(cpu, cpu->accumulator);
			negativeFlag(cpu, cpu->accumulator);
			break;
		}
		
		case 0x46:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}

		case 0x56:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}

		case 0x4e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}

		case 0x5e:{	//absoult,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}
	}
}

void lxa(struct CPU *cpu){	//unofficial instruction
	unsigned char arg = busRead(&(cpu->bus), cpu->PC);
	cpu->accumulator &= arg;
	cpu->x = cpu->accumulator;

	negativeFlag(cpu, cpu->x);
	zeroFlag(cpu, cpu->x);

	cpu->PC++;
}

void ora(struct CPU *cpu){
	unsigned char arg;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x09:{	//immediate
			arg = busRead(&(cpu->bus), cpu->PC);
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}
		
		case 0x05:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}

		case 0x15:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}
		case 0x0d:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			cpu->PC++;	//note absolute addressing take 3 bytes so the program counter will have to be interated twice for the absolute calls
			break;
		}
		
		case 0x1d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x);
			address += cpu->x; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			cpu->PC++;
			break;
		}
		
		case 0x19:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			cpu->PC++;
			break;
		}
		case 0x01:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			break;
		}
		case 0x11:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			break;
		
		}
	}
	cpu->PC++;
	
	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);
}

void pha(struct CPU *cpu){
	push(cpu, cpu->accumulator);
}

void php(struct CPU *cpu){
	push(cpu, (cpu->processorStatus | 0b00010000));	//break bit is supposed to be on when pushed
}

void pla(struct CPU *cpu){
	cpu->accumulator = pop(cpu);

	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
}

void plp(struct CPU *cpu){
	cpu->processorStatus = ((pop(cpu) & 0b11101111) | 0b00100000);
}

void rla(struct CPU *cpu){	//unofficial instruction
	unsigned char preShiftVal;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x27:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			cpu->accumulator &= busRead(&(cpu->bus), address);
			break;
		}

		case 0x37:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			cpu->accumulator &= busRead(&(cpu->bus), address);
			break;

		}

		case 0x2f:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			cpu->accumulator &= busRead(&(cpu->bus), address);
			cpu->PC++;
			break;

		}

		case 0x3f:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			cpu->accumulator &= busRead(&(cpu->bus), address);
			cpu->PC++;
			break;

		}

		case 0x3b:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			cpu->accumulator &= busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0x23:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			cpu->accumulator &= busRead(&(cpu->bus), address);
			break;
		}

		case 0x33:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu) + cpu->y;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			cpu->accumulator &= busRead(&(cpu->bus), address);
			break;
		}
	}
	zeroFlag(cpu, cpu->accumulator);	
	negativeFlag(cpu, cpu->accumulator);	

	cpu->PC++;
}

void rol(struct CPU *cpu){
	unsigned char preShiftVal;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x2a:{	//accumulator
			preShiftVal = cpu->accumulator;
			cpu->accumulator <<= 1;
			cpu->accumulator |= (cpu->processorStatus & 0b00000001);	//accumulator should be being ored by 0b0000000X with the x being the first bit (carry bit) from processorStatus
			carryFlag(cpu, preShiftVal);	//storing the 7th bit of the accumulator in carry flag from before the accumulator was shifted
			zeroFlag(cpu, cpu->accumulator);
			negativeFlag(cpu, cpu->accumulator);
			break;
		}

		case 0x26:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}

		case 0x36:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}

		case 0x2e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}

		case 0x3e:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal << 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus & 0b00000001));
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}
	}
}

void ror(struct CPU *cpu){
	unsigned char preShiftVal;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x6a:{	//accumulator
			preShiftVal = cpu->accumulator;
			cpu->accumulator >>= 1;
			cpu->accumulator |= (cpu->processorStatus << 7);	//accumulator should be being ored by 0bX0000000 with the x being the first bit (carry bit) from processorStatus
			carryFlag(cpu, preShiftVal << 7);	//storing the 0th bit of the accumulator in carry flag from before the accumulator was shifted
			zeroFlag(cpu, cpu->accumulator);
			negativeFlag(cpu, cpu->accumulator);
			break;
		}

		case 0x66:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			carryFlag(cpu, preShiftVal << 7);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}

		case 0x76:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			carryFlag(cpu, preShiftVal << 7);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC++;
			break;
		}

		case 0x6e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			carryFlag(cpu, preShiftVal << 7);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}

		case 0x7e:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			carryFlag(cpu, preShiftVal << 7);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}
	}
}

void rra(struct CPU *cpu){	//unofficial instruction
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	unsigned char preShiftVal;
	unsigned short result;
	unsigned char val;
	switch(opCode){
		case 0x67:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			val = busRead(&(cpu->bus), address);
			carryFlag(cpu, preShiftVal << 7);
			break;
		}

		case 0x77:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			val = busRead(&(cpu->bus), address);
			carryFlag(cpu, preShiftVal << 7);
			break;
		}

		case 0x6f:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			val = busRead(&(cpu->bus), address);
			carryFlag(cpu, preShiftVal << 7);
			cpu->PC++;
			break;
		}

		case 0x7f:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			val = busRead(&(cpu->bus), address);
			carryFlag(cpu, preShiftVal << 7);
			cpu->PC++;
			break;
		}

		case 0x7b:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			val = busRead(&(cpu->bus), address);
			carryFlag(cpu, preShiftVal << 7);
			cpu->PC++;
			break;
		}

		case 0x63:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			val = busRead(&(cpu->bus), address);
			carryFlag(cpu, preShiftVal << 7);
			break;
		}

		case 0x73:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu) + cpu->y;
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			val = busRead(&(cpu->bus), address);
			carryFlag(cpu, preShiftVal << 7);
			break;
		}
	}
	result = cpu->accumulator + val + (cpu->processorStatus & 0b00000001);
	
	if(result > 0xff){	//Set carry flag
		cpu->processorStatus |= 0b00000001;
	}
	else{
		cpu->processorStatus &= 0b11111110;
	}
	
	if(((cpu->accumulator ^ val) & 0b10000000) == 0 && ((cpu->accumulator ^ result) & 0b10000000) != 0){	//set overflow flag
		cpu->processorStatus |= 0b01000000;
	}
	else{
		cpu->processorStatus &= 0b10111111;
	}
	
	cpu->accumulator = result & 0xff;

	zeroFlag(cpu, cpu->accumulator);
	negativeFlag(cpu, cpu->accumulator);

	cpu->PC++;
}

void rti(struct CPU *cpu){
	cpu->processorStatus = ((pop(cpu) & 0b11101111) | 0b00100000);
	cpu->PC = popAbsoluteAddress(cpu) + 1;
}

void rts(struct CPU *cpu){
	cpu->PC = popAbsoluteAddress(cpu) + 1;
}

void sax(struct CPU *cpu){	//unofficial instruction
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x87:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			busWrite(&(cpu->bus), address, (cpu->accumulator & cpu->x));
			break;
		}

		case 0x97:{	//zero page,Y
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->y;
			busWrite(&(cpu->bus), address, (cpu->accumulator & cpu->x));
			break;
		}

		case 0x8f:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			busWrite(&(cpu->bus), address, (cpu->accumulator & cpu->x));
			cpu->PC++;
			break;
		}

		case 0x83:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			busWrite(&(cpu->bus), address, (cpu->accumulator & cpu->x));
			break;
		}
	}
	cpu->PC++;
}

void sbc(struct CPU *cpu){
	unsigned short result;
	unsigned char val;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);	
	switch(opCode){
		case 0xeb:	//unofficial sbc
		case 0xe9:{	//immediate
			val = busRead(&(cpu->bus), cpu->PC);
			break;
		}
		
		case 0xe5:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			val = busRead(&(cpu->bus), address);
			break;
		}

		case 0xf5:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			val = busRead(&(cpu->bus), address);
			break;
		}
		
		case 0xed:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			val = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0xfd:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->x);
			address += cpu->x;
			val = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}
		
		case 0xf9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			val = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}
		
		case 0xe1:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			val = busRead(&(cpu->bus), address);
			break;
		}

		case 0xf1:{
			unsigned short address = indirectYAddress(cpu);
			cpu->extraCycles = isPageCrossed(address, cpu->y);
			address += cpu->y; 
			val = busRead(&(cpu->bus), address);
			break;
		}

	}
	
	result = cpu->accumulator - val - !(cpu->processorStatus & 0b00000001);
	if(result <= 0xff){	//Set carry flag
		cpu->processorStatus |= 0b00000001;
	}
	else{
		cpu->processorStatus &= 0b11111110;
	}
	
	if(((cpu->accumulator ^ val) & 0b10000000) != 0 && ((cpu->accumulator ^ result) & 0b10000000) != 0){	//set overflow flag
		cpu->processorStatus |= 0b01000000;
	}
	else{
		cpu->processorStatus &= 0b10111111;
	}
	
	cpu->accumulator = result & 0xff;

	zeroFlag(cpu, cpu->accumulator);
	negativeFlag(cpu, cpu->accumulator);
	cpu->PC++;
}

void sec(struct CPU *cpu){
	cpu->processorStatus |= 0b00000001;	//turn carryFlag on
}

void sed(struct CPU *cpu){
	cpu->processorStatus |= 0b00001000;	//turn decimal Flag on
}

void sei(struct CPU *cpu){
	cpu->processorStatus |= 0b00000100;	//turn interrupt flag on
}

void slo(struct CPU *cpu){	//unofficial instruction
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x07:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			cpu->accumulator |= busRead(&(cpu->bus), address);	
			break;
		}

		case 0x17:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			cpu->accumulator |= busRead(&(cpu->bus), address);
			break;
		}

		case 0x0f:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			cpu->accumulator |= busRead(&(cpu->bus), address);
			cpu->PC++;	
			break;
		}

		case 0x1f:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			cpu->accumulator |= busRead(&(cpu->bus), address);
			cpu->PC++;	
			break;
		}

		case 0x1b:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			cpu->accumulator |= busRead(&(cpu->bus), address);
			cpu->PC++;	
			break;
		}

		case 0x03:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			cpu->accumulator |= busRead(&(cpu->bus), address);
			break;
		}

		case 0x13:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu) + cpu->y;
			carryFlag(cpu, busRead(&(cpu->bus), address));
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) << 1);
			cpu->accumulator |= busRead(&(cpu->bus), address);
			break;
		}
	}
	negativeFlag(cpu, cpu->accumulator);
	zeroFlag(cpu, cpu->accumulator);

	cpu->PC++;
}

void sre(struct CPU *cpu){	//unofficial instruction
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x47:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			cpu->accumulator ^= busRead(&(cpu->bus), address);
			break;
		}

		case 0x57:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			cpu->accumulator ^= busRead(&(cpu->bus), address);
			break;
		}

		case 0x4f:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			cpu->accumulator ^= busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0x5f:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			cpu->accumulator ^= busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0x5b:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			cpu->accumulator ^= busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}

		case 0x43:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			cpu->accumulator ^= busRead(&(cpu->bus), address);
			break;
		}

		case 0x53:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu) + cpu->y;
			carryFlag(cpu, busRead(&(cpu->bus), address) << 7);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) >> 1);
			cpu->accumulator ^= busRead(&(cpu->bus), address);
			break;
		}

	}
	negativeFlag(cpu, cpu->accumulator);
	zeroFlag(cpu, cpu->accumulator);

	cpu->PC++;
}

void sta(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x85:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			busWrite(&(cpu->bus), address, cpu->accumulator);
			break;
		}

		case 0x95:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			busWrite(&(cpu->bus), address, cpu->accumulator);
			break;
		}

		case 0x8d:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			busWrite(&(cpu->bus), address, cpu->accumulator);
			cpu->PC++;
			break;
		}

		case 0x9d:{	//absolute,X
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			busWrite(&(cpu->bus), address, cpu->accumulator);
			cpu->PC++;
			break;
		}

		case 0x99:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
			busWrite(&(cpu->bus), address, cpu->accumulator);
			cpu->PC++;
			break;
		}

		case 0x81:{	//indirect,X
			unsigned short address = indirectXAddress(cpu);
			busWrite(&(cpu->bus), address, cpu->accumulator);
			break;
		}

		case 0x91:{	//indirect,Y
			unsigned short address = indirectYAddress(cpu) + cpu->y;
			busWrite(&(cpu->bus), address, cpu->accumulator);
			break;
		}

	}
	cpu->PC++;
}

void stx(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x86:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			busWrite(&(cpu->bus), address, cpu->x);
			break;
		}

		case 0x96:{	//zero page,Y
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->y;
			busWrite(&(cpu->bus), address, cpu->x);
			break;
		}

		case 0x8e:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			busWrite(&(cpu->bus), address, cpu->x);
			cpu->PC++;
			break;
		}
	}
	cpu->PC++;
}

void sty(struct CPU *cpu){
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);
	switch(opCode){
		case 0x84:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			busWrite(&(cpu->bus), address, cpu->y);
			break;
		}

		case 0x94:{	//zero page,X
			unsigned char address = busRead(&(cpu->bus), cpu->PC) + cpu->x;
			busWrite(&(cpu->bus), address, cpu->y);
			break;
		}

		case 0x8c:{	//absolute
			unsigned short address = absoluteAddress(cpu, cpu->PC);
			busWrite(&(cpu->bus), address, cpu->y);
			cpu->PC++;
			break;
		}
	}
	cpu->PC++;
}

void sxa(struct CPU *cpu){	//unofficial instruction
	unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
	busWrite(&(cpu->bus), address, cpu->x & ((address >> 8) + 1));
	cpu->PC += 2;	
}

void sya(struct CPU *cpu){	//unofficial instruction
	unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
	busWrite(&(cpu->bus), address, cpu->y & ((address >> 8) + 1));
	cpu->PC += 2;	
}

void tas(struct CPU *cpu){	//unofficial instruction
	unsigned char result = cpu->x & cpu->accumulator;
	cpu->stackPointer = result + &(cpu->bus.prgRam[0x100]);
	unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
	busWrite(&(cpu->bus), address, (result & ((address >> 8) + 1)));
	cpu->PC += 2;
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
	cpu->x = (cpu->stackPointer - cpu->bus.prgRam) - 0x100;

	zeroFlag(cpu, cpu->x);
	
	negativeFlag(cpu, cpu->x);
	
}

void txs(struct CPU *cpu){
	cpu->stackPointer = cpu->x + &(cpu->bus.prgRam[0x100]);
}

void tya(struct CPU *cpu){
	cpu->accumulator = cpu->y;
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
}

void xxa(struct CPU *cpu){	//unofficial instruction
	cpu->accumulator = cpu->x;
	cpu->accumulator &= busRead(&(cpu->bus), cpu->PC);
	cpu->PC++;
}

void loadInstructions(struct CPU *cpu, char *instructions, int instructionsLen){
	for(int i = cpu->PC; i < instructionsLen + cpu->PC; i++){
		busWrite(&(cpu->bus), i, instructions[i - cpu->PC]);
	}
}

void cpuLoop(struct CPU *cpu){
	if(cpu->bus.ppu->nmiInt){
		nmiInt(cpu);
	}
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC);
	cpu->PC++;
	switch(opCode){
		case 0x69:
		case 0x65:
		case 0x75:
		case 0x6d:
		case 0x7d:
		case 0x79:
		case 0x61:
		case 0x71:{
			adc(cpu);
			break;
		}

		case 0x0b:
		case 0x2b:{
			anc(cpu);
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
			and(cpu);
			break;
		}			

		case 0x6b:{
			arr(cpu);
			break;
		}

		case 0x0a:
		case 0x06:
		case 0x16:
		case 0x0e:
		case 0x1e:{
			asl(cpu);
			break;
		}

		case 0xcb:{
			axs(cpu);
			break;
		}

		case 0x90:{
			bcc(cpu);
			break;	
		}
		
		case 0xb0:{
			bcs(cpu);
			break;			
		}

		case 0xf0:{
			beq(cpu);
			break;
		}		

		case 0x24:
		case 0x2c:{
			bit(cpu);
			break;
		}

		case 0x30:{
			bmi(cpu);
			break;
		}

		case 0xd0:{
			bne(cpu);
			break;
		}

		case 0x10:{
			bpl(cpu);
			break;
		}

		case 0x00:{	//brk instruction
			cpu->processorStatus |= 0b00010000;	//turn break flag on
			break;
		}

		case 0x50:{
			bvc(cpu);
			break;
		}

		case 0x70:{
			bvs(cpu);
			break;
		}

		case 0x18:{
			clc(cpu);
			break;
		}

		case 0xd8:{
			cld(cpu);
			break;
		}

		case 0x58:{
			cli(cpu);
			break;
		}

		case 0xb8:{
			clv(cpu);
			break;
		}

		case 0xc9:
		case 0xc5:
		case 0xd5:
		case 0xcd:
		case 0xdd:
		case 0xd9:
		case 0xc1:
		case 0xd1:{
			cmp(cpu);
			break;
		}

		case 0xe0:
		case 0xe4:
		case 0xec:{
			cpx(cpu);
			break;
		}

		case 0xc0:
		case 0xc4:
		case 0xcc:{
			cpy(cpu);
			break;
		}

		case 0xc7:
		case 0xd7:
		case 0xcf:
		case 0xdf:
		case 0xdb:
		case 0xc3:
		case 0xd3:{
			dcp(cpu);
			break;
		}

		case 0xc6:
		case 0xd6:
		case 0xce:
		case 0xde:{
			dec(cpu);
			break;
		}

		case 0xca:{
			dex(cpu);
			break;
		}

		case 0x88:{
			dey(cpu);
			break;
		}

		case 0x04:
		case 0x14:
		case 0x34:
		case 0x44:
		case 0x54:
		case 0x74:
		case 0x80:
		case 0x82:
		case 0x89:
		case 0xc2:
		case 0xd4:
		case 0xe2:
		case 0xf4:
		case 0x64:{
			dop(cpu);
			break;
		}

		case 0x49:
		case 0x45:
		case 0x55:
		case 0x4d:
		case 0x5d:
		case 0x59:
		case 0x41:
		case 0x51:{
			eor(cpu);
			break;
		}

		case 0xe6:
		case 0xf6:
		case 0xee:
		case 0xfe:{
			inc(cpu);
			break;
		}

		case 0xe8:{
			inx(cpu);
			break;

		}

		case 0xc8:{
			iny(cpu);
			break;

		}

		case 0xe7:
		case 0xf7:
		case 0xef:
		case 0xff:
		case 0xfb:
		case 0xe3:
		case 0xf3:{
			isb(cpu);
			break;
		}

		case 0x4c:
		case 0x6c:{
			jmp(cpu);
			break;

		}

		case 0x20:{
			jsr(cpu);
			break;
		}

		case 0xbb:{
			las(cpu);
			break;
		}

		case 0xa7:
		case 0xb7:
		case 0xaf:
		case 0xbf:
		case 0xa3:
		case 0xb3:{
			lax(cpu);
			break;
		}

		case 0xa9:
		case 0xa5:
		case 0xb5:
		case 0xad:
		case 0xbd:
		case 0xb9:
		case 0xa1:
		case 0xb1:{
			lda(cpu);
			break;
		}


		case 0xa2:
		case 0xa6:
		case 0xb6:
		case 0xae:
		case 0xbe:{
			ldx(cpu);
			break;
		}

		case 0xa0:
		case 0xa4:
		case 0xb4:
		case 0xac:
		case 0xbc:{
			ldy(cpu);
			break;
		}

		case 0x4a:
		case 0x46:
		case 0x56:
		case 0x4e:
		case 0x5e:{
			lsr(cpu);
			break;
		}

		case 0x02:	//The rest of these are the KIL unofficial instructions
		case 0x12:	//From my understanding I don't think it does anything
		case 0x22:	//So I am going to just treat it as NOP
		case 0x32:	//https://www.nesdev.org/undocumented_opcodes.txt
		case 0x42:
		case 0x52:
		case 0x62:
		case 0x72:
		case 0x92:
		case 0xb2:
		case 0xd2:
		case 0xf2:
		case 0x1a:	//start of unofficial NOP instructions
		case 0x3a:	
		case 0x5a:	
		case 0x7a:	
		case 0xda:	
		case 0xfa:	//end of unofficial NOP instructions
		
		case 0xea:{	//NOP instruction
			break;
		}

		case 0x09:
		case 0x05:
		case 0x15:
		case 0x0d:
		case 0x1d:
		case 0x19:
		case 0x01:
		case 0x11:{
			ora(cpu);
			break;
		}

		case 0x48:{
			pha(cpu);
			break;

		}

		case 0x08:{
			php(cpu);
			break;
		}

		case 0x68:{
			pla(cpu);
			break;
		}

		case 0x28:{
			plp(cpu);
			break;
		}

		case 0x27:
		case 0x37:
		case 0x2f:
		case 0x3f:
		case 0x3b:
		case 0x23:
		case 0x33:{
			rla(cpu);
			break;
		}

		case 0x2a:
		case 0x26:
		case 0x36:
		case 0x2e:
		case 0x3e:{
			rol(cpu);
			break;
		}
		
		case 0x6a:
		case 0x66:
		case 0x76:
		case 0x6e:
		case 0x7e:{
			ror(cpu);
			break;
		}

		case 0x67:
		case 0x77:
		case 0x6f:
		case 0x7f:
		case 0x7b:
		case 0x63:
		case 0x73:{
			rra(cpu);
			break;
		}

		case 0x40:{
			rti(cpu);
			break;
		}

		case 0x60:{
			rts(cpu);
			break;
		}

		case 0x87:
		case 0x97:
		case 0x83:
		case 0x8f:{
			sax(cpu);
			break;
		}

		case 0xeb:
		case 0xe9:
		case 0xe5:
		case 0xf5:
		case 0xed:
		case 0xfd:
		case 0xf9:
		case 0xe1:
		case 0xf1:{
			sbc(cpu);
			break;
		}

		case 0x38:{
			sec(cpu);
			break;
		}

		case 0xf8:{
			sed(cpu);
			break;
		}

		case 0x78:{
			sei(cpu);
			break;
		}

		case 0x07:
		case 0x17:
		case 0x0f:
		case 0x1f:
		case 0x1b:
		case 0x03:
		case 0x13:{
			slo(cpu);
			break;
		}

		case 0x47:
		case 0x57:
		case 0x4f:
		case 0x5f:
		case 0x5b:
		case 0x43:
		case 0x53:{
			sre(cpu);
			break;
		}

		case 0x85:
		case 0x95:
		case 0x8d:
		case 0x9d:
		case 0x99:
		case 0x81:
		case 0x91:{
			sta(cpu);
			break;
		}

		case 0x86:
		case 0x96:
		case 0x8e:{
			stx(cpu);
			break;
		}

		case 0x84:
		case 0x94:
		case 0x8c:{
			sty(cpu);
			break;
		}

		case 0x9e:{
			sxa(cpu);
			break;
		}

		case 0x9c:{
			sya(cpu);
			break;
		}

		case 0x9b:{
			tas(cpu);
			break;
		}

		case 0xaa:{
			tax(cpu);
			break;
		}
		
		case 0xa8:{
			tay(cpu);
			break;
		}
		
		case 0x0c:	//TOP aka triple nop
		case 0x1c:
		case 0x3c:
		case 0x5c:
		case 0x7c:
		case 0xdc:
		case 0xfc:{
			cpu->PC += 2;
			break;
		}

		case 0xba:{
			tsx(cpu);
			break;
		}

		case 0x8a:{
			txa(cpu);
			break;
		}

		case 0x9a:{
			txs(cpu);
			break;
		}

		case 0x98:{
			tya(cpu);
			break;
		}			

		case 0x8b:{
			xxa(cpu);
			break;
		}

		default:{
			printf("%x instructions does not exist\n", busRead(&(cpu->bus), cpu->PC - 1));
			break;
		}
	}
	struct Opcode opCodes[0x100];
	createOpArray(opCodes);
	busTick(&(cpu->bus), opCodes[opCode].cycles + cpu->extraCycles);
}

void initCPU(struct CPU *cpu, unsigned char *instructions, int instructionsLen){
	cpu->PC = 0xc000;

	cpu->stackPointer = &(cpu->bus.prgRam[0x1fd]);	//stackPointer goes from [0x100-0x1ff] starting at the top and working its way down. I guess it starts at fd becuase that what it is in nesTest.log

	//loadInstructions(cpu, instructions, instructionsLen);

	cpu->processorStatus = 0x24;
}

/*
int main(){	
	unsigned char instructions[] = {0xa9, 0xc0, 0xaa, 0xe8, 0x00, '\0'};
	
	struct CPU cpu = {0};

	initCPU(&cpu, instruction);

	cpuLoop(&cpu);

	printf("accumulator = %x\n", cpu.accumulator);
	printf("x = %x\n", cpu.x);
	printf("processorStatus = %d\n", cpu.processorStatus);
	
}*/
