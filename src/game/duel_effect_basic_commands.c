#include "../types.h"
#define MAIN_MODE_STATE_ACTIVE_AS_ARRAY
#include "main_mode_state.h"
#include "../unmatched.h"
#include "duel_effect_basic_commands.h"

void func_80038888(void)
{
}

void func_80038890(void)
{
}

void func_80038898(u8 *object)
{
    u8 **stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    u8 value = *(*stream)++;
    D_8009B26C[0] = 5;
    D_8009B363[0] = value;
}
