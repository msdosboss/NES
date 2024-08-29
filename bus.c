#include "bus.h"

/*
struct Bus{
	unsigned char prgRam[2048];
	struct Rom rom;
};
*/
unsigned char busRead(struct Bus *bus, unsigned short addr){
	if(addr >= 0x0 && addr < 0x2000){
		addr &= 0b11111111111;
		return bus->prgRam[addr];
	}

	if(addr >= 0x2000 && addr < 0x4000){
		addr %= 8;
		addr += 2000;	//addresses 0x2000-0x2007 are mirrored between 0x2008-0x3fff
		return bus->prgRam[addr];
	}

	if(addr >= 0x4000 && addr < 0x401f){	
		printf("APU not supported yet");
		return 0;
	}

	if(addr >= 0x8000 && addr <= 0x10000){
		addr -= 0x8000;
		if(addr >= 0x4000 && bus->rom.prgRomLen == 0x4000){
			return bus->rom.prgRom[addr % 0x4000];
		}
		return bus->rom.prgRom[addr];
	}
}

void busWrite(struct Bus *bus, unsigned short addr, unsigned char data){
	if(addr >= 0x0 && addr < 0x2000){
		addr &= 0b11111111111;
		bus->prgRam[addr] = data;
	}

	if(addr >= 0x2000 && addr < 0x4000){
		addr %= 8;
		addr += 2000;	//addresses 0x2000-0x2007 are mirrored between 0x2008-0x3fff
		bus->prgRam[addr] = data;
	}

	if(addr >= 0x4000 && addr < 0x401f){	
		printf("APU not supported yet");
	}

	if(addr >= 0x8000 && addr <= 0x10000){
		printf("Trying to write to ROM at %x\n", addr);
	}
}
