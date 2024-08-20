#include "mask.h"

int isGreyScaleOn(unsigned char mask){
	return (mask & GREYSCALE);
}

int isBackgroundRendOn(unsigned char mask){
	return (mask & SHOWBACKGROUND) >> 3;
}

int isSpriteRendOn(unsigned char mask){
	return (mask & SHOWSPRITES) >> 4;
}

int isLeftBackgroundRendOn(unsigned char mask){
	return (mask & SHOWBACKGROUNDLEFT) >> 1;
}

int isLeftSpriteRendOn(unsigned char mask){
	return (mask & SHOWSPRITESLEFT) >> 2;
}

int colorMode(unsigned char mask){
	mask &= 11100000;
	switch(mask){
		case EMPHASIZERED:{
			return REDEMPHASIS;
		}

		case EMPHASIZEGREEN:{
			return GREENEMPHASIS;
		}

		case EMPHASIZEBLUE:{
			return BLUEEMPHASIS;
		}
		default:{
			printf("Multiple color modes are on\nmask = %b\n", mask);
			break;
		}
	}
}
