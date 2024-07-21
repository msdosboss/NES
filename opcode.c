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

struct Opcode *createOpArray(){
	struct Opcode opcodes[0x100];
	//LDA
	opcodes[0x69] = {0x69, "LDA", 2, 2, IMMEDIATE};
	opcodes[0x65] = {0x65, "LDA", 2, 3, ZEROPAGE};
	opcodes[0x75] = {0x75, "LDA", 2, 4, ZEROPAGEX};
	opcodes[0x6d] = {0x6d, "LDA", 3, 4, ABSOLUTE};
	opcodes[0x7d] = {0x7d, "LDA", 3, 4, ABSOLUTEX};
	opcodes[0x79] = {0x79, "LDA", 3, 4, ABSOLUTEY};
	opcodes[0x61] = {0x61, "LDA", 2, 6, INDIRECTX};
	opcodes[0x71] = {0x71, "LDA", 2, 5, INDIRECTY};

	//AND
	opcodes[0x29] = {0x29, "AND", 2, 2, IMMEDIATE};
	opcodes[0x25] = {0x25, "AND", 2, 3, ZEROPAGE};
	opcodes[0x35] = {0x35, "AND", 2, 4, ZEROPAGEX};
	opcodes[0x2d] = {0x2d, "AND", 3, 4, ABSOLUTE};
	opcodes[0x3d] = {0x3d, "AND", 3, 4, ABSOLUTEX};
	opcodes[0x39] = {0x39, "AND", 3, 4, ABSOLUTEY};
	opcodes[0x21] = {0x21, "AND", 2, 6, INDIRECTX};
	opcodes[0x31] = {0x31, "AND", 2, 5, INDIRECTY};

	//ASL
	opcodes[0x0a] = {0x0a, "ASL", 2, 2, NONEADDRESSING};
	opcodes[0x06] = {0x06, "ASL", 2, 5, ZEROPAGE};
	opcodes[0x16] = {0x16, "ASL", 2, 6, ZEROPAGEX};
	opcodes[0x0e] = {0x0e, "ASL", 3, 6, ABSOLUTE};
	opcodes[0x1e] = {0x1e, "ASL", 3, 7, ABSOLUTEX};

	//Branch instructions
	opcodes[0x90] = {0x90, "BCC", 2, 2, NONEADDRESSING};
	opcodes[0xb0] = {0xb0, "BCS", 2, 2, NONEADDRESSING};
	opcodes[0xf0] = {0xf0, "BEQ", 2, 2, NONEADDRESSING};
	opcodes[0x30] = {0x30, "BMI", 2, 2, NONEADDRESSING};
	opcodes[0xd0] = {0xd0, "BNE", 2, 2, NONEADDRESSING};
	opcodes[0x10] = {0x10, "BPL", 2, 2, NONEADDRESSING};
	opcodes[0x50] = {0x50, "BVC", 2, 2, NONEADDRESSING};
	opcodes[0x70] = {0x70, "BVS", 2, 2, NONEADDRESSING};

	//BIT
	opcodes[0x24] = {0x24, "BIT", 2, 3, ZEROPAGE};
	opcodes[0x2c] = {0x2c, "BIT", 3, 4, ABSOLUTE};

	//BRK
	opcodes[0x00] = {0x00, "BRK", 1, 7, NONEADDRESSING};

	//Clear instuctions
	opcodes[0x18] = {0x18, "CLC", 1, 2, NONEADDRESSING};
	opcodes[0xd8] = {0xd8, "CLD", 1, 2, NONEADDRESSING};
	opcodes[0x58] = {0x58, "CLI", 1, 2, NONEADDRESSING};
	opcodes[0xb8] = {0xb8, "CLV", 1, 2, NONEADDRESSING};

	//CMP
	opcodes[0xc9] = {0xc9, "CMP", 2, 2, IMMEDIATE};
	opcodes[0xc5] = {0xc5, "CMP", 2, 3, ZEROPAGE};
	opcodes[0xd5] = {0xd5, "CMP", 2, 4, ZEROPAGEX};
	opcodes[0xcd] = {0xcd, "CMP", 3, 4, ABSOLUTE};
	opcodes[0xdd] = {0xdd, "CMP", 3, 4, ABSOLUTEX};
	opcodes[0xd9] = {0xd9, "CMP", 3, 4, ABSOLUTEY};
	opcodes[0xc1] = {0xc1, "CMP", 2, 6, INDIRECTX};
	opcodes[0xd1] = {0xd1, "CMP", 2, 5, INDIRECTY};

	//CPX
	opcodes[0xe0] = {0xe0, "CPX", 2, 2, IMMEDIATE};
	opcodes[0xe4] = {0xe4, "CPX", 2, 3, ZEROPAGE};
	opcodes[0xec] = {0xec, "CPX", 3, 4, ABSOLUTE};

	//CPY	
	opcodes[0xc0] = {0xc0, "CPX", 2, 2, IMMEDIATE};
	opcodes[0xc4] = {0xc4, "CPX", 2, 3, ZEROPAGE};
	opcodes[0xcc] = {0xcc, "CPX", 3, 4, ABSOLUTE};

	return opcodes;
}
