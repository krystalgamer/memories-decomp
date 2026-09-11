#include "../types.h"
#include "duel_effect.h"
#include "func_8003D300.h"
#include "../unmatched.h"

/* The size is a codegen input under -G8: it keeps this symbol out of small
 * data. Only byte zero is currently established. */

void func_8003D300(u8 value)
{
    D_800EF6E0[0] = 1;
    D_8009B3CF = 0;
    D_8009B3C1 = 0;
    D_8009B3CC = 0x8000;
    D_8009B3DD = 0;
    D_8009B3C7 = value;
}
