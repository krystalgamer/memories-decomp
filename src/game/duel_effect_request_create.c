#include "../types.h"
#include "duel_effect_request.h"
#include "../unmatched.h"

/* Hands out one request: DuelEffect_AllocateRequest allocates it for the
 * caller's effect id, and bit 7 of the pool's status byte records that one is
 * out. The id
 * reaches the allocator in $a0 untouched, so forwarding it costs nothing;
 * this definition used to take no parameter and leave the id ambient, which
 * compiles to the same instructions. */
DuelEffectRequest *DuelEffect_CreateRequest(s32 id)
{
    DuelEffectRequest *value = (DuelEffectRequest *)DuelEffect_AllocateRequest(id);

    if (value != 0) {
        gDuel_bEffectRequestStatus |= DUEL_EFFECT_REQUEST_STATUS_ACTIVE;
    }
    return value;
}
