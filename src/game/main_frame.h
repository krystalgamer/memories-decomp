#ifndef MEMORIES_DECOMP_MAIN_FRAME_H
#define MEMORIES_DECOMP_MAIN_FRAME_H

#include "../types.h"

/* The VSync callback installed by Main_Init: ticks the three frame counters,
   reads the raw pads, and runs the sound driver's per-frame work under a
   reentrancy flag. */
void Main_VBlankCB(void);

/* Advance one frame: run the pending frame work, wait for the previous frame
   to finish drawing, start the next one, and fold the raw pads into the
   held/pressed/repeat state. Every busy-wait in the tree is a loop around
   this call. */
void func_80012D4C(void);

/* Advance `count` frames. The count is not checked, so zero means 2^32. */
void func_80012D84(s32 count);

#endif
