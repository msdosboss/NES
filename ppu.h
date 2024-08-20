#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"

#define NAMETABLE1 0b00000001
#define NAMETABLE2 0b00000010
#define VRAM_ADD_INCREMENT 0b00000100
#define SPRITE_PATTERN_ADDR 0b00001000
#define BACKROUND_PATTERN_ADDR 0b00010000
#define SPRITE_SIZE 0b00100000
#define MASTER_SLAVE_SELECT 0b01000000
#define GENERATE_NMI 0b10000000

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
