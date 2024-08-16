#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"

struct AddrRegister{
	unsigned char value[2];
	int hiByte;
};

struct PPU{
	unsigned char *chrRom;
	unsigned char paletteTable[32];
	unsigned char vram[2048];
	unsigned char oamData[256];
	int mirrorMode;
};
