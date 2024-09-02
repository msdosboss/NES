#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"
#include "PPURegisters/addr.h"
#include "PPURegisters/controller.h"
#include "PPURegisters/mask.h"
#include "PPURegisters/status.c"

struct PPU{
	unsigned char *chrRom;
	unsigned char paletteTable[32];
	unsigned char vram[2048];
	unsigned char oamAddr;
	unsigned char oamData[256];
	int mirrorMode;
	struct AddrRegister addr;
	unsigned char status;
	unsigned char controller;
	unsigned char dataBuffer;
};

void initPPU(struct PPU *ppu, struct Rom *rom);
unsigned char ppuRead(struct PPU *ppu);
unsigned short mirroredVramAddr(unsigned short address);