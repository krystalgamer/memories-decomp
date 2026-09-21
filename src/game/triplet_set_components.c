#include "../types.h"
#include "triangle_subdivision.h"

void Triplet_SetComponents(u8 *output, s32 first, s32 second, s32 third)
{
    output[0] = first;
    output[1] = second;
    output[2] = third;
}
