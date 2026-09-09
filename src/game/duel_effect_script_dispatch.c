#include "../types.h"
#include "sound.h"
#include "duel_effect.h"
#include "duel_effect_script_dispatch.h"

void func_80038800(DuelEffectCommand *command)
{
    u8 **slot;
    u8 *cursor;
    u32 raw;
    u32 saved_raw;
    u32 opcode;
    u32 high_bit;

    slot = (u8 **)((u8 *)command + command->cursor_slot * 4);
    cursor = *slot;
    raw = *cursor++;
    saved_raw = raw;
    opcode = saved_raw & 0x7F;
    *slot = cursor;

    if (opcode == 0) {
        SD_BGMFadeOut();
        goto shared_test;
    } else {
        SD_BGMFadeOutWithStep(opcode);
    }

shared_test:
    high_bit = saved_raw & 0x80;
    if (high_bit != 0) {
        command->command_state = 13;
        D_8009B350 = 1;
    }
}
