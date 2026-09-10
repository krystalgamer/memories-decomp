#include "../types.h"
#include "duel_effect_request.h"
#include "../unmatched.h"

/* Hands out one request: func_8002C604 allocates it for the caller's effect
 * id, and bit 7 of the pool's status byte records that one is out. The id
 * reaches the allocator in $a0 untouched, so forwarding it costs nothing;
 * this definition used to take no parameter and leave the id ambient, which
 * compiles to the same instructions. */
DuelEffectRequest *func_8002C68C(s32 id)
{
    DuelEffectRequest *value = (DuelEffectRequest *)func_8002C604(id);

    if (value != 0) {
        D_8009B260 |= 0x80;
    }
    return value;
}
