#include "../types.h"
#include "func_80029EB0.h"

#define CARD_FLAG_RECORD_BYTES(base, index) \
    ((u8 *)&((u32 *)base)[index])

unsigned int Library_GetCardFlags(unsigned char *base, int index)
{
    base = CARD_FLAG_RECORD_BYTES(base, index);
    return base[0x56];
}
