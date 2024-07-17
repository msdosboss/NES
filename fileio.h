#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FOURSCREEN 2
#define VERTICAL 1
#define HORIZONTAL 0

#define PRGROMPAGESIZE 16384
#define CHRROMPAGESIZE 8192

struct Rom{
	int len;
	unsigned char *prgRom;
	unsigned char *chrRom;
	unsigned char mapper;
	int mirrorMode;
};
int instructionCount(char *str);
unsigned char hexToChar(char *hexVal);
struct Rom parse(char *str);
struct Rom loadRom(char *fileName);

#endif
