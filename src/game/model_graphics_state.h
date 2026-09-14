#ifndef MEMORIES_DECOMP_MODEL_GRAPHICS_STATE_H
#define MEMORIES_DECOMP_MODEL_GRAPHICS_STATE_H

#include "../types.h"

/* Shared model/graphics state at 0x8009AF88-0x8009AFAB.
 *
 * The model scene owns the pointer and model state through D_8009AFA1.
 * Graphics_BeginFrame and Graphics_SyncFrame publish the active buffer and
 * bounded frame step in D_8009AFA2-D_8009AFA4.
 *
 * graphics_frame.c needs the absolute-address declaration arm below. Its
 * retail stores use %hi/%lo and its view of D_8009AFA3 is non-volatile.
 * func_80058E1C also uses a non-volatile view, but keeps two explicit source
 * reads. The C owner remains volatile and in .sdata for the default view. */
/* Pointer to the active 0xB2-byte model record. Model_SetSlotProperties
 * selects it from D_80091008; model view/update code reads fields through it.
 * The background renderer reads through +0xB1, including texture metadata,
 * and reads this pointer before checking its model slot's active byte. */
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
/* Its definition is `s8 D_8009AF99 = 1;` (model_graphics_state.c:14), the
 * two assembly readers load it `lb` and none `lbu`, and both writers store
 * -1: func_8005A188 (matched) and the func_80051350 candidate. */
extern s8 D_8009AF99;
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
#elif defined(MODEL_GRAPHICS_STATE_CLAMP_NONVOLATILE)
extern u8 D_8009AFA2;
extern u8 D_8009AFA3;
#else
extern u8 D_8009AFA2;
extern volatile u8 D_8009AFA3;
#endif

/* A4-A7 have real four-byte backing in model_graphics_state.c: frame-step
 * override, unclassified byte, D_8009AFA6, and the scene's active slot (0/1).
 * func_800507D0 reaches the last byte with a GP-relative A4 + 3 relocation;
 * it must not index past a scalar declaration. The owner and this consumer
 * select the bounded array. Existing consumers retain the first-byte scalar
 * view (and Graphics_BeginFrame's absolute addressing).
 * D_8009AFA6 is the linker identity of byte 2, not a second allocation.
 * The following halfwords at A8/AA are outside this four-byte object. */
#if defined(MODEL_GRAPHICS_STATE_SCENE_BYTES)
#if defined(MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE)
#error scene byte view requires GP-relative addressing
#endif
extern u8 D_8009AFA4[4];
#elif defined(MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE)
extern u8 D_8009AFA4 __attribute__((section(".data")));
#else
extern u8 D_8009AFA4;
#endif

extern u8 D_8009AFA6;

s32 func_80058DCC(void);

#endif
