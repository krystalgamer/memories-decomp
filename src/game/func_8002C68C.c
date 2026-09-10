#define FUNC_8002C604_AMBIENT_ID
#include "../types.h"
#include "func_8002C604.h"

/* The allocator stores an s32 request id, but this caller passes nothing, so
 * the id is whatever its own caller left in $a0. The guarded header arm keeps
 * that retail call without inventing an argument. This TU cannot include
 * duel_effect_request.h either: callers pass an id to func_8002C68C, but this
 * definition takes no arguments so that value remains ambient in $a0 for the
 * allocator call. */
extern u8 D_8009B260;

void *func_8002C68C(void)
{
    void *value = func_8002C604();

    if (value != 0) {
        D_8009B260 |= 0x80;
    }
    return value;
}
