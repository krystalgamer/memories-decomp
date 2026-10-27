#include "../types.h"
#include "model.h"
#include "model_slot_part_animation.h"

/* The GsSEQ fields read here are the ones Model_SeekSlotPartAnimation writes: ti is the
 * command the part stopped at, tframe the length of that command and rframe
 * the time left in it. 0x6000 is the clamp Model_SeekSlotPartAnimation applies in mode
 * 0x3C. */
s32 tent_Model_GetSlotPartAnimationPosition(ModelSlot *slot, s32 part_index)
{
    s32 duration_units;
    s32 animation_row;
    ModelSlotPart *part_sequence;
    s32 *command_base;
    s32 *current_command;
    s32 *row_command;
    s32 first_command_index;
    s32 remaining_frame;

    duration_units = 0;
    animation_row = 1;
    part_sequence = slot->animation_sequences[part_index];
    if (part_sequence == 0) {
        return 0;
    }
    current_command = slot->animation_commands + part_sequence->ti;
    if (*current_command < 0) {
        return 0;
    }
    command_base = slot->animation_commands;
    for (animation_row = 1; animation_row < 10; animation_row++) {
        first_command_index = slot->animation_command_indices[animation_row][part_index];
        row_command = command_base + first_command_index;
        if (first_command_index != 0xFFFF && current_command >= row_command - 1 &&
            command_base + *(u16 *)(row_command - 1) >= current_command) {
            break;
        }
    }
    if (animation_row < 10) {
        while (row_command < current_command) {
            duration_units += ((u8 *)row_command)[2];
            row_command++;
        }
        remaining_frame = part_sequence->rframe;
        if (remaining_frame != 0x6000 && remaining_frame != 0x7000) {
            duration_units += (part_sequence->tframe - remaining_frame) / 16;
        }
    } else {
        animation_row = 0;
    }
    return (animation_row << 24) | duration_units;
}

/* Seeks channel part_index of model slot slot to position scaled_position within sequence animation_row. The
 * sequence's entry index comes from animation_command_indices[animation_row][part_index] and its step count from
 * animation_duration_rows[animation_row].values[part_index]. The command list at animation_commands holds 4-byte entries
 * whose byte 2 is multiplied by 16 for this walk, and the halfword of the entry
 * before a sequence's first is its length. The walk consumes scaled_position modulo one
 * step's length, entry by entry. It stops at the first entry that outlasts
 * the remainder, at a negative entry, or at the end, then skips forward over
 * non-negative entries to find the next stop. The part's GsSEQ receives the
 * stop index (ti), the entry before it (ci), the entry length (tframe) and
 * the time left in it (rframe); mode 0x3C clamps that last value to 0x6000. */
void Model_SeekSlotPartAnimation(ModelSlot *slot, s32 part_index, s32 animation_row, u32 scaled_position) {
    s32 *command_base;
    ModelSlotPart *part_sequence;
    s32 *command_cursor;
    s32 *row_header;
    s32 *stop_cursor;
    s32 first_command_index;
    s32 row_frame_duration;
    s32 remaining_position;
    s32 entry_count;
    s32 entry_number;
    s32 command_index;
    s32 entry_frame_duration;
    s32 remaining_frame;
    s32 position_after_entry;
    s32 part_pointer_offset;
    s32 part_index_offset;

    /* Both channel offsets are materialised up front and never read by
       name: cse reuses them for the animation_sequences and animation_command_indices indexing just
       below, and that is what moves slot out of $a0 into $t3 at entry.
       Dropping either one changes the entry sequence. */
    part_pointer_offset = part_index * 4;
    part_index_offset = part_index * 2;
    first_command_index = slot->animation_command_indices[animation_row][part_index];
    command_base = slot->animation_commands;
    part_sequence = slot->animation_sequences[part_index];
    command_cursor = command_base + first_command_index;
    if (part_sequence == 0) {
        return;
    }
    if (first_command_index == 0xFFFF) {
        return;
    }
    /* Read twice rather than through a local: cse folds the second read,
       and the extra reference is what puts the address in $v1. */
    if (slot->animation_duration_rows[animation_row].values[part_index] == 0) {
        return;
    }
    row_frame_duration = slot->animation_duration_rows[animation_row].values[part_index] * 16;
    remaining_position = scaled_position % row_frame_duration;
    row_header = command_cursor - 1;
    if (remaining_position == 0) {
        if (scaled_position != 0 && slot->playback_state != 0x3C) {
            remaining_position = row_frame_duration;
        }
    }
    /* The guard and the loop both read the length from memory. jump.c
       duplicates the loop's exit test at its entry, cse folds it against
       the guard, and the loop optimiser hoists the mode byte, the 0x3C
       and the in-loop length read (which cse2 then turns into the copy
       retail keeps in $a3). A named length local defeats all of that. */
    entry_number = 0;
    if (entry_number >= *(u16 *)row_header) {
        return;
    }
    for (; entry_number < *(u16 *)row_header; entry_number++) {
        if (slot->playback_state != 0x3C) {
            position_after_entry = remaining_position - (((u8 *)command_cursor)[2] << 4);
            if (position_after_entry <= 0) {
                break;
            }
        } else {
            position_after_entry = remaining_position - (((u8 *)command_cursor)[2] << 4);
            if (position_after_entry < 0) {
                break;
            }
        }
        remaining_position = remaining_position - (((u8 *)command_cursor)[2] << 4);
        if (command_cursor[1] < 0) {
            break;
        }
        command_cursor++;
    }

    entry_count = *(u16 *)row_header;
    if (entry_number >= entry_count) {
        return;
    }
    /* A goto loop: a while would be rotated and undo the advance on the
       exit path. The advance sits before the count test on purpose, since
       retail's back-branch delay slot runs it on the exit path too. */
    stop_cursor = command_cursor;
scan:
    if (stop_cursor[1] >= 0) {
        entry_number++;
        stop_cursor++;
        if (entry_number < entry_count) {
            goto scan;
        }
    }
    command_index = command_cursor - slot->animation_commands;
    part_sequence->ti = command_index;
    if (command_index != 0) {
        command_index = command_index - 1;
    } else {
        command_index = stop_cursor - slot->animation_commands;
    }
    part_sequence->ci = command_index;
    entry_frame_duration = ((u8 *)command_cursor)[2] << 4;
    remaining_frame = entry_frame_duration - remaining_position;
    part_sequence->tframe = entry_frame_duration;
    part_sequence->rframe = remaining_frame;
    if (slot->playback_state != 0x3C) {
        return;
    }
    if ((s16)remaining_frame < part_sequence->tframe) {
        return;
    }
    part_sequence->rframe = 0x6000;
}
