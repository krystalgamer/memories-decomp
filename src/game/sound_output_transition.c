#include "../types.h"
#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_transition.h"

void SD_FadeInStreamAudio(void)
{
    SDValue *first = g_SDValue;
    SDValue *state;

    first->cd_fade_target = 255;
    state = g_SDValue;
    first->cd_fade_step = 64;
    state->status_flags = (state->status_flags & 0xFFFC) | 4;
    if ((state->status_flags & 0x80) &&
        state->sequence_fade_step >= 0) {
        state->sequence_fade_step = -16;
        state->bgm_fade_target = 220;
    }
}

/* This function reads the sound root through a volatile pointer view. */
void SD_FadeOutStreamAudio(void)
{
    SDValue *state = *(SDValue *volatile *)&g_SDValue;
    SDValue *flags;

    if (state->status_flags & 0x80) {
        state->sequence_fade_step = 8;
        state->bgm_fade_target = 255;
        state = *(SDValue *volatile *)&g_SDValue;
        state->cd_fade_target = 0;
        flags = *(SDValue *volatile *)&g_SDValue;
        state->cd_fade_step = -64;
        flags->status_flags &= 0xFFFB;
        return;
    }
    state->cd_fade_target = 0;
    flags = *(SDValue *volatile *)&g_SDValue;
    state->cd_fade_step = -64;
    flags->status_flags &= 0xFFFB;
}

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
