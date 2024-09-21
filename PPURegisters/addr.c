#include "addr.h"

void initAddrRegister(struct AddrRegister *addr){
	memset(addr->value, 0, sizeof(addr->value));
	addr->hiByte = 1;
}

unsigned short getAddrRegister(struct AddrRegister *addr){ 
	unsigned short fullAddr = ((addr->value[0] << 8) | addr->value[1]);
	return fullAddr;
}

void setAddrRegister(struct AddrRegister *addr, unsigned short data){
	addr->value[0] = (data >> 8);
	addr->value[1] = data & 0xff;
}

void incrementAddrRegister(struct AddrRegister *addr){
	unsigned char post = addr->value[1]++;
	if(post < addr->value[1]){
		addr->value[1]++;
		addr->value[0]++;
	}
	else{
		addr->value[1]++;
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

