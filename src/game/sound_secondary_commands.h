#ifndef MEMORIES_DECOMP_SOUND_SECONDARY_COMMANDS_H
#define MEMORIES_DECOMP_SOUND_SECONDARY_COMMANDS_H

#include "../types.h"

/* The three sequence commands SD_DispatchSequenceChannelEvent dispatches:
 * SD_SEQUENCE_CONTROL_CHANGE, SD_SEQUENCE_PROGRAM_CHANGE and
 * SD_SEQUENCE_PITCH_BEND respectively.
 *
 * Its old file, sound_sequence_events.c, declared all three itself, and
 * every one of its declarations widened a u8 to s32 -- the channel index for
 * the latter two, and the control value for the first. Nothing compared the spellings, because
 * passing an s32 where a u8 is expected is an ordinary arithmetic
 * conversion rather than a diagnosable mismatch, so the build matched with
 * both live in the tree.
 *
 * The u8 widths come from the definitions and suit the data: these are
 * sequence bytes, and the reader hands them straight through.
 *
 * SD_SequenceTimerCallback lives in sd_sequence_timer_callback.c and is
 * declared in sound_event_runtime.h, rather than this command header. */
void func_8004B49C(s32 arg0, s32 arg1, u8 arg2);
void func_8004B6E8(u8 index, s32 value);
void func_8004B70C(u8 index, s32 unused, s32 value);

#endif
