#include "../../types.h"
#include "helpers.h"

s32 func_801680AC(s32 position)
{
    s32 distance;
    s32 result;

    if (position < 60) {
        distance = (60 - position) >> 2;
        distance *= distance;
        distance >>= 4;
        result = position + distance;
    } else if (position < 93) {
        result = position;
    } else {
        distance = (position - 92) >> 2;
        distance *= distance;
        distance >>= 4;
        result = position - distance;
    }
    return result;
}
