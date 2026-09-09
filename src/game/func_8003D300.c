#include "../types.h"
#include "duel_effect.h"

typedef struct {
    unsigned char field0;
    unsigned char pad[9];
} LargeObject;

extern LargeObject D_800EF6E0;
extern u8 D_8009B3C7;
extern unsigned char D_8009B3CF;
extern unsigned char D_8009B3DD;
extern unsigned short D_8009B3CC;

void func_8003D300(unsigned char value)
{
    D_800EF6E0.field0 = 1;
    D_8009B3CF = 0;
    D_8009B3C1 = 0;
    D_8009B3CC = 0x8000;
    D_8009B3DD = 0;
    D_8009B3C7 = value;
}
