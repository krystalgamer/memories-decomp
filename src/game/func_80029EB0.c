#include "../types.h"
#include "func_80029EB0.h"

unsigned int func_80029EB0(unsigned char *base, int index)
{
    base = (u8 *)&((u32 *)base)[index];
    return base[0x56];
}
