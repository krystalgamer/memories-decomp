#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_TRANSITION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_TRANSITION_H

#include "../types.h"
#include "display_object.h"

/* Crossfades one display object into another and releases what it replaces:
 * it builds two intermediates, settles b's field_44 and clears its blend
 * bits, then frees a and both intermediates. The first argument may be null,
 * which main_run_boot_sequence.c relies on for the first transition of the
 * boot sequence.
 *
 * Both parameters are DisplayObject, which is what the definition says. Its
 * one caller holds them as `register u8 *` locals pinned to particular
 * registers, so the conversion is stated at the call rather than by widening
 * this declaration to void *. */
void func_8004365C(DisplayObject *a, DisplayObject *b);

/* Spins the frame loop for count frames, or until START or the confirm button
 * is pressed once the file-transfer request words have gone quiet. Callers
 * pass a plain frame count. */
void func_800438B8(s32 count);

#endif
