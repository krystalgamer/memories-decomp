#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_EVENTS_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_EVENTS_H

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

/* The three sequence commands SD_DispatchSequenceChannelEvent dispatches:
 * SD_SEQUENCE_CONTROL_CHANGE, SD_SEQUENCE_PROGRAM_CHANGE and
 * SD_SEQUENCE_PITCH_BEND respectively.
 *
 * The dispatcher's earlier file (an unrelated sound_sequence_events.c that
 * predates this unit) declared all three itself, and every one of its
 * declarations widened a u8 to s32 -- the channel index for
 * the latter two, and the control value for the first. Nothing compared the spellings, because
 * passing an s32 where a u8 is expected is an ordinary arithmetic
 * conversion rather than a diagnosable mismatch, so the build matched with
 * both live in the tree.
 *
 * The u8 widths come from the definitions and suit the data: these are
 * sequence bytes, and the reader hands them straight through.
 *
 * SD_SequenceTimerCallback and SD_OpenSequenceTimerEvent, the unit's last
 * two functions, are declared in sound_event_runtime.h because the callback
 * is installed through the SDK event ABI rather than dispatched as a
 * sequence command. */
void SD_HandleSequenceControlChange(s32 channel, s32 controller, u8 value);
void SD_SetSequenceChannelProgram(u8 index, s32 value);
void SD_SetSequenceChannelPitchBendMsb(u8 index, s32 unused, s32 value);

#endif
