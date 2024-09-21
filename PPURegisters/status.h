#ifndef STATUS_H
#define STATUS_H

#include <stdio.h>
#include <stdlib.h>

unsigned char statusRead(unsigned char *status);
void statusVblankOn(unsigned char *status);
void statusVblankOff(unsigned char *status);

#endif
