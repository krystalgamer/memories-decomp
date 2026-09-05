#include "../types.h"
#include "duel_card.h"

s32 func_8002C400(s32 value)
{
    if (value >= DUEL_FIELD_SIDE_ZONE_COUNT) {
        return (value % DUEL_FIELD_SIDE_ZONE_COUNT) * 14 + 174;
    }
    return (value % DUEL_FIELD_SIDE_ZONE_COUNT) * 14 + 14;
}

s32 func_8002C484(s32 value)
{
    s32 quotient = value / DUEL_FIELD_SIDE_ZONE_COUNT;

    return quotient * 178 +
           (value - quotient * DUEL_FIELD_SIDE_ZONE_COUNT) * 16 + 14;
}
