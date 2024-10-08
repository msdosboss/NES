#ifndef ADDR_H
#define ADDR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct AddrRegister{
	unsigned char value[2];
	int hiByte;
};

void initAddrRegister(struct AddrRegister *addr);
unsigned short getAddrRegister(struct AddrRegister *addr);
void setAddrRegister(struct AddrRegister *addr, unsigned short data);
void incrementAddrRegister(struct AddrRegister *addr);
void updateAddrRegister(struct AddrRegister *addr, unsigned char data);
void addrLatch(struct AddrRegister *addr);

#endif
