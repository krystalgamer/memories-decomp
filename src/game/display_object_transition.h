#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_TRANSITION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_TRANSITION_H

#include "../types.h"
#include "display_object.h"

/* Crossfades one display object into another and releases what it replaces:
 * over 17 frames b and a blended copy brighten from 0 to 0x80 while a and its
 * copy dim, and b zooms from 4.0 to 1.0 scale when a is present. It then
 * settles b's field_44 and clears its blend bits, and frees a and both
 * copies. The first argument may be null, which main_run_boot_sequence.c
 * relies on for the first transition of the boot sequence.
 *
 * Both parameters are DisplayObject, which is what the definition says. Its
 * one caller holds them as `register u8 *` locals pinned to particular
 * registers, so the conversion is stated at the call rather than by widening
 * this declaration to void *. */
void DisplayObject_CrossfadeReplace(DisplayObject *a, DisplayObject *b);

/* Holds a boot screen: spins the frame loop for count frames, or until START
 * or the confirm button is pressed once the file-transfer request words have
 * gone quiet. It never returns while a transfer is still in flight, even after
 * count runs out. Main_RunBootSequence passes 4 after the first boot image and
 * 0xB4 while the main-menu package loads behind the second. */
void Main_HoldBootScreen(s32 count);

#endif
