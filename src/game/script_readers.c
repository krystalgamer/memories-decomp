#include "../types.h"

extern u8 *D_8009B290;

int func_80030050(void)
{
    return *D_8009B290++;
}

int func_8003006C(void)
{
    u8 *current = D_8009B290;

    D_8009B290 = current + 2;
    return current[0] | (current[1] << 8);
}
