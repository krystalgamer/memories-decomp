#include "../types.h"
#include "byte_triplet_helpers.h"

void func_8006C2FC(u8 *output, s32 first, s32 second, s32 third)
{
    output[0] = first;
    output[1] = second;
    output[2] = third;
}

void func_8006C30C(u8 *destination, const u8 *source)
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
}

void func_8006C330(u8 *out, u8 *a, u8 *b)
{
    out[0]=(a[0]+b[0])/2; out[1]=(a[1]+b[1])/2; out[2]=(a[2]+b[2])/2;
}
