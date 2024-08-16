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

void initAddrRegister(struct AddrRegister *addr){
	memset(addr->value, 0, sizeof(addr->value));
	addr->hiByte = 1;
}

unsigned short getAddrRegister(struct AddrRegister *addr){ 
	unsigned short fullAddr = ((addr->value[0] << 8) | addr->val[1]);
	return fullAddr;
}

void setAddrRegister(struct AddrRegister *addr, unsigned short data){
	addr->value[0] = (data >> 8);
	addr->value[1] = data & 0xff;
}

void updateAddrRegister(struct AddrRegister *addr, unsigned char data){
	if(addr->hiByte){
		addr->value[0] = data;
	}
	else{
		addr->value[1] = data;
	}

	if(getAddrRegister(addr) > 0x3fff){
		setAddrRegister(addr, getAddrRegister(addr) & 0b11111111111111);	//mirror down addr above 0x3fff
	}
}

