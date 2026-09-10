#ifndef MEMORIES_DECOMP_MODEL_GRAPHICS_STATE_H
#define MEMORIES_DECOMP_MODEL_GRAPHICS_STATE_H

#include "../types.h"

/* Shared model/graphics state at 0x8009AF88-0x8009AFAB.
 *
 * The model scene owns the pointer and model state through D_8009AFA1.
 * Graphics_BeginFrame and Graphics_SyncFrame publish the active buffer and
 * bounded frame step in D_8009AFA2-D_8009AFA4. The final six bytes are still
 * reached as a byte range by unmatched model code; D_8009AFA6 names its first
 * byte, the only one matched C reads directly.
 *
 * graphics_frame.c needs the absolute-address declaration arm below. Its
 * retail stores use %hi/%lo and its view of D_8009AFA3 is non-volatile, while
 * func_80058E1C needs the volatile small-data view to preserve two loads. The
 * C owner remains volatile and in .sdata either way. */
/* Pointer to the active 0xB2-byte model record. Model_SetSlotProperties
 * selects it from D_80091008; model view/update code reads fields through it. */
extern u8 *D_8009AF88;

/* Model view and scene state. D_8009AF8E/D_8009AF90 are the yaw/pitch
 * accumulators updated by func_8004E7B0; D_8009AF94 is the unsigned scene
 * mode; D_8009AF9A is the signed phase-completion sentinel. */
extern u16 D_8009AF8C;
extern s16 D_8009AF8E;
extern s16 D_8009AF90;
extern u16 D_8009AF92;
extern u8 D_8009AF94;
extern u16 D_8009AF96;
extern u8 D_8009AF98;
/* Candidate func_80051350 needs its historical unsigned load/store view.
 * Matched C writes -1 and the assembly readers use lb, so the default is s8. */
#ifdef MODEL_GRAPHICS_STATE_AF99_UNSIGNED
extern u8 D_8009AF99;
#else
extern s8 D_8009AF99;
#endif
extern s8 D_8009AF9A;

/* Out-of-band arguments func_800528AC publishes for unmatched func_800540B4:
 * a one-byte flag and a word-sized address expressed as s32 in retail C. */
extern u8 D_8009AF9B;
extern s32 D_8009AF9C;
extern u8 D_8009AFA0;
extern u8 D_8009AFA1;

/* Active graphics buffer, bounded frame step, and frame-step override. */
#ifdef MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE
extern u8 D_8009AFA2 __attribute__((section(".data")));
extern u8 D_8009AFA3 __attribute__((section(".data")));
extern u8 D_8009AFA4 __attribute__((section(".data")));
#else
extern u8 D_8009AFA2;
extern volatile u8 D_8009AFA3;
extern u8 D_8009AFA4;
#endif

extern u8 D_8009AFA6;

#endif
