#include "bus.h"

/*
struct Bus{
	unsigned char prgRam[2048];
};
*/
unsigned char busRead(struct Bus *bus, unsigned short addr){
	if(addr >= 0x0 && addr < 0x2000){
		addr &= 0b11111111111;
		return bus->prgRam[addr];
	}
	if(addr >= 0x2000 && addr < 0x4000){
		printf("PPU not supported yet");
		return 0;
	}
}

void busWrite(struct Bus *bus, unsigned short addr, unsigned char data){
	if(addr >= 0x0 && addr < 0x2000){
		addr &= 0b11111111111;
		bus->prgRam[addr] = data;
	}
	if(addr >= 0x2000 && addr < 0x4000){
		printf("PPU not supported yet");
	}
}
