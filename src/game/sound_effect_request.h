#ifndef MEMORIES_DECOMP_SOUND_EFFECT_REQUEST_H
#define MEMORIES_DECOMP_SOUND_EFFECT_REQUEST_H
#include "../types.h"

/* Local input snapshots, not a sound-driver memory record. */
typedef union {
    u64 all;
    struct { s32 original; s32 copy; } words;
} SDSEPlayIdPair;

/* Retires the driver's music resources: stops and closes the sequence when
   s_wSD_SEQ_ACCESS is active, releases a successful s_wSD_MUSIC_VAB_ID, then
   clears the pending pair at s_wSD_SEQ_FADE_LEVEL/s_wSD_SEQ_FADE_STEP. */
void SD_ResetMusicState(void);

/* The two BGM fade setters. Both store a signed per-tick step in s_wSD_BGM_FADE_STEP
 * and a target level in s_bSD_BGM_FADE_TARGET; SD_UpdateFades moves s_wSD_BGM_FADE_LEVEL by the
 * step until it reaches the target. Their bodies are identical -- retail has
 * two symbols for the same two stores. SD_BGMFade reaches SD_StartBgmFade;
 * tent_SD_StartBgmFadeCopy is reached only from tent_SD_BGMFadeCopy, which
 * has no caller. */
void tent_SD_StartBgmFadeCopy(s16 value, u8 flag);
void SD_StartBgmFade(s16 step, u8 target);

/* The state query reads only g_SDValue. */
s32 SD_IsBgmFadeActive(void);

/* Updates active effect voices for a direct or bank-mapped sound ID.
 * Bit 0x8000 routes to the output-pan path instead. Pan arithmetic uses the
 * signed low halfword; the public arguments retain their full word ABI. */
void SD_SESetPan(s32 sound_id, s32 pan);

#endif
