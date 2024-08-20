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

   // 7  bit  0
   // ---- ----
   // VPHB SINN
   // |||| ||||
   // |||| ||++- Base nametable address
   // |||| ||    (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
   // |||| |+--- VRAM address increment per CPU read/write of PPUDATA
   // |||| |     (0: add 1, going across; 1: add 32, going down)
   // |||| +---- Sprite pattern table address for 8x8 sprites
   // ||||       (0: $0000; 1: $1000; ignored in 8x16 mode)
   // |||+------ Background pattern table address (0: $0000; 1: $1000)
   // ||+------- Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
   // |+-------- PPU master/slave select
   // |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
   // +--------- Generate an NMI at the start of the
   //            vertical blanking interval (0: off; 1: on)

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

void incrementAddrRegister(struct AddrRegister *addr){
	int incAmount;
	if((busRead(&(ppu->bus), 0x2000) & VRAM_ADD_INCREMENT)){	//0x2000 is the control register
		incAmount = 32;	
	}
	else{
		incAmount = 1;
	}
	unsigned char post = addr.value[1] + incAmount;
	if(post < addr.value[1]){
		addr.value[1] += incAmount;
		addr.value[0] += 1;
	}
	else{
		addr.value[1] += incAmount;
	}
	if(getAddrRegister(addr) > 0x3fff){
		setAddrRegister(addr, getAddrRegister(addr) & 0b11111111111111);	//mirror down addr above 0x3fff
	}

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

