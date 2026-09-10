/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_output.c.
 */
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "../game/func_80044DC0.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_sequence_state.h"
#include "../game/sound_output_state.h"
#include "../game/sound_transfer_lifecycle.h"
#include "../game/sound_voice_selection.h"

#include "../game/sound_init.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_output.h"
/* Not a duplicate of the sound_init.h declaration: a second addressing view
   of the same symbol. The one call below narrows its second argument to s16,
   and the canonical (s32, s32) prototype would widen it back. */
extern SDValue * volatile D_8009B45C_volatile asm("g_SDValue");

void func_8004733C(s32 arg0, s32 arg1)
{
    register s32 v asm("s1") = arg0;

    if ((D_8009B45C_volatile->flags_004A & 2) == 0) {
        return;
    }
    if (arg0 & 0x8000) {
        func_800473CC(SD_BGM_COMMAND_BASE);
        func_80045208(v & SD_COMMAND_VALUE_MASK, (s16)arg1);
    } else {
        register u32 masked asm("v0") =
            (u32)(v & SD_COMMAND_VALUE_MASK);

        if (masked >= SD_BGM_COMMAND_BASE) {
            arg0 -= SD_BGM_COMMAND_BASE;
        }
        func_80049230((s16)arg0, (s16)arg1);
    }
}

