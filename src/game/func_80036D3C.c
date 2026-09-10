#include "../types.h"
#include "duel_effect.h"
#include "func_80036D3C.h"

int func_80036D3C(DuelEffectChannel *object)
{
    u8 **stream = &((u8 **)object)[object->stream_58];
    u8 *current = *stream;
    *stream = current + 2;
    return current[0] | (current[1] << 8);
}
