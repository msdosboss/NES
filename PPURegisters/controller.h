#include <stdlib.h>

#define NAMETABLE1        0b00000001
#define NAMETABLE2        0b00000010
#define VRAMINCAMOUNT     0b00000100
#define SPRITETABLE       0b00001000
#define BACKGROUNDTABLE   0b00010000
#define SPRITESIZE        0b00100000
#define MASTERSLAVESELECT 0b01000000
#define NMIFLAG           0b10000000

int vramAddrIncAmount(unsigned char ppuController);
