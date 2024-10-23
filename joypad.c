#include "joypad.h"

void initJoypad(struct Joypad *joypad){
	joypad->strobe = 0;
	joypad->buttonIndex = 0;
	joypad->buttonStatus = 0;
}

void joypadWrite(struct Joypad *joypad, unsigned char val){
	joypad->strobe = val;
	if(joypad->strobe){
		joypad->buttonIndex = 0;
	}
}

unsigned char joypadRead(struct Joypad *joypad){
	if(joypad->buttonIndex > 7){
		return 1;
	}

	unsigned char result = ((joypad->buttonStatus & (1 << joypad->buttonIndex)) >> joypad->buttonIndex);

	if(!joypad->strobe && joypad->buttonIndex < 7){
		joypad->buttonIndex++;
	}

	printf("result = %d\n", result);

	return result;
}



