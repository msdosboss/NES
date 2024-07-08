#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Rom{
	int len;
	unsigned char *hexVals;
};
int instructionCount(char *str);
unsigned char hexToChar(char *hexVal);
struct Rom parse(char *str);
struct Rom loadRom(char *fileName);

#endif
