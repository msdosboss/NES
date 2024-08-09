#include "log.h"

char upper(char lower){
	if(lower >= 'a' && lower <= 'f'){
		lower -= 32;	//Uppercasing each letter
	}
	return lower;
}

void cycleLog(struct CPU *cpu, struct Opcode opcode, char *str){
	//char *str = malloc(sizeof(char) * 93);
	int i = 0;

	sprintf(str, "%04x", cpu->PC);

	i += 4;
	
	str[i++] = ' ';
	str[i++] = ' ';

	for(int j = 0; j < 4; j++){
		str[j] = upper(str[j]);
	}

	for(int j = 0; j < 3; j++){
		if(opcode.len >= j + 1){
				sprintf(&(str[i]), "%02x ", busRead(&(cpu->bus), cpu->PC + j));
			for(int jj = 0; jj < 2; jj++){
				str[jj + i] = upper(str[jj + i]);
			}
		}
		else{
			sprintf(&(str[i]), "   ");
		}
		i += 3;
		
	}
	
	str[i++] = ' ';

	if(opcode.isOfficial == UNOFFICIAL){
		str[i++] = '*';
	}

	sprintf(&(str[i]), "%s", opcode.name);

	i += 3;

	str[i++] = ' ';

	switch(opcode.addressingMode){
		case NONEADDRESSING:{
			if(opcode.code == 0x4a || opcode.code == 0x6a || opcode.code == 0x2a || opcode.code == 0x0a){
				str[i++] = 'A';
				for(int j = i; j < i + 27; j++){
					str[j] = ' ';
				}
				i += 27;
			}
			else if(opcode.code == 0x6c){	//indirect jmp addressing
				cpu->PC++;
				sprintf(&(str[i]), "($%04x) = %04x", absoluteAddress(cpu, cpu->PC), indirectAddress(cpu));
				cpu->PC--;
				for(int j = i; j < i + 14; j++){
					str[j] = upper(str[j]);
				}
				i += 14;
				for(int j = i; j < i + 14; j++){
					str[j] = ' ';
				}
				i += 14;
			}
			else{
				for(int j = i; j < i + 28; j++){
					str[j] = ' ';
				}
				i += 28;
			}
			break;
		}

		case IMMEDIATE:{
			str[i++] = '#';
			str[i++] = '$';
			sprintf(&(str[i]), "%02x", busRead(&(cpu->bus), cpu->PC + 1));
			for(int j = i;  j < i + 2; j++){
				str[j] = upper(str[j]);
			}
			i += 2;
			for(int j = i; j < i + 24; j++){
				str[j] = ' ';
			}
			i += 24;
			break;
			
		}

		case ZEROPAGE:{
			str[i++] = '$';
			sprintf(&(str[i]), "%02x", busRead(&(cpu->bus), cpu->PC + 1));
			i += 2;
			sprintf(&(str[i]), " = ");
			i += 3;
			sprintf(&(str[i]), "%02x", busRead(&(cpu->bus), busRead(&(cpu->bus), cpu->PC + 1)));
			i += 2;
			for(int j = i - 7; i > j; j++){
				str[j] = upper(str[j]);
			}
			for(int j = i; j < i + 20; j++){
				str[j] = ' ';
			}
			i += 20;
			break;
		}

		case ZEROPAGEX:{
			str[i++] = '$';
			sprintf(&(str[i]), "%02x,X @ %02x = %02x", busRead(&(cpu->bus), cpu->PC + 1), busRead(&(cpu->bus), cpu->PC + 1) + cpu->x & 0xff, busRead(&(cpu->bus), busRead(&(cpu->bus), cpu->PC + 1) + cpu->x &0xff));
			for(int j = i; j < i + 14; j++){
				str[j] = upper(str[j]);
			}
			i += 14;
			for(int j = i; j < i + 13; j++){
				str[j] = ' ';
			}
			i += 13;
			break;
		}
		case ZEROPAGEY:{
			str[i++] = '$';
			sprintf(&(str[i]), "%02x,Y @ %02x = %02x", busRead(&(cpu->bus), cpu->PC + 1), busRead(&(cpu->bus), cpu->PC + 1) + cpu->y & 0xff, busRead(&(cpu->bus), busRead(&(cpu->bus), cpu->PC + 1) + cpu->y &0xff));
			for(int j = i; j < i + 14; j++){
				str[j] = upper(str[j]);
			}
			i += 14;
			for(int j = i; j < i + 13; j++){
				str[j] = ' ';
			}
			i += 13;
			break;
		}
		case ABSOLUTE:{
			if(opcode.code == 0x4c || opcode.code == 0x20){	//JMP or JSR
				str[i++] = '$';
				sprintf(&(str[i]), "%04x", absoluteAddress(cpu, cpu->PC + 1));
				for(int j = 0; j < 4; j++){
					str[i + j] = upper(str[i + j]);
				}
				i += 4;
				for(int j = i; j < i + 23; j++){
					str[j] = ' ';
				}
				i += 23;
			}
			else{
				str[i++] = '$';
				sprintf(&(str[i]), "%04x = %02x", absoluteAddress(cpu, cpu->PC + 1), busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC + 1)));
				for(int j = 0; j < 9; j++){
					str[i + j] = upper(str[i + j]);
				}
				i += 9;
				for(int j = i; j < i + 18; j++){
					str[j] = ' ';
				}
				i += 18;
			}
			break;
		}
		case ABSOLUTEX:{
			unsigned short address = absoluteAddress(cpu, cpu->PC + 1) + cpu->x;
			str[i++] = '$';
			sprintf(&(str[i]), "%04x,X @ %04x = %02x", absoluteAddress(cpu, cpu->PC + 1), absoluteAddress(cpu, cpu->PC + 1) + cpu->x, busRead(&(cpu->bus), address));
			for(int j = i; j < i + 18; j++){
				str[j] = upper(str[j]);
			}
			i += 18;
			for(int j = i; j < i + 9; j++){
				str[j] = ' ';
			}
			i += 9;
			break;
		}
		case ABSOLUTEY:{
			str[i++] = '$';
			sprintf(&(str[i]), "%04x,Y @ %04x = %02x", absoluteAddress(cpu, cpu->PC + 1), ((absoluteAddress(cpu, cpu->PC + 1) + cpu->y) & 0xffff), busRead(&(cpu->bus), absoluteAddress(cpu, cpu->PC + 1) + cpu->y));
			for(int j = 0; j < 18; j++){
				str[i + j] = upper(str[i + j]);
			}
			i += 18;
			for(int j = i; j < i + 9; j++){
				str[j] = ' ';
			}
			i += 9;
			break;
		}
		case INDIRECTX:{
			cpu->PC++;
			sprintf(&(str[i]), "($%02x,X) @ %02x = %04x = %02x", busRead(&(cpu->bus), cpu->PC), ((busRead(&(cpu->bus), cpu->PC) + cpu->x) & 0b11111111), indirectXAddress(cpu), busRead(&(cpu->bus), indirectXAddress(cpu)));
			for(int j = i; j < i + 24; j++){
				str[j] = upper(str[j]);
			}
			i += 24;
			for(int j = i; j < i + 4; j++){
				str[j] = ' ';
			}
			i += 4;
			cpu->PC--;
			break;
		}
		case INDIRECTY:{
			cpu->PC++;
			sprintf(&(str[i]), "($%02x),Y = %04x @ %04x = %02x", busRead(&(cpu->bus), cpu->PC), rollOverAbsoluteAddress(cpu, busRead(&(cpu->bus), cpu->PC)), indirectYAddress(cpu), busRead(&(cpu->bus), indirectYAddress(cpu)));
			for(int j = i; j < i + 26; j++){
				str[j] = upper(str[j]);
			}
			i += 26; 
			for(int j = i; j < i + 2; j++){
				str[j] = ' ';
			}
			i += 2;
			cpu->PC--;
			break;
		}
		case RELATIVE:{
			sprintf(&(str[i]), "$%04x", cpu->PC + 2 + (char)busRead(&(cpu->bus), cpu->PC + 1));
			for(int j = i; j < i + 5; j++){
				str[j] = upper(str[j]);
			}
			i += 5;
			for(int j = i; j < i + 23; j++){
				str[j] = ' ';
			}
			i += 23;
		}
	}
	sprintf(&(str[i]), "A:%02x X:%02x Y:%02x P:%02x SP:%02x", cpu->accumulator, cpu->x, cpu->y, cpu->processorStatus, (cpu->stackPointer - cpu->bus.prgRam) - 0x100);
	for(int j = i; j < i + 25; j++){
		str[j] = upper(str[j]);
	}
}

/*int main(){
	char *str = malloc(sizeof(char) * 93);
	struct CPU cpu = {0};
	cpu.PC = 0xd922;
	struct Opcode opcodes[0x100];
	createOpArray(opcodes);
	cpu.bus.rom = nesCartRead("nesTest.nes");
	cycleLog(cpu, opcodes[busRead(&(cpu.bus), cpu.PC)], str);

	printf("%s", str);

	return 0;
}*/
