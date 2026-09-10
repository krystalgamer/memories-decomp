#ifndef MEMORIES_DECOMP_FUNC_8004ACE4_H
#define MEMORIES_DECOMP_FUNC_8004ACE4_H

#include "../types.h"
#include "sound.h"

/* Applies a sequence channel's pending reverb command. It selects on the
 * command byte at entry[0x11] and uses entry[0x13] as the value, setting
 * either the reverb mode or the left and right depths, and mirrors the value
 * into D_8009B458.
 *
 * Both offsets are named fields of the SDSecondaryRecord its one caller
 * already holds: 0x11 is parameter_selector, which is what the
 * SD_SEQUENCE_PARAMETER_* cases below select on, and 0x13 is control_value,
 * which that caller assigns on the line before it calls this. The second
 * argument is unused by the definition. */
void func_8004ACE4(SDSecondaryRecord *entry, int unused);

#endif
