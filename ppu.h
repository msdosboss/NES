#ifndef PPU_H
#define PPU_H

#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"
#include "PPURegisters/addr.h"
#include "PPURegisters/controller.h"
#include "PPURegisters/mask.h"
#include "PPURegisters/status.h"
#include "PPURegisters/scroll.h"

struct PPU{
	unsigned char *chrRom;
	unsigned char paletteTable[32];
	unsigned char vram[2048];
	unsigned char oamAddr;
	unsigned char oamData[256];
	int mirrorMode;
	struct AddrRegister addr;
	struct ScrollRegister scroll;
	unsigned char status;
	unsigned char controller;
	unsigned char mask;
	unsigned char dataBuffer;
	int scanLines;
	int cycles;
	int nmiInt;
};

struct ViewableRect{
	int x1;
	int y1;
	int x2;
	int y2;
};

struct Tile{
	unsigned char pixels[8][8];
};

struct Frame{
	struct Tile tiles[30][32];
};

void initPPU(struct PPU *ppu, struct Rom *rom);
int isSprite0Hit(struct PPU *ppu, int cycles);
int ppuTick(struct PPU *ppu, int cycles);
unsigned char ppuRead(struct PPU *ppu);
unsigned char ppuBusRead(struct PPU *ppu);
void ppuWrite(struct PPU *ppu, unsigned char data);
unsigned short mirroredVramAddr(struct PPU *ppu, unsigned short address);
unsigned char statusRead(struct PPU *ppu);
void writeToCtrl(struct PPU *ppu, unsigned char data);
void writeToOamAddr(struct PPU *ppu, unsigned char data);
void writeToOamData(struct PPU *ppu, unsigned char data);
void writeToScroll(struct PPU *ppu, unsigned char data);
//void setPixel(struct Pixel *pixel, struct PaletteEntry paletteEntry);
void parseChrRom(struct PPU *ppu, struct Frame *frame, int bank);
int bgPalette(struct PPU *ppu, unsigned char *nameTable, int hor, int ver);
int spritePalette(unsigned char paletteIndex);
void parseNametable(struct PPU *ppu, struct Frame *frame, unsigned char *nameTable, struct ViewableRect rect, int shiftX, int shiftY);
void parseVram(struct PPU *ppu, struct Frame *frame);
struct Frame createFrame();
#endif
