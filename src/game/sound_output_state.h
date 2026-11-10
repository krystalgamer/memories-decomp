#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_STATE_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_STATE_H
#include "../types.h"

/* Starts a CD-volume fade: s_wSD_CD_FADE_STEP receives the signed per-update step
   that SD_UpdateFades adds to s_wSD_CD_VOLUME, and s_bSD_CD_FADE_TARGET the volume it stops
   at. The third argument is written by no path in the definition; every
   caller passes 1. */
void SD_StartCdFade(s16 step, u8 target, s32 unused);

/* Reads the decoded CD block, sums the squares of one half of it, and returns
   the level unless the two low bits of the 0x40 flag word are set. */
s32 SD_MeasureCdAudioLevel(void);

/* Enqueues command 0x11, the CD-audio stop, when s_bSD_DATA_SOURCE bit 0x80 is set
   and none of the last three queue slots already holds one. It starts the
   fade with SD_StartCdFade(-0x20, 0, 1); the command pump then waits for the
   CD volume to reach zero before resetting s_wSD_CD_REQUEST_CODE and the low
   s_wSD_STATUS bits. The BGM and sound-effect stop paths call it for bit-15
   (CD) ids. */
void SD_EnqueueCdStop(void);

/* Forwards command_id and the literal 0x80 to SD_EnqueueCdPlay, whose
   second argument is unused. This wrapper's second argument is also
   unused; the wrapper still narrows the callee's result to s16. */
s16 SD_EnqueueCdSePlay(u16 command_id, s32 unused);

/* Command 0x24: request and start. Returns 0 when this wrapper's gates
   reject the code and 1 once they pass and submission was attempted;
   SD_EnqueueCommand's queue-full result is not propagated, and the pan
   reset and flags update still run in that case. The second argument is
   unused. The command pump turns the command into a sector-range file
   transfer of the selected table entry. */
s32 SD_EnqueueCdPlay(u16 command_id, s32 unused);

/* Command 0x21: request without the start. Same table selection as
   SD_EnqueueCdPlay, no return value. The command pump turns it into a
   seek-only transfer (File_RequestSecondarySeek). */
void SD_EnqueueCdSeek(s32 command_id);

/* Clears the driver's busy flag. Installed as a callback, see
   SD_ArmBusyCallback. */
void SD_ClearBusyFlag(void);

/* Sets the busy flag and installs SD_ClearBusyFlag as the completion callback. */
void SD_ArmBusyCallback(void);

/* Scales the two channel volume scalars in g_SDValue by a shared factor
 * derived from level and pushes the resulting 4-byte CdMix packet. */
void SD_SetCdVolume(s16 level);

void SD_InitBufferState(void);
void SD_ResetCdPan(void);

/* CD-audio balance. s_abSD_CD_CHANNEL_VOLUME[0] and [1] are the left and right factors
 * SD_SetCdVolume multiplies into the CdMix levels, with SD_CHANNEL_VOLUME_MAX
 * (0x80) as unity. A pan of 1..0x80 lowers the left factor to 0x80 - pan and
 * keeps the right at unity; -0x80..-1 keeps the left at unity and stores the
 * low byte of -0x80 - pan on the right (0 at -0x80, but above unity for the
 * rest of that range); any other value centres both. These ranges apply to
 * the low 16 bits of pan read as an s16, which is what both callers pass: a
 * wider s32 aliases its low half, so 65537 acts as 1, INT32_MAX as -1 and
 * INT32_MIN (low half zero) centres. The current s_wSD_CD_VOLUME is then
 * re-applied so the new balance takes effect at once. */
void SD_SetCdPan(s32 pan);
/* Sends the unscaled level to CdMix (val0 plus val1 or val2 by s_bSD_OUTPUT_TYPE),
 * then stores it as both s_wSD_CD_VOLUME and the CD fade target s_bSD_CD_FADE_TARGET and
 * zeroes the fade step s_wSD_CD_FADE_STEP, cancelling any CD fade. It also resets
 * s_bSD_CD_MIX_MULTIPLIER to 7 before the call. Movie_Play passes 0xFF, the movie
 * fade-out in Movie_DecodeAndPresentFrame passes the falling gMovie_bCdVolume, and
 * Movie_StopStream passes 0. */
void SD_SetCdVolumeImmediate(s32 level);
s32 SD_GetCdVolume(void);

#endif
