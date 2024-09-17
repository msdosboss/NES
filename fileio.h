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
	long prgRomLen;
	unsigned char *chrRom;
	unsigned char mapper;
	int mirrorMode;
};

struct PaletteEntry{
	unsigned char red;
	unsigned char blue;
	unsigned char green;
};
int instructionCount(char *str);
unsigned char hexToChar(char *hexVal);
struct Rom parse(char *str);
struct Rom loadRom(char *fileName);
int verifyFormat(unsigned char *raw);
void mirrorMode(struct Rom *rom, unsigned char *raw);
int trainerFlag(unsigned char *raw);
struct Rom nesCartRead(char *fileName);

#endif
