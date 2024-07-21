#ifndef OPCODE_H
#define OPCODE_H

#define IMMEDIATE 1
#define ZEROPAGE 2
#define ZEROPAGEX 3
#define ZEROPAGEY 4
#define ABSOLUTE 5
#define ABSOLUTEX 6
#define ABSOLUTEY 7
#define INDIRECTX 8
#define INDIRECTY 9
#define NONEADDRESSING 0

struct Opcode{
	unsigned char code;
	char name[3];
	int len;
	int cycles;
	int addressingMode;
};

#endif
