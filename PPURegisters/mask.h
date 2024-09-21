#ifndef MASK_H
#define MASK_H

#include <stdlib.h>
#include <stdio.h>

#define GREYSCALEMODE 0
#define REDEMPHASIS   1
#define GREENEMPHASIS 2
#define BLUEEMPHASIS  3

#define GREYSCALE          0b00000001
#define SHOWBACKGROUNDLEFT 0b00000010
#define SHOWSPRITESLEFT    0b00000100
#define SHOWBACKGROUND     0b00001000
#define SHOWSPRITES        0b00010000
#define EMPHASIZERED       0b00100000
#define EMPHASIZEGREEN     0b01000000
#define EMPHASIZEBLUE      0b10000000

int isGreyScaleOn(unsigned char mask);
int isBackgroundRendOn(unsigned char mask);
int isSpriteRendOn(unsigned char mask);
int isLeftBackgroundRendOn(unsigned char mask);
int isLeftSpriteRendOn(unsigned char mask);
int colorMode(unsigned char mask);

#endif
