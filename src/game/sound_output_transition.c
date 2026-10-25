#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_transition.h"

#ifndef VERSION_JAPAN
extern SDValue *G32 volatile g_SDValue_output_transition asm("g_SDValue");
#endif

void SD_FadeInStreamAudio(void)
{
    SDValue *first = g_SDValue;
    SDValue *state;

    first->cd_fade_target = 255;
    state = g_SDValue;
    first->cd_fade_step = 64;
    state->flags_0040 = (state->flags_0040 & 0xFFFC) | 4;
    if ((state->flags_0040 & 0x80) &&
        state->sequence_fade_step >= 0) {
        state->sequence_fade_step = -16;
        state->bgm_fade_target = 220;
    }
}

#ifndef VERSION_JAPAN
/* Preserve this function's volatile pointer view inside the grouped unit. */
#define g_SDValue g_SDValue_output_transition
#endif

void SD_FadeOutStreamAudio(void)
{
    SDValue *state = g_SDValue;
    SDValue *flags;

    if (state->flags_0040 & 0x80) {
        state->sequence_fade_step = 8;
        state->bgm_fade_target = 255;
        state = g_SDValue;
        state->cd_fade_target = 0;
        flags = g_SDValue;
        state->cd_fade_step = -64;
        flags->flags_0040 &= 0xFFFB;
        return;
    }
    state->cd_fade_target = 0;
    flags = g_SDValue;
    state->cd_fade_step = -64;
    flags->flags_0040 &= 0xFFFB;
}

#ifndef VERSION_JAPAN
#undef g_SDValue
#endif

void SD_InitSpuCommonVolume(void)
{
    SpuCommonAttr entry;

    /* 707 is exactly these five bits, and the two fields it does not select
       are still written because retail writes them. */
    entry.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL |
                 SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX;
    entry.mvol.left = 16383;
    entry.mvol.right = 16383;
    entry.cd.volume.left = 32767;
    entry.cd.volume.right = 32767;
    entry.cd.reverb = SPU_OFF;
    entry.cd.mix = SPU_ON;
    SpuSetCommonAttr(&entry);
}
