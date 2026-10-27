#ifndef MEMORIES_DECOMP_SOUND_MIX_H
#define MEMORIES_DECOMP_SOUND_MIX_H

#include "../types.h"

void SD_InitBufferState(void);
void SD_ResetCdPan(void);

/* CD-audio balance. channel_volume[0] and [1] are the left and right factors
 * SD_SetCdVolume multiplies into the CdMix levels, with SD_CHANNEL_VOLUME_MAX
 * (0x80) as unity. A pan of 1..0x80 lowers the left factor to 0x80 - pan and
 * keeps the right at unity; -0x80..-1 keeps the left at unity and stores the
 * low byte of -0x80 - pan on the right (0 at -0x80, but above unity for the
 * rest of that range); any other value centres both. These ranges apply to
 * the low 16 bits of pan read as an s16, which is what both callers pass: a
 * wider s32 aliases its low half, so 65537 acts as 1, INT32_MAX as -1 and
 * INT32_MIN (low half zero) centres. The current cd_volume is then
 * re-applied so the new balance takes effect at once. */
void SD_SetCdPan(s32 pan);
/* Sends the unscaled level to CdMix (val0 plus val1 or val2 by output_type),
 * then stores it as both cd_volume and the CD fade target cd_fade_target and
 * zeroes the fade step cd_fade_step, cancelling any CD fade. It also resets
 * mix_multiplier to 7 before the call. Movie_Play passes 0xFF, the movie
 * fade-out in Movie_DecodeAndPresentFrame passes the falling gMovie_bCdVolume, and
 * Movie_StopStream passes 0. */
void SD_SetCdVolumeImmediate(s32 level);
s32 SD_GetCdVolume(void);

#endif
