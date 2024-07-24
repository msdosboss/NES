#include "log.h"

char upper(char lower){
	if(lower >= 'a' && lower <= 'f'){
		lower -= 32;	//Uppercasing each letter
	}
	return lower;
}

void cycleLog(struct CPU cpu, struct Opcode opcode, char *str){
	//char *str = malloc(sizeof(char) * 93);
	int i = 0;

	sprintf(str, "%04x", cpu.PC);

	i += 4;
	
	str[i++] = ' ';
	str[i++] = ' ';

	for(int j = 0; j < 4; j++){
		str[j] = upper(str[j]);
	}

	for(int j = 0; j < 3; j++){
		if(opcode.len >= j + 1){
				sprintf(&(str[i]), "%02x ", busRead(&(cpu.bus), cpu.PC + j));
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
		case IMMEDIATE:{
			str[i++] = '#';
			str[i++] = '$';
			sprintf(&(str[i]), "%02x", busRead(&(cpu.bus), cpu.PC + 1));
			i += 2;
			break;
			
		}

		case ZEROPAGE:{
			str[i++] = '$';
			sprintf(&(str[i]), "%02x", busRead(&(cpu.bus), cpu.PC + 1));
			i += 2;
			sprintf(&(str[i]), " = ");
			i += 3;
			sprintf(&(str[i]), "%02x", busRead(&(cpu.bus), busRead(&(cpu.bus), cpu.PC + 1)));
			i += 2;
			break;
		}

		case ZEROPAGEX:{
			str[i++] = '$';
			sprintf(&(str[i]), "%02x,X", busRead(&(cpu.bus), cpu.PC + 1) + cpu.x);
			i += 4;
			sprintf(&(str[i]), " = ");
			i += 3;
			sprintf(&(str[i]), "%02x", busRead(&(cpu.bus), busRead(&(cpu.bus), cpu.PC + 1) + cpu.x));
			i += 2;
			break;
		}
		case ZEROPAGEY:{
			str[i++] = '$';
			sprintf(&(str[i]), "%02x,Y", busRead(&(cpu.bus), cpu.PC + 1) + cpu.y);
			i += 4;
			sprintf(&(str[i]), " = ");
			i += 3;
			sprintf(&(str[i]), "%02x", busRead(&(cpu.bus), busRead(&(cpu.bus), cpu.PC + 1) + cpu.y));
			i += 2;
			break;
		}
		case ABSOLUTE:{
			str[i++] = '$';
			sprintf(&(str[i]), "%04x", absoluteAddress(&cpu, cpu.PC + 1));
			for(int j = 0; j < 4; j++){
				str[i + j] = upper(str[i + j]);
			}
			break;
		}
		case ABSOLUTEX:{
			str[i++] = '$';
			sprintf(&(str[i]), "%04x", absoluteAddress(&cpu, cpu.PC + 1) + cpu.x);
			for(int j = 0; j < 4; j++){
				str[i + j] = upper(str[i + j]);
			}
			break;
		}
		case ABSOLUTEY:{
			str[i++] = '$';
			sprintf(&(str[i]), "%04x", absoluteAddress(&cpu, cpu.PC + 1) + cpu.y);
			for(int j = 0; j < 4; j++){
				str[i + j] = upper(str[i + j]);
			}
			break;
		}
		case INDIRECTX:{
			sprintf(&(str[i]), "(%02x,X) @ %02x = %04x = %02x", busRead(&(cpu.bus), cpu.PC + 1), busRead(&(cpu.bus), cpu.PC + 1) + cpu.x, indirectXAddress(cpu.PC + 1), busRead(&(cpu.bus), indirectXAddress(cpu.PC + 1)));
			break;
		}
		case INDIRECTY:{
			sprintf(&(str[i]), "(%02x),Y @ %02x = %04x = %02x", busRead(&(cpu.bus), cpu.PC + 1), busRead(&(cpu.bus), cpu.PC + 1) + cpu.y, indirectYAddress(cpu.PC + 1), busRead(&(cpu.bus), indirectYAddress(cpu.PC + 1)));
			break;
		}
	}
	
}

int main(){
	char *str = malloc(sizeof(char) * 93);
	struct CPU cpu = {0};
	cpu.PC = 0xc5f7;
	struct Opcode opcodes[0x100];
	createOpArray(opcodes);
	cpu.bus.rom = nesCartRead("nesTest.nes");
	cycleLog(cpu, opcodes[busRead(&(cpu.bus), cpu.PC)], str);

	printf("%s", str);

	return 0;
}
