#include "ppu.h"

/*
struct PPU{
	unsigned char *chrRom;
	unsigned char paletteTable[32];
	unsigned char vram[2048];
	unsigned char oamData[256];
	int mirrorMode;
};
*/

void initPPU(struct PPU *ppu, struct Rom *rom){
	ppu->chrRom = rom->chrRom;
	memset(ppu->paletteTable, 0, sizeof(ppu->paletteTable));
	memset(ppu->vram, 0, sizeof(ppu->vram));
	memset(ppu->oamData, 0, sizeof(ppu->oamData));
	ppu->mirrorMode = rom->mirrorMode;
}
