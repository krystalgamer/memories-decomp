#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_STATE_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_STATE_H

#include "../types.h"

/* Starts a CD-volume fade: cd_fade_step receives the signed per-update step
   that SD_UpdateFades adds to cd_volume, and cd_fade_target the volume it stops
   at. The third argument is written by no path in the definition; every
   caller passes 1. */
void SD_StartCdFade(s16 step, u8 target, s32 unused);

/* Reads the decoded CD block, sums the squares of one half of it, and returns
   the level unless the two low bits of the 0x40 flag word are set. */
s32 SD_MeasureCdAudioLevel(void);

/* Enqueues command 0x11, the CD-audio stop, when data_source_flags bit 0x80 is set
   and none of the last three queue slots already holds one. It starts the
   fade with SD_StartCdFade(-0x20, 0, 1); the command pump then waits for the
   CD volume to reach zero before resetting cd_request_code and the low
   status_flags bits. The BGM and sound-effect stop paths call it for bit-15
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

#endif
