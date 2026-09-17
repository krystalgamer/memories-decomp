#ifndef MEMORIES_DECOMP_MAIN_FRAME_H
#define MEMORIES_DECOMP_MAIN_FRAME_H

#include "../types.h"

/* The VSync callback installed by Main_Init: ticks the three frame counters,
   reads the raw pads, and runs the sound driver's per-frame work under a
   reentrancy flag. */
void Main_VBlankCB(void);

/* Advance one frame: run the pending frame work, wait for the previous frame
   to finish drawing, start the next one, and fold the raw pads into the
   held/pressed/repeat state. It is the only frame-advance primitive, and
   every one of its call sites is a loop body -- the busy-waits spin on their
   own condition, Main_Loop is the frontend loop itself, and the counted ones
   count passes through this. */
void Main_AdvanceFrame(void);

/* Advance `count` frames through Main_AdvanceFrame. The count is not
   checked, so zero means 2^32. */
void func_80012D84(s32 count);

#endif
