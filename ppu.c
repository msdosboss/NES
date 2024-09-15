#include "ppu.h"

/*
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

	int scanLines;
	int cycles;

	int nmiInt;
};
*/

void initPPU(struct PPU *ppu, struct Rom *rom){
	ppu->chrRom = rom->chrRom;
	memset(ppu->paletteTable, 0, sizeof(ppu->paletteTable));
	memset(ppu->vram, 0, sizeof(ppu->vram));
	memset(ppu->oamData, 0, sizeof(ppu->oamData));
	ppu->mirrorMode = rom->mirrorMode;
	ppu->cycles = 0;
	ppu->scanLines = 0;
}

int ppuTick(struct PPU *ppu, int cycles){
	ppu->cycles += cycles;
	if(ppu->cycles >= 341){
		ppu->cycles -= 341;
		ppu->scanLines += 1;

		if(scanLines == 241){
			if(isNMIIntOn(ppu->controller)){
				statusVblankOn(&(ppu->status));
				ppu->nmiInt = 1;
			}
		}
		if(ppu->scanLines >= 262){
			ppu->scanLines = 0;
			ppu->nmiInt = 0;
			statusVblankOff(&(ppu->status));
			return 1;
		}
	}
	return 0;
	
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
		return -1;
	}

	else if(address >= 0x3f00 && address <= 0x3fff){
		address %= 0x20;
		address += 0x3f00;
		return ppu->paletteTable[address - 0x3f00];
	}
}

void ppuWrite(struct PPU *ppu, unsigned char data){
	unsigned short address = getAddrRegister(&(ppu->addr));
	for(int i = 0; i < vramAddrIncAmount(ppu->controller); i++){
		incrementAddrRegister(&(ppu->addr));
	}

	if(address >= 0x0 && address < 0x2000){
		printf("Trying to write to ROM at address %x", address);
	}

	else if(address >= 0x2000 && < 0x2fff){
		ppu->vram[addr - 0x2000] = data;
	}

	else if(address >= 0x3000 && addresss < 0x3f00){
		printf("addr space %x is not suppose to be used!\n", address);
	}

	else if(address >= 0x3f00 && address < 0x4000){
		address %= 0x20;
		address += 0x3f00;
		ppu->paletteTable[addr - 0x3f00] = data;
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

void writeToCtrl(struct PPU *ppu, unsigned char data){
	int beforeNmiStatus = ppu->controller & 0b10000000;
	ppu->controller = data;

	if(!beforeNmiStatus && (ppu->controller & 0b10000000) && (ppu->status & 0b10000000)){	//checking if the controller register has the vblank flag on and checking if the status register has generate NMI flag on
		ppu->nmiInt = 1;
	}
}

void writeToOamAddr(struct PPU *ppu, unsigned char data){
	ppu->oamAddr = data;
}

void writeToOamData(struct PPU *ppu, unsigned char data){
	ppu->oamData[ppu->oamAddr] = data;
	ppu->oamAddr++;
}

void writeToScroll(struct PPU *ppu, unsigned char data){
	ppu->scroll = data;
}
