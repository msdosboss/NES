#include <stdio.h>

struct Bus{
	unsigned char cpuRam[2048];
};
unsigned char busRead(struct Bus *bus, unsigned short addr);
void memWrite(struct Bus *bus, unsigned short addr, unsigned char data);
