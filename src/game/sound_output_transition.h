#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_TRANSITION_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_TRANSITION_H

/* Installed by SD_InitState in gFile_RangeReadCallback and run by File_StepActiveTransfer
 * when a sector-range CdlReadS stream starts (state 5): starts the CD input
 * fade up to 255 by 64 per update, clears s_wSD_STATUS bits 0x1/0x2 and sets
 * 0x4. When s_wSD_STATUS 0x80 is set and its level step is not
 * negative, it also stores step -16 in s_wSD_SEQ_FADE_STEP and 220 in s_bSD_BGM_FADE_TARGET.
 * SD_UpdateFades clamps the s_wSD_SEQ_FADE_LEVEL level that s_wSD_SEQ_FADE_STEP steps against
 * s_bSD_SEQ_FADE_TARGET (0xFF once a sequence starts), so that dip is undone in the same
 * update. s_bSD_BGM_FADE_TARGET is instead the target of the separate s_wSD_BGM_FADE_LEVEL ramp
 * driven by s_wSD_BGM_FADE_STEP (set by SD_StartBgmFade), so 220 retargets that ramp
 * if it is active. */
void SD_FadeInStreamAudio(void);
/* Installed in gFile_RangePauseCallback and run on the stream's pause (state 1) and end
 * (state 6): starts the CD input fade down to 0 by 64 per update and clears
 * s_wSD_STATUS bit 0x4. When s_wSD_STATUS 0x80 is set, it also stores step 8 in
 * s_wSD_SEQ_FADE_STEP and 255 in s_bSD_BGM_FADE_TARGET. */
void SD_FadeOutStreamAudio(void);
/* SD_InitState's SPU common setup: the mask selects master volume 0x3FFF,
 * CD volume 0x7FFF and CD mix on. The packet's CD reverb field is also
 * written (off) but SPU_COMMON_CDREV is not in the mask, so it is not
 * applied. */
void SD_InitSpuCommonVolume(void);

#endif
