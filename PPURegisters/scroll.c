#include "scroll.h"

void scrollWrite(struct ScrollRegister *scroll, unsigned char data){
	if(!scroll->latch){
		scroll->scrollX = data;
	}
	else{
		scroll->scrollY = data;
	}

	scroll->latch = scroll->latch ? 0 : 1;
}

void scrollLatch(struct ScrollRegister *scroll){
	scroll->latch = 0;
}

void initScrollRegister(struct ScrollRegister *scroll){
	scroll->scrollX = 0;
	scroll->scrollY = 0;
	scroll->latch = 0;
}
