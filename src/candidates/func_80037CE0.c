/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/text_stream_commands.c.
 */
#include "../types.h"
#include "../game/text_stream_commands.h"
#include "../game/duel_effect.h"
#include "../unmatched.h"
#include "../game/dialog_choice.h"

void func_80037CE0(volatile u8 *object) {
    register unsigned short flags __asm__("$2");
    register unsigned char control __asm__("$3");
    if (object[0x56] >= gDialog_bChoiceCount) {
        object[0x51] = 1;
        flags = *(volatile unsigned short *)(object + 0x34);
        object[0x56] = 0;
        control = D_8009B34C;
        D_8009B340 = 0;
        flags &= 0xEFFF;
        control &= 0x30;
        *(unsigned short *)(object + 0x34) = flags;
        if (control != 0) gDialog_bChoiceCount = 2;
    }
}

