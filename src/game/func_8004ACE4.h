#ifndef MEMORIES_DECOMP_FUNC_8004ACE4_H
#define MEMORIES_DECOMP_FUNC_8004ACE4_H

#include "../types.h"

/* Applies a sequence channel's pending reverb command. It selects on the
 * command byte at entry[0x11] and uses entry[0x13] as the value, setting
 * either the reverb mode or the left and right depths, and mirrors the value
 * into D_8009B458.
 *
 * The parameter is u8 * because the function indexes bytes rather than naming
 * fields; its one caller holds the same record as SDSecondaryRecord *, which
 * sound.h fixes at 0x18 bytes, so both offsets fall inside the record. The
 * second argument is unused by the definition. */
void func_8004ACE4(u8 *entry, int unused);

#endif
