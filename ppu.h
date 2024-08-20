#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"
#include "addr.h"
#include "controller.h"
#include "mask.h"

struct PPU{
	unsigned char *chrRom;
	unsigned char paletteTable[32];
	unsigned char vram[2048];
	unsigned char oamData[256];
	int mirrorMode;
};

void initPPU(struct PPU *ppu, struct Rom *rom);
