#include "controller.h"

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

int vramAddrIncAmount(unsigned char ppuController){
	if((ppuController & VRAMINCAMOUNT) != 0){
		return 32;
	}
	else{
		return 1;
	}
}

int isNMIIntOn(unsigned char ppuController){
	return ppuController & 0b10000000;
}

unsigned short nametableAddr(unsigned char ppuController){
	unsigned char result = ((ppuController & 0b00000010) | (ppuController & 0b00000001));

	switch(result){
		case 0:
			return 0x2000;
			break;
		case 1:
			return 0x2400;
			break;
		case 2:
			return 0x2800;
			break;
		case 3:
			return 0x2c00;
			break;
	}
}
