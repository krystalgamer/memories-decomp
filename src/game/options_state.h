#ifndef MEMORIES_DECOMP_OPTIONS_STATE_H
#define MEMORIES_DECOMP_OPTIONS_STATE_H

/* gOptions_bState and the PAL overlay's gPalOptions_bState keep the
   screen's step in the low nibble and latch STARTED on a step's first
   frame. */
#define OPTIONS_STATE_STEP_MASK 0xF
#define OPTIONS_STATE_STARTED 0x80

#endif
