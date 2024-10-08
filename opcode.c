#include "opcode.h"

/*
struct Opcode{
	unsigned char code;
	char name[3];
	int len;
	int cycles;
	int addressingMode;
	int isOfficial;
};
*/
struct Opcode initOpcode(unsigned char code, const char *name, int len, int cycles, int addressingMode, int isOfficial){
	struct Opcode opcode;
	opcode.code = code;
	for(int i = 0; i < 3; i++){
		opcode.name[i] = name[i];
	}
	opcode.len = len;
	opcode.cycles = cycles;
	opcode.addressingMode = addressingMode;
	opcode.isOfficial = isOfficial;
	return opcode;
}

void createOpArray(struct Opcode *opcodes){
	//ADC
	opcodes[0x69] = initOpcode(0x69, "ADC", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0x65] = initOpcode(0x65, "ADC", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x75] = initOpcode(0x75, "ADC", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0x6d] = initOpcode(0x6d, "ADC", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0x7d] = initOpcode(0x7d, "ADC", 3, 4, ABSOLUTEX, OFFICIAL);
	opcodes[0x79] = initOpcode(0x79, "ADC", 3, 4, ABSOLUTEY, OFFICIAL);
	opcodes[0x61] = initOpcode(0x61, "ADC", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0x71] = initOpcode(0x71, "ADC", 2, 5, INDIRECTY, OFFICIAL);

	//AHX
	opcodes[0x9f] = initOpcode(0x9f, "AHX", 3, 5, ABSOLUTEY, UNOFFICIAL);
	opcodes[0x93] = initOpcode(0x93, "AHX", 2, 6, INDIRECTY, UNOFFICIAL);

	//ALR
	opcodes[0x4b] = initOpcode(0x4b, "ALR", 2, 2, IMMEDIATE, UNOFFICIAL);

	//ANC
	opcodes[0x0b] = initOpcode(0x0b, "ANC", 2, 2, IMMEDIATE, UNOFFICIAL);
	opcodes[0x2b] = initOpcode(0x2b, "ANC", 2, 2, IMMEDIATE, UNOFFICIAL);

	//AND
	opcodes[0x29] = initOpcode(0x29, "AND", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0x25] = initOpcode(0x25, "AND", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x35] = initOpcode(0x35, "AND", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0x2d] = initOpcode(0x2d, "AND", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0x3d] = initOpcode(0x3d, "AND", 3, 4, ABSOLUTEX, OFFICIAL);
	opcodes[0x39] = initOpcode(0x39, "AND", 3, 4, ABSOLUTEY, OFFICIAL);
	opcodes[0x21] = initOpcode(0x21, "AND", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0x31] = initOpcode(0x31, "AND", 2, 5, INDIRECTY, OFFICIAL);

	//ARR
	opcodes[0x6b] = initOpcode(0x6b, "ARR", 2, 2, IMMEDIATE, UNOFFICIAL);

	//ASL
	opcodes[0x0a] = initOpcode(0x0a, "ASL", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x06] = initOpcode(0x06, "ASL", 2, 5, ZEROPAGE, OFFICIAL);
	opcodes[0x16] = initOpcode(0x16, "ASL", 2, 6, ZEROPAGEX, OFFICIAL);
	opcodes[0x0e] = initOpcode(0x0e, "ASL", 3, 6, ABSOLUTE, OFFICIAL);
	opcodes[0x1e] = initOpcode(0x1e, "ASL", 3, 7, ABSOLUTEX, OFFICIAL);

	//AXS
	opcodes[0xcb] = initOpcode(0xcb, "AXS", 2, 2, IMMEDIATE, UNOFFICIAL);	

	//Branch instructions
	opcodes[0x90] = initOpcode(0x90, "BCC", 2, 2, RELATIVE, OFFICIAL);
	opcodes[0xb0] = initOpcode(0xb0, "BCS", 2, 2, RELATIVE, OFFICIAL);
	opcodes[0xf0] = initOpcode(0xf0, "BEQ", 2, 2, RELATIVE, OFFICIAL);
	opcodes[0x30] = initOpcode(0x30, "BMI", 2, 2, RELATIVE, OFFICIAL);
	opcodes[0xd0] = initOpcode(0xd0, "BNE", 2, 2, RELATIVE, OFFICIAL);
	opcodes[0x10] = initOpcode(0x10, "BPL", 2, 2, RELATIVE, OFFICIAL);
	opcodes[0x50] = initOpcode(0x50, "BVC", 2, 2, RELATIVE, OFFICIAL);
	opcodes[0x70] = initOpcode(0x70, "BVS", 2, 2, RELATIVE, OFFICIAL);

	//BIT
	opcodes[0x24] = initOpcode(0x24, "BIT", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x2c] = initOpcode(0x2c, "BIT", 3, 4, ABSOLUTE, OFFICIAL);

	//BRK
	opcodes[0x00] = initOpcode(0x00, "BRK", 1, 7, NONEADDRESSING, OFFICIAL);

	//Clear instuctions
	opcodes[0x18] = initOpcode(0x18, "CLC", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0xd8] = initOpcode(0xd8, "CLD", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x58] = initOpcode(0x58, "CLI", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0xb8] = initOpcode(0xb8, "CLV", 1, 2, NONEADDRESSING, OFFICIAL);

	//CMP
	opcodes[0xc9] = initOpcode(0xc9, "CMP", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0xc5] = initOpcode(0xc5, "CMP", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0xd5] = initOpcode(0xd5, "CMP", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0xcd] = initOpcode(0xcd, "CMP", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0xdd] = initOpcode(0xdd, "CMP", 3, 4, ABSOLUTEX, OFFICIAL);
	opcodes[0xd9] = initOpcode(0xd9, "CMP", 3, 4, ABSOLUTEY, OFFICIAL);
	opcodes[0xc1] = initOpcode(0xc1, "CMP", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0xd1] = initOpcode(0xd1, "CMP", 2, 5, INDIRECTY, OFFICIAL);

	//CPX
	opcodes[0xe0] = initOpcode(0xe0, "CPX", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0xe4] = initOpcode(0xe4, "CPX", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0xec] = initOpcode(0xec, "CPX", 3, 4, ABSOLUTE, OFFICIAL);

	//CPY	
	opcodes[0xc0] = initOpcode(0xc0, "CPY", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0xc4] = initOpcode(0xc4, "CPY", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0xcc] = initOpcode(0xcc, "CPY", 3, 4, ABSOLUTE, OFFICIAL);

	//DCP
	opcodes[0xc7] = initOpcode(0xc7, "DCP", 2, 5, ZEROPAGE, UNOFFICIAL);	
	opcodes[0xd7] = initOpcode(0xd7, "DCP", 2, 6, ZEROPAGEX, UNOFFICIAL);	
	opcodes[0xcf] = initOpcode(0xcf, "DCP", 3, 6, ABSOLUTE, UNOFFICIAL);	
	opcodes[0xdf] = initOpcode(0xdf, "DCP", 3, 7, ABSOLUTEX, UNOFFICIAL);	
	opcodes[0xdb] = initOpcode(0xdb, "DCP", 3, 7, ABSOLUTEY, UNOFFICIAL);	
	opcodes[0xc3] = initOpcode(0xc3, "DCP", 2, 8, INDIRECTX, UNOFFICIAL);	
	opcodes[0xd3] = initOpcode(0xd3, "DCP", 2, 8, INDIRECTY, UNOFFICIAL);	

	//DEC
	opcodes[0xc6] = initOpcode(0xc6, "DEC", 2, 5, ZEROPAGE, OFFICIAL);
	opcodes[0xd6] = initOpcode(0xd6, "DEC", 2, 6, ZEROPAGEX, OFFICIAL);
	opcodes[0xce] = initOpcode(0xce, "DEC", 3, 6, ABSOLUTE, OFFICIAL);
	opcodes[0xde] = initOpcode(0xde, "DEC", 3, 7, ABSOLUTEX, OFFICIAL);

	//DEX
	opcodes[0xca] = initOpcode(0xca, "DEX", 1, 2, NONEADDRESSING, OFFICIAL);

	//DEY
	opcodes[0x88] = initOpcode(0x88, "DEY", 1, 2, NONEADDRESSING, OFFICIAL);

	//DOP
	opcodes[0x80] = initOpcode(0x80, "NOP", 2, 2, IMMEDIATE, UNOFFICIAL);	
	opcodes[0x82] = initOpcode(0x82, "NOP", 2, 2, IMMEDIATE, UNOFFICIAL);	
	opcodes[0x89] = initOpcode(0x89, "NOP", 2, 2, IMMEDIATE, UNOFFICIAL);	
	opcodes[0xc2] = initOpcode(0xc2, "NOP", 2, 2, IMMEDIATE, UNOFFICIAL);	
	opcodes[0xe2] = initOpcode(0xe2, "NOP", 2, 2, IMMEDIATE, UNOFFICIAL);	
	opcodes[0x04] = initOpcode(0x04, "NOP", 2, 3, ZEROPAGE, UNOFFICIAL);	
	opcodes[0x44] = initOpcode(0x44, "NOP", 2, 3, ZEROPAGE, UNOFFICIAL);	
	opcodes[0x64] = initOpcode(0x64, "NOP", 2, 3, ZEROPAGE, UNOFFICIAL);	
	opcodes[0x14] = initOpcode(0x14, "NOP", 2, 4, ZEROPAGEX, UNOFFICIAL);	
	opcodes[0x34] = initOpcode(0x34, "NOP", 2, 4, ZEROPAGEX, UNOFFICIAL);	
	opcodes[0x54] = initOpcode(0x54, "NOP", 2, 4, ZEROPAGEX, UNOFFICIAL);	
	opcodes[0x74] = initOpcode(0x74, "NOP", 2, 4, ZEROPAGEX, UNOFFICIAL);	
	opcodes[0xd4] = initOpcode(0xd4, "NOP", 2, 4, ZEROPAGEX, UNOFFICIAL);	
	opcodes[0xf4] = initOpcode(0xf4, "NOP", 2, 4, ZEROPAGEX, UNOFFICIAL);	
	
	//EOR
	opcodes[0x49] = initOpcode(0x49, "EOR", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0x45] = initOpcode(0x45, "EOR", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x55] = initOpcode(0x55, "EOR", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0x4d] = initOpcode(0x4d, "EOR", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0x5d] = initOpcode(0x5d, "EOR", 3, 4, ABSOLUTEX, OFFICIAL);
	opcodes[0x59] = initOpcode(0x59, "EOR", 3, 4, ABSOLUTEY, OFFICIAL);
	opcodes[0x41] = initOpcode(0x41, "EOR", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0x51] = initOpcode(0x51, "EOR", 2, 5, INDIRECTY, OFFICIAL);

	//INC
	opcodes[0xe6] = initOpcode(0xe6, "INC", 2, 5, ZEROPAGE, OFFICIAL);
	opcodes[0xf6] = initOpcode(0xf6, "INC", 2, 6, ZEROPAGEX, OFFICIAL);
	opcodes[0xee] = initOpcode(0xee, "INC", 3, 6, ABSOLUTE, OFFICIAL);
	opcodes[0xfe] = initOpcode(0xfe, "INC", 3, 7, ABSOLUTEX, OFFICIAL);

	//INX
	opcodes[0xe8] = initOpcode(0xe8, "INX", 1, 2, NONEADDRESSING, OFFICIAL);

	//INY
	opcodes[0xc8] = initOpcode(0xc8, "INY", 1, 2, NONEADDRESSING, OFFICIAL);

	//ISB
	opcodes[0xe7] = initOpcode(0xe7, "ISB", 2, 5, ZEROPAGE, UNOFFICIAL);
	opcodes[0xf7] = initOpcode(0xf7, "ISB", 2, 6, ZEROPAGEX, UNOFFICIAL);
	opcodes[0xef] = initOpcode(0xef, "ISB", 3, 6, ABSOLUTE, UNOFFICIAL);
	opcodes[0xff] = initOpcode(0xff, "ISB", 3, 7, ABSOLUTEX, UNOFFICIAL);
	opcodes[0xfb] = initOpcode(0xfb, "ISB", 3, 7, ABSOLUTEY, UNOFFICIAL);
	opcodes[0xe3] = initOpcode(0xe3, "ISB", 2, 8, INDIRECTX, UNOFFICIAL);
	opcodes[0xf3] = initOpcode(0xf3, "ISB", 2, 8, INDIRECTY, UNOFFICIAL);
	
	//JMP
	opcodes[0x4c] = initOpcode(0x4c, "JMP", 3, 3, ABSOLUTE, OFFICIAL);
	opcodes[0x6c] = initOpcode(0x6c, "JMP", 3, 5, NONEADDRESSING, OFFICIAL);	//make sure to have a special case for this because its not real NONEADDRESSING

	//JSR
	opcodes[0x20] = initOpcode(0x20, "JSR", 3, 6, ABSOLUTE, OFFICIAL);

	//KIL
	opcodes[0x02] = initOpcode(0x02, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x12] = initOpcode(0x12, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x22] = initOpcode(0x22, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x32] = initOpcode(0x32, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x42] = initOpcode(0x42, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x52] = initOpcode(0x52, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x62] = initOpcode(0x62, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x72] = initOpcode(0x72, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x92] = initOpcode(0x92, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0xb2] = initOpcode(0xb2, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0xd2] = initOpcode(0xd2, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);
	opcodes[0xf2] = initOpcode(0xf2, "KIL", 1, 1, NONEADDRESSING, UNOFFICIAL);

	//LAS
	opcodes[0xbb] = initOpcode(0xbb, "LAS", 3, 4, ABSOLUTEY, UNOFFICIAL);

	//LAX
	opcodes[0xa7] = initOpcode(0xa7, "LAX", 2, 3, ZEROPAGE, UNOFFICIAL);
	opcodes[0xb7] = initOpcode(0xb7, "LAX", 2, 4, ZEROPAGEY, UNOFFICIAL);
	opcodes[0xaf] = initOpcode(0xAF, "LAX", 3, 4, ABSOLUTE, UNOFFICIAL);
	opcodes[0xbf] = initOpcode(0xbf, "LAX", 3, 4, ABSOLUTEY, UNOFFICIAL);
	opcodes[0xa3] = initOpcode(0xa3, "LAX", 2, 6, INDIRECTX, UNOFFICIAL);
	opcodes[0xb3] = initOpcode(0xb3, "LAX", 2, 5, INDIRECTY, UNOFFICIAL);

	//LDA
	opcodes[0xa9] = initOpcode(0xa9, "LDA", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0xa5] = initOpcode(0xa5, "LDA", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0xb5] = initOpcode(0xb5, "LDA", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0xad] = initOpcode(0xad, "LDA", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0xbd] = initOpcode(0xbd, "LDA", 3, 4, ABSOLUTEX, OFFICIAL);
	opcodes[0xb9] = initOpcode(0xb9, "LDA", 3, 4, ABSOLUTEY, OFFICIAL);
	opcodes[0xa1] = initOpcode(0xa1, "LDA", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0xb1] = initOpcode(0xb1, "LDA", 2, 5, INDIRECTY, OFFICIAL);

	//LDX
	opcodes[0xa2] = initOpcode(0xa2, "LDX", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0xa6] = initOpcode(0xa6, "LDX", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0xb6] = initOpcode(0xb6, "LDX", 2, 4, ZEROPAGEY, OFFICIAL);
	opcodes[0xae] = initOpcode(0xae, "LDX", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0xbe] = initOpcode(0xbe, "LDX", 3, 4, ABSOLUTEY, OFFICIAL);

	//LDY	
	opcodes[0xa0] = initOpcode(0xa0, "LDY", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0xa4] = initOpcode(0xa4, "LDY", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0xb4] = initOpcode(0xb4, "LDY", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0xac] = initOpcode(0xac, "LDY", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0xbc] = initOpcode(0xbc, "LDY", 3, 4, ABSOLUTEX, OFFICIAL);

	//LSR
	opcodes[0x4a] = initOpcode(0x4a, "LSR", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x46] = initOpcode(0x46, "LSR", 2, 5, ZEROPAGE, OFFICIAL);
	opcodes[0x56] = initOpcode(0x56, "LSR", 2, 6, ZEROPAGEX, OFFICIAL);
	opcodes[0x4e] = initOpcode(0x4e, "LSR", 3, 6, ABSOLUTE, OFFICIAL);
	opcodes[0x5e] = initOpcode(0x5e, "LSR", 3, 7, ABSOLUTEX, OFFICIAL);

	//LXA
	opcodes[0xab] = initOpcode(0xab, "LXA", 2, 2, IMMEDIATE, UNOFFICIAL);

	//NOP
	opcodes[0xea] = initOpcode(0xea, "NOP", 1, 2, NONEADDRESSING, OFFICIAL);

	//NOP Unofficial
	opcodes[0x1a] = initOpcode(0x1a, "NOP", 1, 2, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x3a] = initOpcode(0x3a, "NOP", 1, 2, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x5a] = initOpcode(0x5a, "NOP", 1, 2, NONEADDRESSING, UNOFFICIAL);
	opcodes[0x7a] = initOpcode(0x7a, "NOP", 1, 2, NONEADDRESSING, UNOFFICIAL);
	opcodes[0xda] = initOpcode(0xda, "NOP", 1, 2, NONEADDRESSING, UNOFFICIAL);
	opcodes[0xfa] = initOpcode(0xfa, "NOP", 1, 2, NONEADDRESSING, UNOFFICIAL);


	//ORA
	opcodes[0x09] = initOpcode(0x09, "ORA", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0x05] = initOpcode(0x05, "ORA", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x15] = initOpcode(0x15, "ORA", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0x0d] = initOpcode(0x0d, "ORA", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0x1d] = initOpcode(0x1d, "ORA", 3, 4, ABSOLUTEX, OFFICIAL);
	opcodes[0x19] = initOpcode(0x19, "ORA", 3, 4, ABSOLUTEY, OFFICIAL);
	opcodes[0x01] = initOpcode(0x01, "ORA", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0x11] = initOpcode(0x11, "ORA", 2, 5, INDIRECTY, OFFICIAL);

	//Push and pull
	opcodes[0x48] = initOpcode(0x48, "PHA", 1, 3, NONEADDRESSING, OFFICIAL);
	opcodes[0x08] = initOpcode(0x08, "PHP", 1, 3, NONEADDRESSING, OFFICIAL);
	opcodes[0x68] = initOpcode(0x68, "PLA", 1, 4, NONEADDRESSING, OFFICIAL);
	opcodes[0x28] = initOpcode(0x28, "PLP", 1, 4, NONEADDRESSING, OFFICIAL);

	//RLA
	opcodes[0x27] = initOpcode(0x27, "RLA", 2, 5, ZEROPAGE, UNOFFICIAL);
	opcodes[0x37] = initOpcode(0x37, "RLA", 2, 6, ZEROPAGEX, UNOFFICIAL);
	opcodes[0x2f] = initOpcode(0x2f, "RLA", 3, 6, ABSOLUTE, UNOFFICIAL);
	opcodes[0x3f] = initOpcode(0x3f, "RLA", 3, 7, ABSOLUTEX, UNOFFICIAL);
	opcodes[0x3b] = initOpcode(0x3b, "RLA", 3, 7, ABSOLUTEY, UNOFFICIAL);
	opcodes[0x23] = initOpcode(0x23, "RLA", 2, 8, INDIRECTX, UNOFFICIAL);
	opcodes[0x33] = initOpcode(0x33, "RLA", 2, 8, INDIRECTY, UNOFFICIAL);

	//ROL
	opcodes[0x2a] = initOpcode(0x2a, "ROL", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x26] = initOpcode(0x26, "ROL", 2, 5, ZEROPAGE, OFFICIAL);
	opcodes[0x36] = initOpcode(0x36, "ROL", 2, 6, ZEROPAGEX, OFFICIAL);
	opcodes[0x2e] = initOpcode(0x2e, "ROL", 3, 6, ABSOLUTE, OFFICIAL);
	opcodes[0x3e] = initOpcode(0x3e, "ROL", 3, 7, ABSOLUTEX, OFFICIAL);

	//ROR	
	opcodes[0x6a] = initOpcode(0x6a, "ROR", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x66] = initOpcode(0x66, "ROR", 2, 5, ZEROPAGE, OFFICIAL);
	opcodes[0x76] = initOpcode(0x76, "ROR", 2, 6, ZEROPAGEX, OFFICIAL);
	opcodes[0x6e] = initOpcode(0x6e, "ROR", 3, 6, ABSOLUTE, OFFICIAL);
	opcodes[0x7e] = initOpcode(0x7e, "ROR", 3, 7, ABSOLUTEX, OFFICIAL);
	
	//RRA
	opcodes[0x67] = initOpcode(0x67, "RRA", 2, 5, ZEROPAGE, UNOFFICIAL);
	opcodes[0x77] = initOpcode(0x77, "RRA", 2, 6, ZEROPAGEX, UNOFFICIAL);
	opcodes[0x6f] = initOpcode(0x6f, "RRA", 3, 6, ABSOLUTE, UNOFFICIAL);
	opcodes[0x7f] = initOpcode(0x7f, "RRA", 3, 7, ABSOLUTEX, UNOFFICIAL);
	opcodes[0x7b] = initOpcode(0x7b, "RRA", 3, 7, ABSOLUTEY, UNOFFICIAL);
	opcodes[0x63] = initOpcode(0x63, "RRA", 2, 8, INDIRECTX, UNOFFICIAL);
	opcodes[0x73] = initOpcode(0x73, "RRA", 2, 8, INDIRECTY, UNOFFICIAL);

	//Return
	opcodes[0x40] = initOpcode(0x40, "RTI", 1, 6, NONEADDRESSING, OFFICIAL);
	opcodes[0x60] = initOpcode(0x60, "RTS", 1, 6, NONEADDRESSING, OFFICIAL);

	//SAX
	opcodes[0x87] = initOpcode(0x87, "SAX", 2, 3, ZEROPAGE, UNOFFICIAL);
	opcodes[0x97] = initOpcode(0x97, "SAX", 2, 4, ZEROPAGEY, UNOFFICIAL);
	opcodes[0x8f] = initOpcode(0x8f, "SAX", 3, 4, ABSOLUTE, UNOFFICIAL);
	opcodes[0x83] = initOpcode(0x83, "SAX", 2, 6, INDIRECTX, UNOFFICIAL);

	//SBC
	opcodes[0xe9] = initOpcode(0xe9, "SBC", 2, 2, IMMEDIATE, OFFICIAL);
	opcodes[0xe5] = initOpcode(0xe5, "SBC", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0xf5] = initOpcode(0xf5, "SBC", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0xed] = initOpcode(0xed, "SBC", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0xfd] = initOpcode(0xfd, "SBC", 3, 4, ABSOLUTEX, OFFICIAL);
	opcodes[0xf9] = initOpcode(0xf9, "SBC", 3, 4, ABSOLUTEY, OFFICIAL);
	opcodes[0xe1] = initOpcode(0xe1, "SBC", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0xf1] = initOpcode(0xf1, "SBC", 2, 5, INDIRECTY, OFFICIAL);

	//SBC unofficial
	opcodes[0xeb] = initOpcode(0xeb, "SBC", 2, 2, IMMEDIATE, UNOFFICIAL);

	//Set instructions
	opcodes[0x38] = initOpcode(0x38, "SEC", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0xf8] = initOpcode(0xf8, "SED", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x78] = initOpcode(0x78, "SEI", 1, 2, NONEADDRESSING, OFFICIAL);

	//SLO
	opcodes[0x07] = initOpcode(0x07, "SLO", 2, 5, ZEROPAGE, UNOFFICIAL);
	opcodes[0x17] = initOpcode(0x17, "SLO", 2, 6, ZEROPAGEX, UNOFFICIAL);
	opcodes[0x0f] = initOpcode(0x0f, "SLO", 3, 6, ABSOLUTE, UNOFFICIAL);
	opcodes[0x1f] = initOpcode(0x1f, "SLO", 3, 7, ABSOLUTEX, UNOFFICIAL);
	opcodes[0x1b] = initOpcode(0x1b, "SLO", 3, 7, ABSOLUTEY, UNOFFICIAL);
	opcodes[0x03] = initOpcode(0x03, "SLO", 2, 8, INDIRECTX, UNOFFICIAL);
	opcodes[0x13] = initOpcode(0x13, "SLO", 2, 8, INDIRECTY, UNOFFICIAL);

	//SRE
	opcodes[0x47] = initOpcode(0x47, "SRE", 2, 5, ZEROPAGE, UNOFFICIAL);
	opcodes[0x57] = initOpcode(0x57, "SRE", 2, 6, ZEROPAGEX, UNOFFICIAL);
	opcodes[0x4f] = initOpcode(0x4f, "SRE", 3, 6, ABSOLUTE, UNOFFICIAL);
	opcodes[0x5f] = initOpcode(0x5f, "SRE", 3, 7, ABSOLUTEX, UNOFFICIAL);
	opcodes[0x5b] = initOpcode(0x5b, "SRE", 3, 7, ABSOLUTEY, UNOFFICIAL);
	opcodes[0x43] = initOpcode(0x43, "SRE", 2, 8, INDIRECTX, UNOFFICIAL);
	opcodes[0x53] = initOpcode(0x53, "SRE", 2, 8, INDIRECTY, UNOFFICIAL);

	//STA
	opcodes[0x85] = initOpcode(0x85, "STA", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x95] = initOpcode(0x95, "STA", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0x8d] = initOpcode(0x8d, "STA", 3, 4, ABSOLUTE, OFFICIAL);
	opcodes[0x9d] = initOpcode(0x9d, "STA", 3, 5, ABSOLUTEX, OFFICIAL);
	opcodes[0x99] = initOpcode(0x99, "STA", 3, 5, ABSOLUTEY, OFFICIAL);
	opcodes[0x81] = initOpcode(0x81, "STA", 2, 6, INDIRECTX, OFFICIAL);
	opcodes[0x91] = initOpcode(0x91, "STA", 2, 6, INDIRECTY, OFFICIAL);

	//STX
	opcodes[0x86] = initOpcode(0x86, "STX", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x96] = initOpcode(0x96, "STX", 2, 4, ZEROPAGEY, OFFICIAL);
	opcodes[0x8e] = initOpcode(0x8e, "STX", 3, 4, ABSOLUTE, OFFICIAL);

	//STY
	opcodes[0x84] = initOpcode(0x84, "STY", 2, 3, ZEROPAGE, OFFICIAL);
	opcodes[0x94] = initOpcode(0x94, "STY", 2, 4, ZEROPAGEX, OFFICIAL);
	opcodes[0x8c] = initOpcode(0x8c, "STY", 3, 4, ABSOLUTE, OFFICIAL);

	//SXA
	opcodes[0x9e] = initOpcode(0x9e, "SXA", 3, 5, ABSOLUTEY, UNOFFICIAL);

	//SYA
	opcodes[0x9c] = initOpcode(0x9c, "SYA", 3, 5, ABSOLUTEX, UNOFFICIAL);

	//TAS
	opcodes[0x9b] = initOpcode(0x9b, "TAS", 3, 5, ABSOLUTEY, UNOFFICIAL);;

	//Transfer
	opcodes[0xaa] = initOpcode(0xaa, "TAX", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0xa8] = initOpcode(0xa8, "TAY", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0xba] = initOpcode(0xba, "TSX", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x8a] = initOpcode(0x8a, "TXA", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x9a] = initOpcode(0x9a, "TXS", 1, 2, NONEADDRESSING, OFFICIAL);
	opcodes[0x98] = initOpcode(0x98, "TYA", 1, 2, NONEADDRESSING, OFFICIAL);

	//TOP (triple NOP)
	opcodes[0x0c] = initOpcode(0x0c, "NOP", 3, 4, ABSOLUTE, UNOFFICIAL);
	opcodes[0x1c] = initOpcode(0x1c, "NOP", 3, 4, ABSOLUTEX, UNOFFICIAL);
	opcodes[0x3c] = initOpcode(0x3c, "NOP", 3, 4, ABSOLUTEX, UNOFFICIAL);
	opcodes[0x5c] = initOpcode(0x5c, "NOP", 3, 4, ABSOLUTEX, UNOFFICIAL);
	opcodes[0x7c] = initOpcode(0x7c, "NOP", 3, 4, ABSOLUTEX, UNOFFICIAL);
	opcodes[0xdc] = initOpcode(0xdc, "NOP", 3, 4, ABSOLUTEX, UNOFFICIAL);
	opcodes[0xfc] = initOpcode(0xfc, "NOP", 3, 4, ABSOLUTEX, UNOFFICIAL);

	//XXA
	opcodes[0x8b] = initOpcode(0x8b, "XXA", 2, 2, IMMEDIATE, UNOFFICIAL);
}

/*int main(){
	struct Opcode opcodes[0x100];
	createOpArray(opcodes);
	printf("%s\n", opcodes[0xba].name);
	return 0;
}*/
