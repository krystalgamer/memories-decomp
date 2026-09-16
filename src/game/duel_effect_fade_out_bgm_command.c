#include "../types.h"
#include "sound.h"
#include "duel_effect.h"
#include "duel_effect_fade_out_bgm_command.h"

void DuelEffect_FadeOutBgmCommand(DuelEffectCommand *command)
{
    u8 **slot;
    u8 *cursor;
    u32 raw;
    u32 saved_raw;
    u32 opcode;
    u32 high_bit;

    slot = &((TextStreamOwner *)command)->streams[command->cursor_slot];
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
