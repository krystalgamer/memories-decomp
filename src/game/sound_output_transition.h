#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_TRANSITION_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_TRANSITION_H

/* Installed by SD_InitState in D_8009B0F0 and run by File_StepActiveTransfer
 * when a sector-range CdlReadS stream starts (state 5): starts the CD input
 * fade up to 255 by 64 per update, clears flags_0040 bits 0x1/0x2 and sets
 * 0x4. While a sequence plays (flags_0040 0x80) and its level step is not
 * negative, it also stores step -16 in field_1588 and 220 in field_1584.
 * SD_UpdateFades clamps that step against field_158A (0xFF once a sequence
 * starts), not field_1584, so the sequence level is restored in the same
 * update. */
void SD_FadeInStreamAudio(void);
/* Installed in D_8009B120 and run on the stream's pause (state 1) and end
 * (state 6): starts the CD input fade down to 0 by 64 per update and clears
 * flags_0040 bit 0x4. While a sequence plays, it also stores step 8 in
 * field_1588 and 255 in field_1584. */
void SD_FadeOutStreamAudio(void);
/* SD_InitState's SPU common setup: master volume 0x3FFF, CD volume 0x7FFF,
 * CD mix on and CD reverb off. */
void SD_InitSpuCommonVolume(void);

#endif
