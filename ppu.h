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
	unsigned char mask;
	unsigned char scroll;
	unsigned char dataBuffer;
};

void initPPU(struct PPU *ppu, struct Rom *rom);
unsigned char ppuRead(struct PPU *ppu);
void ppuWrite(struct PPU *ppu, unsigned char data);
unsigned short mirroredVramAddr(unsigned short address);
void writeToCtrl(struct PPU *ppu, unsigned char data);
void writeToOamAddr(struct PPU *ppu, unsigned char data);
void writeToOamData(struct PPU *ppu, unsigned char data);
void writeToScroll(struct PPU *ppu, unsigned char data);
