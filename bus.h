#include <stdio.h>
#include "ppu.h"
#include "fileio.h"

struct Bus{
	unsigned char prgRam[2048];
	struct Rom rom;
	struct PPU *ppu;
};
unsigned char busRead(struct Bus *bus, unsigned short addr);
void busWrite(struct Bus *bus, unsigned short addr, unsigned char data);
