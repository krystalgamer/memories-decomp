#include "../types.h"
#include "script_state.h"

extern unsigned char D_8009B26C[];
extern u8 D_8009B363[];

void func_8002EB48(void)
{
    unsigned char value = *D_8009B290++;

    D_8009B26C[0] = 5;
    D_8009B363[0] = value;
}
