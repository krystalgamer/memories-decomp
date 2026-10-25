#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_TRANSITION_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_TRANSITION_H

/* Installed by SD_InitState in gFile_RangeReadCallback and run by File_StepActiveTransfer
 * when a sector-range CdlReadS stream starts (state 5): starts the CD input
 * fade up to 255 by 64 per update, clears status_flags bits 0x1/0x2 and sets
 * 0x4. When status_flags 0x80 is set and its level step is not
 * negative, it also stores step -16 in sequence_fade_step and 220 in bgm_fade_target.
 * SD_UpdateFades clamps the sequence_fade_level level that sequence_fade_step steps against
 * sequence_fade_target (0xFF once a sequence starts), so that dip is undone in the same
 * update. bgm_fade_target is instead the target of the separate bgm_fade_level ramp
 * driven by bgm_fade_step (set by SD_StartBgmFade), so 220 retargets that ramp
 * if it is active. */
void SD_FadeInStreamAudio(void);
/* Installed in gFile_RangePauseCallback and run on the stream's pause (state 1) and end
 * (state 6): starts the CD input fade down to 0 by 64 per update and clears
 * status_flags bit 0x4. When status_flags 0x80 is set, it also stores step 8 in
 * sequence_fade_step and 255 in bgm_fade_target. */
void SD_FadeOutStreamAudio(void);
/* SD_InitState's SPU common setup: the mask selects master volume 0x3FFF,
 * CD volume 0x7FFF and CD mix on. The packet's CD reverb field is also
 * written (off) but SPU_COMMON_CDREV is not in the mask, so it is not
 * applied. */
void SD_InitSpuCommonVolume(void);

#endif
