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

	sprintf(&(str[i]), "%s", opcode.name);

	i += 3;

	str[i++] = ' ';

	switch(opcode.addressingMode){
		case NONEADDRESSING:{
			for(int j = i; j < i + 28; j++){
				str[j] = ' ';
			}
			i += 28;
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
			sprintf(&(str[i]), "%02x,X", busRead(&(cpu->bus), cpu->PC + 1) + cpu->x);
			i += 4;
			sprintf(&(str[i]), " = ");
			i += 3;
			sprintf(&(str[i]), "%02x", busRead(&(cpu->bus), busRead(&(cpu->bus), cpu->PC + 1) + cpu->x));
			i += 2;
			for(int j = i - 9; i > j; j++){
				str[j] = upper(str[j]);
			}
			for(int j = i; j < i + 18; j++){
				str[j] = ' ';
			}
			i += 18;
			break;
		}
		case ZEROPAGEY:{
			str[i++] = '$';
			sprintf(&(str[i]), "%02x,Y", busRead(&(cpu->bus), cpu->PC + 1) + cpu->y);
			i += 4;
			sprintf(&(str[i]), " = ");
			i += 3;
			sprintf(&(str[i]), "%02x", busRead(&(cpu->bus), busRead(&(cpu->bus), cpu->PC + 1) + cpu->y));
			i += 2;
			for(int j = i - 9; i > j; j++){
				str[j] = upper(str[j]);
			}
			for(int j = i; j < i + 18; j++){
				str[j] = ' ';
			}
			i += 18;
			break;
		}
		case ABSOLUTE:{
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
			break;
		}
		case ABSOLUTEX:{
			str[i++] = '$';
			sprintf(&(str[i]), "%04x", absoluteAddress(cpu, cpu->PC + 1) + cpu->x);
			for(int j = 0; j < 4; j++){
				str[i + j] = upper(str[i + j]);
			}
			i += 4;
			for(int j = i; j < i + 23; j++){
				str[j] = ' ';
			}
			i += 23;
			break;
		}
		case ABSOLUTEY:{
			str[i++] = '$';
			sprintf(&(str[i]), "%04x", absoluteAddress(cpu, cpu->PC + 1) + cpu->y);
			for(int j = 0; j < 4; j++){
				str[i + j] = upper(str[i + j]);
			}
			i += 4;
			for(int j = i; j < i + 23; j++){
				str[j] = ' ';
			}
			i += 23;
			break;
		}
		case INDIRECTX:{
			cpu->PC++;
			sprintf(&(str[i]), "(%02x,X) @ %02x = %04x = %02x", busRead(&(cpu->bus), cpu->PC), busRead(&(cpu->bus), cpu->PC) + cpu->x, indirectXAddress(cpu), busRead(&(cpu->bus), indirectXAddress(cpu)));
			i += 17;
			for(int j = i; j < i + 7; j++){
				str[j] = ' ';
			}
			i += 7;
			cpu->PC--;
			break;
		}
		case INDIRECTY:{
			cpu->PC++;
			sprintf(&(str[i]), "(%02x),Y = %04x @ %04x = %02x", busRead(&(cpu->bus), cpu->PC), absoluteAddress(cpu, busRead(&(cpu->bus), cpu->PC)), indirectYAddress(cpu), busRead(&(cpu->bus), indirectYAddress(cpu)));
			i += 17;
			for(int j = i; j < i + 7; j++){
				str[j] = ' ';
			}
			i += 7;
			cpu->PC--;
			break;
		}
		case RELATIVE:{
			//struct CPU orcaCpu = *cpu;
			//cpuLoop(&orcaCpu);
			//sprintf(&(str[i]), "$%04x", orcaCpu.PC);
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
	//printf("i = %d\n", i);
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
