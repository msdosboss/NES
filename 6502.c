#include "6502.h"

/*struct CPU{
	unsigned char accumulator, x, y, processorStatus;
	unsigned short PC;
	unsigned char *stackPointer;
	struct Bus bus;
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

unsigned short absoluteAddress(struct CPU *cpu, unsigned short startingPoint){	//this is kind of a bad name because it's used in indirect address mode as well
	unsigned short address = busRead(&(cpu->bus), startingPoint + 1);	//putting the 2 arg byte in front because this is little eddien 
	address = address << 8;	//making room for the first arg
	address = address | busRead(&(cpu->bus), startingPoint);	//this line assumes that the bits being shifted in are all zero. I dont know this to be the case
	return address;
}

unsigned short indirectXAddress(struct CPU *cpu){
	unsigned char indirectAddress = busRead(&(cpu->bus), cpu->PC);
	indirectAddress = indirectAddress + cpu->x;
	return absoluteAddress(cpu, indirectAddress);
}

unsigned short indirectYAddress(struct CPU *cpu){
	unsigned char indirectAddress = busRead(&(cpu->bus), cpu->PC);
	return absoluteAddress(cpu, indirectAddress) + cpu->y;
}

unsigned short indirectAddress(struct CPU *cpu){
	unsigned short indirectAddress = absoluteAddress(cpu, cpu->PC);
	return absoluteAddress(cpu, indirectAddress);
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
			val = busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC) + cpu->x);
			cpu->PC++;
			break;
		}
		
		case 0x79:{	//absolute,Y
			val = busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC) + cpu->y);
			cpu->PC++;
			break;
		}
		
		case 0x61:{	//indirect,X
			val = busRead(&(cpu->bus), indirectXAddress(cpu));
			break;
		}

		case 0x71:{
			val = busRead(&(cpu->bus), indirectYAddress(cpu));
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
			arg = busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC) + cpu->x);
			cpu->accumulator = cpu->accumulator & arg;
			cpu->PC++;
			break;
		}
		
		case 0x39:{	//absolute,Y
			arg = busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC) + cpu->y);
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
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator & arg;
			break;
		
		}
	}
	cpu->PC++;
	
	negativeFlag(cpu, cpu->accumulator);

	zeroFlag(cpu, cpu->accumulator);
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

void bcc(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000001) == 0){
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bcs(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000001) != 0){
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void beq(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000010) != 0){
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
	if((cpu->processorStatus & 0b10000000) == 0){
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bne(struct CPU *cpu){
	if((cpu->processorStatus & 0b00000010) == 0){
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bpl(struct CPU *cpu){
	if((cpu->processorStatus & 0b10000000) == 0){
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bvc(struct CPU *cpu){
	if((cpu->processorStatus & 0b01000000) == 0){
		cpu->PC = cpu->PC + (char)busRead(&(cpu->bus), cpu->PC);
	}
	cpu->PC++;
}

void bvs(struct CPU *cpu){
	if((cpu->processorStatus & 0b01000000) != 0){
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
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
			result = cpu->x - busRead(&(cpu->bus), cpu->PC);
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator ^ arg;
			cpu->PC++;
			break;
		}
		
		case 0x59:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
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
	//unsigned short index = cpu->programCounter - (cpu->memMap + 1);
	push(cpu, (unsigned char)(index));	//pushes the first 8 bits of the address to stack
	index = index >> 8;
	push(cpu, (unsigned char)(index));
	unsigned short address = absoluteAddress(cpu, cpu->PC);
	cpu->PC = address;
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = arg;
			cpu->PC++;
			break;
		}

		case 0xb9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			arg = busRead(&(cpu->bus), address);
			cpu->accumulator = cpu->accumulator | arg;
			cpu->PC++;
			break;
		}
		
		case 0x19:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
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
			unsigned char address = absoluteAddress(cpu, cpu->PC);
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
			unsigned char address = absoluteAddress(cpu, cpu->PC) + cpu->x;
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
			carryFlag(cpu, preShiftVal);	//storing the 7th bit of the accumulator in carry flag from before the accumulator was shifted
			zeroFlag(cpu, cpu->accumulator);
			negativeFlag(cpu, cpu->accumulator);
			break;
		}

		case 0x66:{	//zero page
			unsigned char address = busRead(&(cpu->bus), cpu->PC);
			preShiftVal = busRead(&(cpu->bus), address);
			busWrite(&(cpu->bus), address, preShiftVal >> 1);
			busWrite(&(cpu->bus), address, busRead(&(cpu->bus), address) | (cpu->processorStatus << 7));
			carryFlag(cpu, preShiftVal);
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
			carryFlag(cpu, preShiftVal);
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
			carryFlag(cpu, preShiftVal);
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
			carryFlag(cpu, preShiftVal);
			zeroFlag(cpu, busRead(&(cpu->bus), address));
			negativeFlag(cpu, busRead(&(cpu->bus), address));
			cpu->PC += 2;
			break;
		}
	}
}

void rti(struct CPU *cpu){
	cpu->processorStatus = pop(cpu);
	cpu->PC = popAbsoluteAddress(cpu);
}

void rts(struct CPU *cpu){
	cpu->PC = popAbsoluteAddress(cpu) + 1;
}

void sbc(struct CPU *cpu){
	unsigned short result;
	unsigned char val;
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC - 1);	
	switch(opCode){
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
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->x;
			val = busRead(&(cpu->bus), address);
			cpu->PC++;
			break;
		}
		
		case 0xf9:{	//absolute,Y
			unsigned short address = absoluteAddress(cpu, cpu->PC) + cpu->y;
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
			unsigned short address = indirectYAddress(cpu);
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
			printf("address = %x\n", address);
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

void txs(struct CPU *cpu){
	*cpu->stackPointer = cpu->x;
}

void tya(struct CPU *cpu){
	cpu->accumulator = cpu->y;
	
	zeroFlag(cpu, cpu->accumulator);
	
	negativeFlag(cpu, cpu->accumulator);
}

void loadInstructions(struct CPU *cpu, char *instructions, int instructionsLen){
	for(int i = cpu->PC; i < instructionsLen + cpu->PC; i++){
		busWrite(&(cpu->bus), i, instructions[i - cpu->PC]);
	}
}

void cpuLoop(struct CPU *cpu){
	//printf("Instruction %x is being run and pc is pointing at %x in memory\n processorStatus = %b\n cpu->accumulator = %x\nval at 0xff = %x and val at 0x02 = %x\n", busRead(&(cpu->bus), cpu->PC), cpu->PC, cpu->processorStatus, cpu->accumulator, busRead(&(cpu->bus), 0xff), busRead(&(cpu->bus), 0x02));
	char *str = malloc(sizeof(char) * 93);
	struct Opcode opcodes[0x100];
	createOpArray(opcodes);
	cycleLog(*cpu, opcodes[busRead(&(cpu->bus), cpu->PC)], str);
	printf("%s\n", str);
	unsigned char opCode = busRead(&(cpu->bus), cpu->PC);
	switch(opCode){
		case 0x69:
		case 0x65:
		case 0x75:
		case 0x6d:
		case 0x7d:
		case 0x79:
		case 0x61:
		case 0x71:{
			cpu->PC++;
			adc(cpu);
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
			cpu->PC++;
			and(cpu);
			break;
		}			
		
		case 0x0a:
		case 0x06:
		case 0x16:
		case 0x0e:
		case 0x1e:{
			cpu->PC++;
			asl(cpu);
			break;
		}

		case 0x90:{
			cpu->PC++;
			bcc(cpu);
			break;	
		}
		
		case 0xb0:{
			cpu->PC++;
			bcs(cpu);
			break;			
		}

		case 0xf0:{
			cpu->PC++;
			beq(cpu);
			break;
		}		

		case 0x24:
		case 0x2c:{
			cpu->PC++;
			bit(cpu);
			break;
		}

		case 0x30:{
			cpu->PC++;
			bmi(cpu);
			break;
		}

		case 0xd0:{
			cpu->PC++;
			bne(cpu);
			break;
		}

		case 0x10:{
			cpu->PC++;
			bpl(cpu);
			break;
		}

		case 0x00:{
			cpu->PC++;
			cpu->processorStatus |= 0b00010000;
			break;
		}

		case 0x50:{
			cpu->PC++;
			bvc(cpu);
			break;
		}

		case 0x70:{
			cpu->PC++;
			bvs(cpu);
			break;
		}

		case 0x18:{
			cpu->PC++;
			clc(cpu);
			break;
		}

		case 0xd8:{
			cpu->PC++;
			cld(cpu);
			break;
		}

		case 0x58:{
			cpu->PC++;
			cli(cpu);
			break;
		}

		case 0xb8:{
			cpu->PC++;
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
			cpu->PC++;
			cmp(cpu);
			break;
		}

		case 0xe0:
		case 0xe4:
		case 0xec:{
			cpu->PC++;
			cpx(cpu);
			break;
		}

		case 0xc0:
		case 0xc4:
		case 0xcc:{
			cpu->PC++;
			cpy(cpu);
			break;
		}

		case 0xc6:
		case 0xd6:
		case 0xce:
		case 0xde:{
			cpu->PC++;
			dec(cpu);
			break;
		}

		case 0xca:{
			cpu->PC++;
			dex(cpu);
			break;
		}

		case 0x88:{
			cpu->PC++;
			dey(cpu);
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
			cpu->PC++;
			eor(cpu);
			break;
		}

		case 0xe6:
		case 0xf6:
		case 0xee:
		case 0xfe:{
			cpu->PC++;
			inc(cpu);
			break;
		}

		case 0xe8:{
			cpu->PC++;
			inx(cpu);
			break;

		}

		case 0xc8:{
			cpu->PC++;
			iny(cpu);
			break;

		}

		case 0x4c:
		case 0x6c:{
			cpu->PC++;
			jmp(cpu);
			break;

		}

		case 0x20:{
			cpu->PC++;
			jsr(cpu);
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
			cpu->PC++;
			lda(cpu);
			break;
		}


		case 0xa2:
		case 0xa6:
		case 0xb6:
		case 0xae:
		case 0xbe:{
			cpu->PC++;
			ldx(cpu);
			break;
		}

		case 0xa0:
		case 0xa4:
		case 0xb4:
		case 0xac:
		case 0xbc:{
			cpu->PC++;
			ldy(cpu);
			break;
		}

		case 0x4a:
		case 0x46:
		case 0x56:
		case 0x4e:
		case 0x5e:{
			cpu->PC++;
			lsr(cpu);
			break;
		}

		case 0xea:{	//NOP instruction
			cpu->PC++;
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
			cpu->PC++;
			ora(cpu);
			break;
		}

		case 0x48:{
			cpu->PC++;
			pha(cpu);
			break;

		}

		case 0x08:{
			cpu->PC++;
			php(cpu);
			break;
		}

		case 0x68:{
			cpu->PC++;
			pla(cpu);
			break;
		}

		case 0x28:{
			cpu->PC++;
			plp(cpu);
			break;
		}

		case 0x2a:
		case 0x26:
		case 0x36:
		case 0x2e:
		case 0x3e:{
			cpu->PC++;
			rol(cpu);
			break;
		}
		
		case 0x6a:
		case 0x66:
		case 0x76:
		case 0x6e:
		case 0x7e:{
			cpu->PC++;
			ror(cpu);
			break;
		}

		case 0x40:{
			cpu->PC++;
			rti(cpu);
			break;
		}

		case 0x60:{
			cpu->PC++;
			rts(cpu);
			break;
		}

		case 0xe9:
		case 0xe5:
		case 0xf5:
		case 0xed:
		case 0xfd:
		case 0xf9:
		case 0xe1:
		case 0xf1:{
			cpu->PC++;
			sbc(cpu);
			break;
		}

		case 0x38:{
			cpu->PC++;
			sec(cpu);
			break;
		}

		case 0xf8:{
			cpu->PC++;
			sed(cpu);
			break;
		}

		case 0x78:{
			cpu->PC++;
			sei(cpu);
			break;
		}

		case 0x85:
		case 0x95:
		case 0x8d:
		case 0x9d:
		case 0x99:
		case 0x81:
		case 0x91:{
			cpu->PC++;
			sta(cpu);
			break;
		}

		case 0x86:
		case 0x96:
		case 0x8e:{
			cpu->PC++;
			stx(cpu);
			break;
		}

		case 0x84:
		case 0x94:
		case 0x8c:{
			cpu->PC++;
			sty(cpu);
			break;
		}

		case 0xaa:{
			cpu->PC++;
			tax(cpu);
			break;
		}
		
		case 0xa8:{
			cpu->PC++;
			tay(cpu);
			break;
		}
		
		case 0xba:{
			cpu->PC++;
			tsx(cpu);
			break;
		}

		case 0x8a:{
			cpu->PC++;
			txa(cpu);
			break;
		}

		case 0x9a:{
			cpu->PC++;
			txs(cpu);
			break;
		}

		case 0x98:{
			cpu->PC++;
			tya(cpu);
			break;
		}			

		default:{
			cpu->PC++;
			printf("%x instructions does not exist\n", busRead(&(cpu->bus), cpu->PC - 1));
			break;
		}
	}
}

void initCPU(struct CPU *cpu, unsigned char *instructions, int instructionsLen){
	cpu->PC = 0xc000;

	cpu->stackPointer = &(cpu->bus.prgRam[0x1ff]);	//stackPointer goes from [0x100-0x1ff] starting at the top and working its way down

	//loadInstructions(cpu, instructions, instructionsLen);

	cpu->processorStatus &= 0;
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
