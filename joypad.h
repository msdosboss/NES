#ifndef JOYPAD_H
#define JOYPAD_H

#include <stdlib.h>
#include <stdio.h>

#define RIGHTDPAD 0b10000000
#define LEFTDPAD 0b01000000
#define DOWNDPAD 0b00100000
#define UPDPAD 0b00010000
#define STARTBUTTON 0b00001000
#define SELECTBUTTON 0b00000100
#define ABUTTON 0b00000010
#define BBUTTON 0b00000001

struct Joypad{
	int strobe;
	unsigned char buttonIndex;
	unsigned char buttonStatus;
};

void initJoypad(struct Joypad *joypad);
void joypadWrite(struct Joypad *joypad, unsigned char val);
unsigned char joypadRead(struct Joypad *joypad);

#endif
