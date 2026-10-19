#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_CHANNEL_EVENTS_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_CHANNEL_EVENTS_H

#include "../types.h"
#include "sound.h"

/* Applies a sequence channel's pending reverb command. It selects on the
 * command byte at entry[0x11] and uses entry[0x13] as the value, setting
 * either the reverb mode or the left and right depths, and mirrors the value
 * into gSD_pSecondaryState.
 *
 * Both offsets are named fields of the SDSecondaryRecord its one caller
 * already holds: 0x11 is parameter_selector, which is what the
 * SD_SEQUENCE_PARAMETER_* cases below select on, and 0x13 is control_value,
 * which that caller assigns on the line before it calls this. The second
 * argument is unused by the definition. */
void SD_ApplySequenceReverbParameter(SDSecondaryRecord *entry, int unused);

/* Starts one secondary-driver note on `channel` (its low byte selects the
 * sequence channel record) for `note` with `velocity`: it walks the VAB
 * program and tone data, allocates a secondary object, fills the voice
 * attributes, keys the voice on and routes its reverb.
 *
 * The definition takes `velocity` as a byte in old style; the caller passes
 * a word, so the prototype keeps the promoted type. */
void SD_KeyOnSecondaryNote(s32 channel, s32 note, s32 velocity);

#endif
