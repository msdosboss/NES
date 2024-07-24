#ifndef LOG_H
#define LOG_H
#include "6502.h"
#include "opcode.h"
#include "fileio.h"
#include <stdlib.h>
#include <stdio.h>

void cycleLog(struct CPU cpu, struct Opcode opcode, char *str);
char upper(char lower);
#endif

