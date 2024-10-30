#ifndef SCROLL_H
#define SCROLL_H

struct ScrollRegister{
	int scrollY;
	int scrollX;
	int latch;
};

void scrollWrite(struct ScrollRegister *scroll, unsigned char data);
void scrollLatch(struct ScrollRegister *scroll);
void initScrollRegister(struct ScrollRegister *scroll);

#endif
