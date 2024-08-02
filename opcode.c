#include "opcode.h"

/*
struct Opcode{
	unsigned char code;
	char name[3];
	int len;
	int cycles;
	int addressingMode;
};
*/
struct Opcode initOpcode(unsigned char code, const char *name, int len, int cycles, int addressingMode){
	struct Opcode opcode;
	opcode.code = code;
	for(int i = 0; i < 3; i++){
		opcode.name[i] = name[i];
	}
	opcode.len = len;
	opcode.cycles = cycles;
	opcode.addressingMode = addressingMode;
	return opcode;
}

void createOpArray(struct Opcode *opcodes){
	//ADC
	opcodes[0x69] = initOpcode(0x69, "ADC", 2, 2, IMMEDIATE);
	opcodes[0x65] = initOpcode(0x65, "ADC", 2, 3, ZEROPAGE);
	opcodes[0x75] = initOpcode(0x75, "ADC", 2, 4, ZEROPAGEX);
	opcodes[0x6d] = initOpcode(0x6d, "ADC", 3, 4, ABSOLUTE);
	opcodes[0x7d] = initOpcode(0x7d, "ADC", 3, 4, ABSOLUTEX);
	opcodes[0x79] = initOpcode(0x79, "ADC", 3, 4, ABSOLUTEY);
	opcodes[0x61] = initOpcode(0x61, "ADC", 2, 6, INDIRECTX);
	opcodes[0x71] = initOpcode(0x71, "ADC", 2, 5, INDIRECTY);

	//AND
	opcodes[0x29] = initOpcode(0x29, "AND", 2, 2, IMMEDIATE);
	opcodes[0x25] = initOpcode(0x25, "AND", 2, 3, ZEROPAGE);
	opcodes[0x35] = initOpcode(0x35, "AND", 2, 4, ZEROPAGEX);
	opcodes[0x2d] = initOpcode(0x2d, "AND", 3, 4, ABSOLUTE);
	opcodes[0x3d] = initOpcode(0x3d, "AND", 3, 4, ABSOLUTEX);
	opcodes[0x39] = initOpcode(0x39, "AND", 3, 4, ABSOLUTEY);
	opcodes[0x21] = initOpcode(0x21, "AND", 2, 6, INDIRECTX);
	opcodes[0x31] = initOpcode(0x31, "AND", 2, 5, INDIRECTY);

	//ASL
	opcodes[0x0a] = initOpcode(0x0a, "ASL", 1, 2, NONEADDRESSING);
	opcodes[0x06] = initOpcode(0x06, "ASL", 2, 5, ZEROPAGE);
	opcodes[0x16] = initOpcode(0x16, "ASL", 2, 6, ZEROPAGEX);
	opcodes[0x0e] = initOpcode(0x0e, "ASL", 3, 6, ABSOLUTE);
	opcodes[0x1e] = initOpcode(0x1e, "ASL", 3, 7, ABSOLUTEX);

	//Branch instructions
	opcodes[0x90] = initOpcode(0x90, "BCC", 2, 2, RELATIVE);
	opcodes[0xb0] = initOpcode(0xb0, "BCS", 2, 2, RELATIVE);
	opcodes[0xf0] = initOpcode(0xf0, "BEQ", 2, 2, RELATIVE);
	opcodes[0x30] = initOpcode(0x30, "BMI", 2, 2, RELATIVE);
	opcodes[0xd0] = initOpcode(0xd0, "BNE", 2, 2, RELATIVE);
	opcodes[0x10] = initOpcode(0x10, "BPL", 2, 2, RELATIVE);
	opcodes[0x50] = initOpcode(0x50, "BVC", 2, 2, RELATIVE);
	opcodes[0x70] = initOpcode(0x70, "BVS", 2, 2, RELATIVE);

	//BIT
	opcodes[0x24] = initOpcode(0x24, "BIT", 2, 3, ZEROPAGE);
	opcodes[0x2c] = initOpcode(0x2c, "BIT", 3, 4, ABSOLUTE);

	//BRK
	opcodes[0x00] = initOpcode(0x00, "BRK", 1, 7, NONEADDRESSING);

	//Clear instuctions
	opcodes[0x18] = initOpcode(0x18, "CLC", 1, 2, NONEADDRESSING);
	opcodes[0xd8] = initOpcode(0xd8, "CLD", 1, 2, NONEADDRESSING);
	opcodes[0x58] = initOpcode(0x58, "CLI", 1, 2, NONEADDRESSING);
	opcodes[0xb8] = initOpcode(0xb8, "CLV", 1, 2, NONEADDRESSING);

	//CMP
	opcodes[0xc9] = initOpcode(0xc9, "CMP", 2, 2, IMMEDIATE);
	opcodes[0xc5] = initOpcode(0xc5, "CMP", 2, 3, ZEROPAGE);
	opcodes[0xd5] = initOpcode(0xd5, "CMP", 2, 4, ZEROPAGEX);
	opcodes[0xcd] = initOpcode(0xcd, "CMP", 3, 4, ABSOLUTE);
	opcodes[0xdd] = initOpcode(0xdd, "CMP", 3, 4, ABSOLUTEX);
	opcodes[0xd9] = initOpcode(0xd9, "CMP", 3, 4, ABSOLUTEY);
	opcodes[0xc1] = initOpcode(0xc1, "CMP", 2, 6, INDIRECTX);
	opcodes[0xd1] = initOpcode(0xd1, "CMP", 2, 5, INDIRECTY);

	//CPX
	opcodes[0xe0] = initOpcode(0xe0, "CPX", 2, 2, IMMEDIATE);
	opcodes[0xe4] = initOpcode(0xe4, "CPX", 2, 3, ZEROPAGE);
	opcodes[0xec] = initOpcode(0xec, "CPX", 3, 4, ABSOLUTE);

	//CPY	
	opcodes[0xc0] = initOpcode(0xc0, "CPY", 2, 2, IMMEDIATE);
	opcodes[0xc4] = initOpcode(0xc4, "CPY", 2, 3, ZEROPAGE);
	opcodes[0xcc] = initOpcode(0xcc, "CPY", 3, 4, ABSOLUTE);

	//DEC
	opcodes[0xc6] = initOpcode(0xc6, "DEC", 2, 5, ZEROPAGE);
	opcodes[0xd6] = initOpcode(0xd6, "DEC", 2, 6, ZEROPAGEX);
	opcodes[0xce] = initOpcode(0xce, "DEC", 3, 6, ABSOLUTE);
	opcodes[0xde] = initOpcode(0xde, "DEC", 3, 7, ABSOLUTEX);

	//DEX
	opcodes[0xca] = initOpcode(0xca, "DEX", 1, 2, NONEADDRESSING);

	//DEY
	opcodes[0x88] = initOpcode(0x88, "DEY", 1, 2, NONEADDRESSING);

	//EOR
	opcodes[0x49] = initOpcode(0x49, "EOR", 2, 2, IMMEDIATE);
	opcodes[0x45] = initOpcode(0x45, "EOR", 2, 3, ZEROPAGE);
	opcodes[0x55] = initOpcode(0x55, "EOR", 2, 4, ZEROPAGEX);
	opcodes[0x4d] = initOpcode(0x4d, "EOR", 3, 4, ABSOLUTE);
	opcodes[0x5d] = initOpcode(0x5d, "EOR", 3, 4, ABSOLUTEX);
	opcodes[0x59] = initOpcode(0x59, "EOR", 3, 4, ABSOLUTEY);
	opcodes[0x41] = initOpcode(0x41, "EOR", 2, 6, INDIRECTX);
	opcodes[0x51] = initOpcode(0x51, "EOR", 2, 5, INDIRECTY);

	//INC
	opcodes[0xe6] = initOpcode(0xe6, "INC", 2, 5, ZEROPAGE);
	opcodes[0xf6] = initOpcode(0xf6, "INC", 2, 6, ZEROPAGEX);
	opcodes[0xee] = initOpcode(0xee, "INC", 3, 6, ABSOLUTE);
	opcodes[0xfe] = initOpcode(0xfe, "INC", 3, 7, ABSOLUTEX);

	//INX
	opcodes[0xe8] = initOpcode(0xe8, "INX", 1, 2, NONEADDRESSING);

	//INY
	opcodes[0xc8] = initOpcode(0xc8, "INY", 1, 2, NONEADDRESSING);
	
	//JMP
	opcodes[0x4c] = initOpcode(0x4c, "JMP", 3, 3, ABSOLUTE);
	opcodes[0x6c] = initOpcode(0x6c, "JMP", 3, 5, NONEADDRESSING);	//make sure to have a special case for this because its not real NONEADDRESSING

	//JSR
	opcodes[0x20] = initOpcode(0x20, "JSR", 3, 6, ABSOLUTE);

	//LDA
	opcodes[0xa9] = initOpcode(0xa9, "LDA", 2, 2, IMMEDIATE);
	opcodes[0xa5] = initOpcode(0xa5, "LDA", 2, 3, ZEROPAGE);
	opcodes[0xb5] = initOpcode(0xb5, "LDA", 2, 4, ZEROPAGEX);
	opcodes[0xad] = initOpcode(0xad, "LDA", 3, 4, ABSOLUTE);
	opcodes[0xbd] = initOpcode(0xbd, "LDA", 3, 4, ABSOLUTEX);
	opcodes[0xb9] = initOpcode(0xb9, "LDA", 3, 4, ABSOLUTEY);
	opcodes[0xa1] = initOpcode(0xa1, "LDA", 2, 6, INDIRECTX);
	opcodes[0xb1] = initOpcode(0xb1, "LDA", 2, 5, INDIRECTY);

	//LDX
	opcodes[0xa2] = initOpcode(0xa2, "LDX", 2, 2, IMMEDIATE);
	opcodes[0xa6] = initOpcode(0xa6, "LDX", 2, 3, ZEROPAGE);
	opcodes[0xb6] = initOpcode(0xb6, "LDX", 2, 4, ZEROPAGEX);
	opcodes[0xae] = initOpcode(0xae, "LDX", 3, 4, ABSOLUTE);
	opcodes[0xbe] = initOpcode(0xbe, "LDX", 3, 4, ABSOLUTEY);

	//LDY	
	opcodes[0xa0] = initOpcode(0xa0, "LDY", 2, 2, IMMEDIATE);
	opcodes[0xa4] = initOpcode(0xa4, "LDY", 2, 3, ZEROPAGE);
	opcodes[0xb4] = initOpcode(0xb4, "LDY", 2, 4, ZEROPAGEX);
	opcodes[0xac] = initOpcode(0xac, "LDY", 3, 4, ABSOLUTE);
	opcodes[0xbc] = initOpcode(0xbc, "LDY", 3, 4, ABSOLUTEX);

	//LSR
	opcodes[0x4a] = initOpcode(0x4a, "LSR", 1, 2, NONEADDRESSING);
	opcodes[0x46] = initOpcode(0x46, "LSR", 2, 5, ZEROPAGE);
	opcodes[0x56] = initOpcode(0x56, "LSR", 2, 6, ZEROPAGEX);
	opcodes[0x4e] = initOpcode(0x4e, "LSR", 3, 6, ABSOLUTE);
	opcodes[0x5e] = initOpcode(0x5e, "LSR", 3, 7, ABSOLUTEX);

	//NOP
	opcodes[0xea] = initOpcode(0xea, "NOP", 1, 2, NONEADDRESSING);

	//ORA
	opcodes[0x09] = initOpcode(0x09, "ORA", 2, 2, IMMEDIATE);
	opcodes[0x05] = initOpcode(0x05, "ORA", 2, 3, ZEROPAGE);
	opcodes[0x15] = initOpcode(0x15, "ORA", 2, 4, ZEROPAGEX);
	opcodes[0x0d] = initOpcode(0x0d, "ORA", 3, 4, ABSOLUTE);
	opcodes[0x1d] = initOpcode(0x1d, "ORA", 3, 4, ABSOLUTEX);
	opcodes[0x19] = initOpcode(0x19, "ORA", 3, 4, ABSOLUTEY);
	opcodes[0x01] = initOpcode(0x01, "ORA", 2, 6, INDIRECTX);
	opcodes[0x11] = initOpcode(0x11, "ORA", 2, 5, INDIRECTY);

	//Push and pull
	opcodes[0x48] = initOpcode(0x48, "PHA", 1, 3, NONEADDRESSING);
	opcodes[0x08] = initOpcode(0x08, "PHP", 1, 3, NONEADDRESSING);
	opcodes[0x68] = initOpcode(0x68, "PLA", 1, 4, NONEADDRESSING);
	opcodes[0x28] = initOpcode(0x28, "PLP", 1, 4, NONEADDRESSING);

	//ROL
	opcodes[0x2a] = initOpcode(0x2a, "ROL", 1, 2, NONEADDRESSING);
	opcodes[0x26] = initOpcode(0x26, "ROL", 2, 5, ZEROPAGE);
	opcodes[0x36] = initOpcode(0x36, "ROL", 2, 6, ZEROPAGEX);
	opcodes[0x2e] = initOpcode(0x2e, "ROL", 3, 6, ABSOLUTE);
	opcodes[0x3e] = initOpcode(0x3e, "ROL", 3, 7, ABSOLUTEX);

	//ROR	
	opcodes[0x6a] = initOpcode(0x6a, "ROR", 1, 2, NONEADDRESSING);
	opcodes[0x66] = initOpcode(0x66, "ROR", 2, 5, ZEROPAGE);
	opcodes[0x76] = initOpcode(0x76, "ROR", 2, 6, ZEROPAGEX);
	opcodes[0x6e] = initOpcode(0x6e, "ROR", 3, 6, ABSOLUTE);
	opcodes[0x7e] = initOpcode(0x7e, "ROR", 3, 7, ABSOLUTEX);
	
	//Return
	opcodes[0x40] = initOpcode(0x40, "RTI", 1, 6, NONEADDRESSING);
	opcodes[0x60] = initOpcode(0x60, "RTS", 1, 6, NONEADDRESSING);

	//SBC
	opcodes[0xe9] = initOpcode(0xe9, "SBC", 2, 2, IMMEDIATE);
	opcodes[0xe5] = initOpcode(0xe5, "SBC", 2, 3, ZEROPAGE);
	opcodes[0xf5] = initOpcode(0xf5, "SBC", 2, 4, ZEROPAGEX);
	opcodes[0xed] = initOpcode(0xed, "SBC", 3, 4, ABSOLUTE);
	opcodes[0xfd] = initOpcode(0xfd, "SBC", 3, 4, ABSOLUTEX);
	opcodes[0xf9] = initOpcode(0xf9, "SBC", 3, 4, ABSOLUTEY);
	opcodes[0xe1] = initOpcode(0xe1, "SBC", 2, 6, INDIRECTX);
	opcodes[0xf1] = initOpcode(0xf1, "SBC", 2, 5, INDIRECTY);

	//Set instructions
	opcodes[0x38] = initOpcode(0x38, "SEC", 1, 2, NONEADDRESSING);
	opcodes[0xf8] = initOpcode(0xf8, "SED", 1, 2, NONEADDRESSING);
	opcodes[0x78] = initOpcode(0x78, "SEI", 1, 2, NONEADDRESSING);

	//STA
	opcodes[0x85] = initOpcode(0x85, "STA", 2, 3, ZEROPAGE);
	opcodes[0x95] = initOpcode(0x95, "STA", 2, 4, ZEROPAGEX);
	opcodes[0x8d] = initOpcode(0x8d, "STA", 3, 4, ABSOLUTE);
	opcodes[0x9d] = initOpcode(0x9d, "STA", 3, 5, ABSOLUTEX);
	opcodes[0x99] = initOpcode(0x99, "STA", 3, 5, ABSOLUTEY);
	opcodes[0x81] = initOpcode(0x81, "STA", 2, 6, INDIRECTX);
	opcodes[0x91] = initOpcode(0x91, "STA", 2, 6, INDIRECTY);

	//STX
	opcodes[0x86] = initOpcode(0x86, "STX", 2, 3, ZEROPAGE);
	opcodes[0x96] = initOpcode(0x96, "STX", 2, 4, ZEROPAGEY);
	opcodes[0x8e] = initOpcode(0x8e, "STX", 3, 4, ABSOLUTE);

	//STY
	opcodes[0x84] = initOpcode(0x84, "STY", 2, 3, ZEROPAGE);
	opcodes[0x94] = initOpcode(0x94, "STY", 2, 4, ZEROPAGEX);
	opcodes[0x8c] = initOpcode(0x8c, "STY", 3, 4, ABSOLUTE);

	//Transfer
	opcodes[0xaa] = initOpcode(0xaa, "TAX", 1, 2, NONEADDRESSING);
	opcodes[0xa8] = initOpcode(0xa8, "TAY", 1, 2, NONEADDRESSING);
	opcodes[0xba] = initOpcode(0xba, "TSX", 1, 2, NONEADDRESSING);
	opcodes[0x8a] = initOpcode(0x8a, "TXA", 1, 2, NONEADDRESSING);
	opcodes[0x9a] = initOpcode(0x9a, "TXS", 1, 2, NONEADDRESSING);
	opcodes[0x98] = initOpcode(0x98, "TYA", 1, 2, NONEADDRESSING);

}

/*int main(){
	struct Opcode opcodes[0x100];
	createOpArray(opcodes);
	printf("%s\n", opcodes[0xba].name);
	return 0;
}*/
