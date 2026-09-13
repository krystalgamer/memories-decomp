#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "../types.h"
#include "../unmatched.h"
#include "duel_effect_basic_commands.h"
#include "main_mode_state.h"

void func_80038888(void)
{
}

void func_80038890(void)
{
}

void func_80038898(DuelEffectChannel *object)
{
    u8 **stream = &((TextStreamOwner *)object)->streams[object->stream_58];
    u8 value = *(*stream)++;
    D_8009B26C[0] = 5;
    D_8009B363[0] = value;
}
