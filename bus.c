#include "bus.h"

/*
struct Bus{
	unsigned char prgRam[2048];
	struct Rom rom;
	struct PPU ppu;
};
*/
unsigned char busRead(struct Bus *bus, unsigned short addr){
	if(addr >= 0x0 && addr < 0x2000){
		addr &= 0b11111111111;
		return bus->prgRam[addr];
	}

	else if(addr == 0x2000 || addr == 0x2001 || addr == 0x2003 || addr == 0x2006 || addr == 0x4014){
		//printf("Reading from write only PPU memory at %x\n", addr);
		return 0;
	}

	else if(addr == 0x2002){
		return statusRead(bus->ppu);
	}

	else if(addr == 0x2004){
		return bus->ppu->oamData[bus->ppu->oamAddr];
	}

	else if(addr == 0x2007){
		return ppuBusRead(bus->ppu);
	}

	else if(addr >= 0x2008 && addr < 0x4000){
		addr %= 8;
		addr += 2000;	//addresses 0x2000-0x2007 are mirrored between 0x2008-0x3fff
		busRead(bus, addr);
	}

	else if(addr == 0x4016){
		printf("reading from 0x4016\n");
		return joypadRead(bus->joypad);
	}

	else if(addr >= 0x4000 && addr < 0x401f){	
		//printf("APU not supported yet");
		return 0;
	}

	else if(addr >= 0x8000 && addr <= 0x10000){
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

	else if(addr == 0x2000){
		writeToCtrl(bus->ppu, data);
	}

	else if(addr == 0x2001){
		bus->ppu->mask = data;
	}

	else if(addr == 0x2002){
		//printf("trying to write to status register, but status register is read only\n");
	}

	else if(addr == 0x2003){
		writeToOamAddr(bus->ppu, data);
	}

	else if(addr == 0x2004){
		writeToOamData(bus->ppu, data);
	}

	else if(addr == 0x2005){
		writeToScroll(bus->ppu, data);
	}

	else if(addr == 0x2006){
		updateAddrRegister(&(bus->ppu->addr), data);
	}

	else if(addr == 0x2007){
		ppuWrite(bus->ppu, data);
	}

	else if(addr >= 0x2008 && addr < 0x4000){
		addr %= 8;
		addr += 2000;	//addresses 0x2000-0x2007 are mirrored between 0x2008-0x3fff
		busWrite(bus, addr, data);
	}

	else if(addr == 0x4014){
		unsigned short addr = 0x100 * (unsigned short)data;
		//printf("bulk write addr = %x\n", addr);
		for(int i = 0; i < 256; i++){
			bus->ppu->oamData[bus->ppu->oamAddr++] = busRead(bus, addr++);
			//printf("bus->ppu->oamData[bus->ppu->oamAddr] = %x\n", bus->ppu->oamData[bus->ppu->oamAddr - 1]);
		}
	}

	else if(addr >= 0x4000 && addr < 0x401f){	
		//printf("APU not supported yet\n");
	}

	else if(addr >= 0x8000 && addr <= 0x10000){
		printf("Trying to write to ROM at %x\n", addr);
	}
}

void busTick(struct Bus *bus, int cycleCount){
	ppuTick(bus->ppu, cycleCount * 3);
}
