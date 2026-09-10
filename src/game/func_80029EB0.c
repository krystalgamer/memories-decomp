#include "../types.h"
#include "func_80029EB0.h"

unsigned int func_80029EB0(unsigned char *base, int index)
{
    base += index * 4;
    return base[0x56];
}
