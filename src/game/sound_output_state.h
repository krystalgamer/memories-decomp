#ifndef MEMORIES_DECOMP_SOUND_OUTPUT_STATE_H
#define MEMORIES_DECOMP_SOUND_OUTPUT_STATE_H

#include "../types.h"

/* Stores a fade delta and its flag into the driver state. The third argument
   is written by no path in the definition; every caller passes 1. */
void func_8004503C(s16 value, u8 flag, s32 unused);

/* Reads the decoded CD block, sums the squares of one half of it, and returns
   the level unless the two low bits of the 0x40 flag word are set. */
s32 func_80045054(void);

/* Enqueues command 0x11 unless one of the last three queued commands already
   is one. */
void func_80045114(void);

/* Requests the sound at `value` with kind 0x80. The second argument is
   ignored, but it is a real parameter rather than a caller's mistake: see
   notes below and the candidate measurement in the commit message. */
s16 func_800451E0(u16 value, s32 unused);

/* Command 0x24: request and start. Returns 1 when the request was enqueued
   and 0 when the driver refused it. The second argument is unused. */
s32 func_80045208(u16 code, s32 unused);

/* Command 0x21: request without the start. Same table selection as
   func_80045208, no return value. */
void func_80045334(s32 code);

/* Clears the driver's busy flag. Installed as a callback, see
   SD_ArmBusyCallback. */
void SD_ClearBusyFlag(void);

#endif
