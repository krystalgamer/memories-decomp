/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_voice_selection.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/func_80047788.h"
#include "../game/sound.h"
#include "../game/sound_output_state.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_voice_selection.h"

s32 func_80047C50(s32 value)
{
    register s32 result asm("$2") = value;

    value &= 0x8000;
    if (value)
        return result & 0xFFFF;
    return 0xFFFF;
}

