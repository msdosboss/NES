#include "ppu.h"

/*
struct PPU{
	unsigned char *chrRom;
	unsigned char paletteTable[32];
	unsigned char vram[2048];
	unsigned char oamData[256];
	int mirrorMode;

	struct AddrRegister addr;
	unsigned char controller;
	unsigned char dataBuffer;
};
*/

void initPPU(struct PPU *ppu, struct Rom *rom){
	ppu->chrRom = rom->chrRom;
	memset(ppu->paletteTable, 0, sizeof(ppu->paletteTable));
	memset(ppu->vram, 0, sizeof(ppu->vram));
	memset(ppu->oamData, 0, sizeof(ppu->oamData));
	ppu->mirrorMode = rom->mirrorMode;
}

unsigned char ppuRead(struct PPU *ppu){
	unsigned short address = getAddrRegister(&(ppu->addr));
	for(int i = 0; i < vramAddrIncAmount(ppu->controller); i++){
		incrementAddrRegister(&(ppu->addr));
	}
	if(address >= 0x0 && address <= 0x1fff){
		unsigned char data = ppu->dataBuffer;
		ppu->dataBuffer = chrRom[addr];
		return data;
	}

	else if(address >= 0x2000 && address <= 0x2fff){
		unsigned char data = ppu->dataBuffer;
		ppu->dataBuffer = ppu->vram[addr - 0x2000];	//NEED TO IMPLEMENT MIRRORING!!!!
		return data;
	}

	else if(address >= 0x3000 && addresss <= 0x3eff){
		printf("addr space %x is not suppose to be used!\n", address);
	}

	else if(address >= 0x3f00 && address <= 0x3fff){
		return ppu->paletteTable[address - 0x3f00];
	}
}

unsigned short mirroredVramAddr(unsigned short address){
	unsigned short mirroredVram = address & 0b10111111111111;	//mirror down 0x3000-0x3eff to 0x2000-0x20eff
	mirroredVram -= 0x2000;
	unsigned short nameTable = mirroredVram / 0x400	//Name table index 

	if(ppu->mirrorMode == HORIZONTAL){
		if(nameTable == 1 || nameTable == 2){
			mirroredVram -= 0x400;
		}
		if(nameTable == 3){
			mirroredVram -= 0x800;
		}
	}
	else if(ppu->mirrorMode == VERTICAL){
		if(nameTable == 2 || nameTable == 3){
			mirroredVram -= 0x800;
		}
	}

	return mirroredVram;
}
