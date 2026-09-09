#ifndef MEMORIES_DECOMP_MODEL_SCENE_SETUP_H
#define MEMORIES_DECOMP_MODEL_SCENE_SETUP_H

#include "../types.h"

/* Resets the model scene: clears the two counters at D_8009AF8E/D_8009AF90 and
 * the packed state words, and rebuilds the coordinate unit. Three units call it
 * to enter a scene, and main_run_credits.c reaches it from an asm block through
 * a .reloc rather than a call. */
void func_800530C4(void);

/* Sets one model slot's properties. Variadic on purpose, and the arity is the
 * argument: slot 2 takes one property, slots 0 and 1 take five, and a first
 * property below zero leaves the current one alone. The definition reads them
 * with va_arg in that order, so a caller passing the wrong count for its slot
 * reads whatever follows on the stack. */
void Model_SetSlotProperties(s32 idx, ...);

/* Clears the scene's 256-entry table to VRAM, resets the three transfer
 * channels and the model slot flags, and drops the scene counters. Like
 * func_800530C4 above it is also reached from main_run_credits.c through an
 * asm .reloc rather than a call, so that reference does not go through this
 * declaration. */
void func_800533D8(void);

#endif
