#include "../types.h"
#include "model.h"
#include "model_link_slot_animation_rows.h"
#include "model_slot_row_tables.h"

#define MODEL_CHANNEL_HALFWORD(cursor) (*(u16 *)(cursor))

/* One model slot's primitive decoder and animation-table reset, build and
   relink passes. The four form one contiguous gcc_2_8_1_g0 run.

   The model loader resets the tables before sending each event through the
   decoder; slot setup later walks the imported list. The reset fills keys
   with 0xFFFF, zeroes rows and maxima, and stores the command list at 0xDD8;
   the walk claims those keys, accumulates rows, and reads the same list. The
   byte cursors reach the part bitfield through ModelSlot::extended_animation_part_mask. */

/* Primitive decoder: walks the header's record count through records of the
   kind selected by unit->type's low halfword. For slot half `slot_index` it
   rebases two halfwords in each record, keeps *max_lit_color_index as the running maximum
   of the record's index halfwords, and adds the kind's step to *packet_bytes; it returns the count, or 0
   for an unknown kind. Every selector arm is written out in both switches,
   the two pairs with equal strides included, and carries its own final
   compare; cross-jumping folds them into retail's shared tails. */
/* Europe also supports mode 2's packed-channel conversion. Its separate
   translation unit keeps explicit maximum and secondary cursors rather
   than applying loop strength reduction. */
#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_MODEL_SLOT_DECODER)
s32 tent_Model_RebaseTexturedPrimitives(
    s32 slot_index, const GsTYPEUNIT *unit, GsARGUNIT *ctx, s32 *max_lit_color_index, s32 *packet_bytes
)
{
    ModelPrimitiveBlockHeader *primitive_header;
    u8 *primitive_record;
    u8 *primitive_base;
    u8 *p;
    u8 *q;
    s32 record_count;
    s32 remaining_records;
    s32 page_word_offset;
    s32 off2;
    s32 record_stride;
    s32 packet_size;
    s32 page_bias;
    s32 clut_bias;
    s32 color_depth;
#ifdef VERSION_EUROPE
    s32 v;
    u8 *field;
#else
    u32 v;
#endif
    u32 adjusted;
    /* x and y carry the second rebase's row and sum as well as the running
       maximum and its candidate; one pseudo each keeps them in $a0/$a2. */
    s32 x;
#ifndef VERSION_EUROPE
    s32 y;
#endif
    u32 primitive_kind;

#ifdef VERSION_EUROPE
#define MODEL_SECOND_OFFSET v
#else
#define MODEL_SECOND_OFFSET off2
#endif
    primitive_header = (ModelPrimitiveBlockHeader *)ctx->primp;
    primitive_base = (u8 *)((GsARGUNIT_NORMAL *)ctx)->primtop;
    record_count = primitive_header->record_count;
    primitive_record = primitive_base + primitive_header->word_offset * 4;
    remaining_records = record_count;
#ifdef VERSION_EUROPE
    if ((u32)slot_index >= 3) {
#else
    if ((u32)slot_index >= 2) {
#endif
        return 0;
    }
#ifdef VERSION_EUROPE
    off2 = (u16)unit->type;
    switch ((u32)off2) {
#else
    primitive_kind = (u16)unit->type;
    switch (primitive_kind) {
#endif
    case 9:
        page_word_offset = 6;
        MODEL_SECOND_OFFSET = 2;
        record_stride = 0x14;
        packet_size = 0x20;
        break;
    case 0x209:
        page_word_offset = 0xA;
        MODEL_SECOND_OFFSET = 6;
        record_stride = 0x18;
        packet_size = 0x20;
        break;
    case 0xD:
        page_word_offset = 6;
        MODEL_SECOND_OFFSET = 2;
        record_stride = 0x18;
        packet_size = 0x28;
        break;
    case 0x20D:
        page_word_offset = 0xA;
        MODEL_SECOND_OFFSET = 6;
        record_stride = 0x1C;
        packet_size = 0x28;
        break;
    case 0x11:
        page_word_offset = 6;
        MODEL_SECOND_OFFSET = 2;
        record_stride = 0x18;
        packet_size = 0x28;
        break;
    case 0x211:
        page_word_offset = 0xA;
        MODEL_SECOND_OFFSET = 6;
        record_stride = 0x1C;
        packet_size = 0x28;
        break;
    case 0x15:
        page_word_offset = 6;
        MODEL_SECOND_OFFSET = 2;
        record_stride = 0x1C;
        packet_size = 0x34;
        break;
    case 0x215:
        page_word_offset = 0xA;
        MODEL_SECOND_OFFSET = 6;
        record_stride = 0x20;
        packet_size = 0x34;
        break;
    default:
        return 0;
    }

    if (--remaining_records != -1) {
        page_bias = (slot_index << 2) - 0xA;
        clut_bias = (slot_index << 4) + 0x3BD8;
#ifdef VERSION_EUROPE
        p = primitive_record + 0x1C;
        q = primitive_record + MODEL_SECOND_OFFSET;
#endif
        do {
#ifdef VERSION_EUROPE
            field = primitive_record + page_word_offset;
            off2 = MODEL_CHANNEL_HALFWORD(field);
            v = ((u32)off2 >> 7) & 3;
            if (slot_index < 2) {
                off2 += page_bias;
                MODEL_CHANNEL_HALFWORD(field) = off2;
                if (v >= 3) {
                    MODEL_CHANNEL_HALFWORD(field) = off2 & 0xFF7F;
                }
                if (v < 2) {
                    u32 packed;

                    v = MODEL_CHANNEL_HALFWORD(q);
                    off2 = ((u32)v >> 6) & 0xF;
                    off2 += 0xD0;
                    off2 += slot_index << 4;
                    adjusted = off2;
                    packed = adjusted << 6;
                    v &= 0x3F;
                    packed |= v;
                    MODEL_CHANNEL_HALFWORD(q) = packed;
                }
            } else if (v < 2) {
                v = MODEL_CHANNEL_HALFWORD(q);
                off2 = (u32)v >> 6;
                if (off2 < 0x100) {
                    v = (v & 0x3F) << 4;
                    adjusted = off2 & 0xF;
                    off2 = adjusted + 0xD0;
                    if (v >= 0x200) {
                        off2 = adjusted + 0xF0;
                    }
                    v = (v & 0xFF) + 0x280;
                    {
                        /* Keep the packed result distinct from the index. */
                        s32 result = off2 << 6;

                        x = v >> 4;
                        result |= x & 0x3F;
                        MODEL_CHANNEL_HALFWORD(q) = result;
                    }
                }
            }
#else
            if (slot_index < 2) {
                p = primitive_record + page_word_offset;
                v = MODEL_CHANNEL_HALFWORD(p);
                adjusted = v + page_bias;
                MODEL_CHANNEL_HALFWORD(p) = adjusted;
                color_depth = (v >> 7) & 3;
                if (color_depth >= 3) {
                    MODEL_CHANNEL_HALFWORD(p) = adjusted & 0xFF7F;
                }
                if (color_depth < 2) {
                    q = primitive_record + off2;
                    v = MODEL_CHANNEL_HALFWORD(q);
                    y = v + clut_bias;
                    MODEL_CHANNEL_HALFWORD(q) = y;
                    x = v >> 6;
                    if (x >= 0x10) {
                        y = (y & 0x3F) + 0x10;
                        MODEL_CHANNEL_HALFWORD(q) = y;
                        MODEL_CHANNEL_HALFWORD(q) =
                            y | ((x % 0x10) << 6);
                    }
                }
            }
#endif
            if (max_lit_color_index != 0) {
#ifdef VERSION_EUROPE
#define MODEL_CURRENT_MAX off2
#define MODEL_MAX_CANDIDATE v
#else
#define MODEL_CURRENT_MAX x
#define MODEL_MAX_CANDIDATE y
#endif
#ifdef VERSION_EUROPE
#define MODEL_MAX_VALUE(offset) (*(u16 *)(p + (offset) - 0x1C))
#else
#define MODEL_MAX_VALUE(offset) (*(u16 *)(primitive_record + (offset)))
#endif
#ifdef VERSION_EUROPE
                off2 = (u16)unit->type;
                switch ((u32)off2) {
#else
                primitive_kind = (u16)unit->type;
                switch (primitive_kind) {
#endif
                case 9:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xC);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                case 0x209:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                case 0xD:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xC);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *max_lit_color_index = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                case 0x20D:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *max_lit_color_index = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x18);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                case 0x11:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xE);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                case 0x211:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x12);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                case 0x15:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xA);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *max_lit_color_index = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x18);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                case 0x215:
                    MODEL_CURRENT_MAX = *max_lit_color_index;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xE);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *max_lit_color_index = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x18);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x1C);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *max_lit_color_index = MODEL_CURRENT_MAX;
                    break;
                }
#undef MODEL_MAX_CANDIDATE
#undef MODEL_CURRENT_MAX
#undef MODEL_MAX_VALUE
            }
#ifdef VERSION_EUROPE
            p = p + record_stride;
            q += record_stride;
#endif
            primitive_record = primitive_record + record_stride;
            *packet_bytes = *packet_bytes + packet_size;
        } while (--remaining_records != -1);
    }
#undef MODEL_SECOND_OFFSET
    return record_count;
}
#endif

#ifndef MODEL_SLOT_DECODER_ONLY
void Model_ResetSlotAnimationTables(s32 slot_index, u8 *hmd)
{
    ModelSlot *slot;
    u8 *slot_bytes;
    u8 *duration_row_cursor;
    u8 *mask_clear_cursor;
    u8 *owner;
    u8 *kind3_payload;
    u8 *kind2_payload;
    u8 *part_flags_cursor;
    u8 *part_flags_base;
    u8 *part_mask_cursor;
    u8 *duration_value_cursor;
    u8 *entry_key_cursor;
    s32 missing_command_index;
    s32 bit_one;
    s32 index;
    s32 part_index;
    s32 duration_row_offset;
    s32 entry_row_offset;
    s32 duration_value_offset;
    s32 entry_key_offset;
    s32 owner_kind;
    s32 mask_bit_base;
    s32 updated_mask_byte;
    s32 mask_byte;
    s32 extended_flag;
    s32 mask_byte_index;

    do {
        do {
            kind3_payload = (u8 *)0;
        } while (0);
    } while (0);
    kind2_payload = (u8 *)0;
    index = 0;
    missing_command_index = 0xFFFF;
    duration_row_offset = 0;
    entry_row_offset = 0;
    slot = &gModel_aSlots[slot_index];
    slot_bytes = (u8 *)slot;
    duration_row_cursor = slot_bytes;
    slot->animation_position = 0;
    slot->animation_sound_position = 0;
    slot->animation_commands = 0;
    *(s32 *)&slot->animation_parameter_base = 0;
    *(s32 *)&slot->animation_snapshot_buffer = 0;
    *(s32 *)&slot->field_DE4 = 0;
    slot->lit_color_cache_address = 0;
    /* The reset walks keys and rows with running byte offsets rather than
       indexing slot->animation_command_indices[index][part_index] / slot->animation_duration_rows[index].values[part_index]. That is not
       a missed cleanup: the indexed form is larger, and the link fails with
       .initialized_data overlapping .text. The walkers are load-bearing.

       The margin is one instruction, which is worth knowing before touching
       anything else here. These three pointer members cost nothing to name:
       they are stored as zero, so the *(s32 *)& that keeps the store a
       non-struct reference is free. The four copies out of the command
       blocks at the end are not -- direct member stores push .text into
       .initialized_data. Their byte-cursor form remains, with each
       displacement derived from the corresponding member instead. */
    do {
        ((ModelSlotRow *)(duration_row_cursor + (u32)&((ModelSlot *)0)->animation_duration_rows))->max = 0;
        part_index = 0;
        duration_value_offset = duration_row_offset;
        entry_key_offset = entry_row_offset;
        do {
            duration_value_cursor = slot_bytes + duration_value_offset;
            duration_value_offset += 2;
            entry_key_cursor = slot_bytes + entry_key_offset;
            entry_key_offset += 2;
            part_index++;
            *(u16 *)(entry_key_cursor + (u32)&((ModelSlot *)0)->animation_command_indices) = missing_command_index;
            *(s16 *)(duration_value_cursor + (u32)&((ModelSlot *)0)->animation_duration_rows) = 0;
        } while (part_index < 0x3A);
        duration_row_offset += sizeof(ModelSlotRow);
        entry_row_offset += 0x74;
        index++;
        duration_row_cursor += sizeof(ModelSlotRow);
    } while (index < 0xA);
    index = 7;
    mask_clear_cursor = slot_bytes + index;
    do {
        mask_clear_cursor[(u32)&((ModelSlot *)0)->extended_animation_part_mask] = 0;
        index--;
        mask_clear_cursor--;
    } while (index >= 0);
    owner = *(u8 **)(hmd + 0x10);
    if (owner == (u8 *)0) {
        return;
    }
    do {
        if (*(s32 *)(owner + 8) != 0) {
            owner_kind = owner[0xF];
            if (owner_kind == 3) {
                kind3_payload = *(u8 **)(owner + 4);
            }
            if (owner_kind == 2) {
                kind2_payload = *(u8 **)(owner + 4);
            }
        }
        owner = *(u8 **)owner;
    } while (owner != (u8 *)-1);
    if (kind3_payload != (u8 *)0) {
        kind3_payload += 8;
        part_flags_base = *(u8 **)kind3_payload;
        kind3_payload += 4;
        index = 0;
        if (*(u16 *)part_flags_base != 0) {
            bit_one = 1;
            part_flags_cursor = part_flags_base;
            do {
                do {
                    do {
                        mask_byte_index = index / 8;
                    } while (0);
                } while (0);
                part_mask_cursor = slot_bytes + mask_byte_index;
                mask_bit_base = mask_byte_index << 3;
                mask_byte = part_mask_cursor[(u32)&((ModelSlot *)0)->extended_animation_part_mask];
                extended_flag = *(volatile s32 *)(part_flags_cursor + 4) & 0x100;
                if (extended_flag != 0) {
                    updated_mask_byte = mask_byte | (bit_one << (index - mask_bit_base));
                } else {
                    updated_mask_byte = mask_byte;
                }
                part_mask_cursor[(u32)&((ModelSlot *)0)->extended_animation_part_mask] = updated_mask_byte;
                part_flags_cursor += 4;
                index++;
            } while ((u32)index < *(u16 *)part_flags_base);
        }
        *(s32 *)(slot_bytes + (u32)&((ModelSlot *)0)->animation_commands) = *(s32 *)kind3_payload;
        *(s32 *)(slot_bytes + (u32)&((ModelSlot *)0)->animation_parameter_base) = *(s32 *)(kind3_payload + 4);
    }
    if (kind2_payload != (u8 *)0) {
        kind2_payload += 4;
        *(s32 *)(slot_bytes + (u32)&((ModelSlot *)0)->animation_snapshot_buffer) = *(s32 *)kind2_payload;
        *(s32 *)(slot_bytes + (u32)&((ModelSlot *)0)->field_DE4) = *(s32 *)(kind2_payload + 4);
    }
}

void Model_BuildSlotAnimationTables(s32 slot_index)
{
    ModelSlot *slot;
    ModelSlotPart *part_sequence;
    s32 *command_cursor;
    u32 command_word;
    s32 row_index;
    s32 part_index;
    s32 first_command_index;

    slot = &gModel_aSlots[slot_index];
    if (slot->animation_commands == 0) {
        return;
    }
    part_index = 0;
    if (part_index < slot->animation_sequence_count) {
        for (; part_index < slot->animation_sequence_count; part_index++) {
            part_sequence = slot->animation_sequences[part_index];
            if (part_sequence == 0) {
                break;
            }
            row_index = 1;
            part_sequence->start_sid = row_index;
            first_command_index = slot->animation_sequences[part_index]->start;
            command_cursor = &slot->animation_commands[first_command_index];
            slot->animation_command_indices[row_index][part_index] = first_command_index;
            while (1) {
                command_word = *command_cursor;
                if ((s32)command_word < 0) {
                    row_index = command_word >> 16;
                    row_index = row_index & 0x7F;
                    if (row_index == 0) {
                        break;
                    }
                    if (slot->animation_command_indices[row_index][part_index] != 0xFFFF) {
                        command_cursor++;
                    } else {
                        slot->animation_command_indices[row_index][part_index] = *(u16 *)command_cursor;
                        command_cursor = &slot->animation_commands[*(u16 *)command_cursor];
                    }
                } else {
                    slot->animation_duration_rows[row_index].values[part_index] =
                        slot->animation_duration_rows[row_index].values[part_index] + ((u8 *)command_cursor)[2];
                    command_cursor++;
                }
            }
        }
    }
    for (row_index = 1; row_index < MODEL_SLOT_ROW_COUNT; row_index++) {
        slot->animation_duration_rows[row_index].max = 0;
        for (part_index = 0; part_index < slot->animation_sequence_count; part_index++) {
            if (slot->animation_duration_rows[row_index].max < slot->animation_duration_rows[row_index].values[part_index]) {
                slot->animation_duration_rows[row_index].max = slot->animation_duration_rows[row_index].values[part_index];
            }
        }
    }
}

/* The relink pass after the table builder above. Every reach now goes through
   ModelSlot: the command list is slot->animation_commands, the channel count is
   slot->animation_sequence_count, and row 1's key for the current channel is
   slot->animation_command_indices[1][part_index].

   The two key reads inside the loops keep a byte-offset sum instead of
   slot->animation_command_indices[row_index][part_index]. Their address has to stay `slot + <running offset> +
   MODEL_SLOT_ROW_KEY_TABLE_OFFSET`, because that way the channel offset and
   the row stride share one induction variable; indexing the member gives the
   compiler a second one, which costs a callee-saved register and two
   instructions. Written left to right so the base is the first addend, which
   is also what the retail sum does. */
void Model_LinkSlotAnimationRows(s32 slot_index)
{
    ModelSlot *slot;
    s32 *row_command;
    s32 *target_command;
    s32 *pre_row_command;
    s32 *row_one_command;
    s32 part_index;
    s32 row_index;
    s32 part_byte_offset;
    s32 entry_command_index;
    s32 target_command_index;
    s32 command_word;
    u16 *target_entry;
    s32 entry_byte_offset;
    s32 target_word;
    s32 target_row;
    s32 row_one;
    s32 missing_command_index;
    s32 row_bits_clear_mask;
    s32 row_one_bit;

    slot = &gModel_aSlots[slot_index];
    part_byte_offset = 0;
    if (slot->animation_commands == 0) {
        return;
    }
    part_index = 0;
    if (slot->animation_sequence_count == 0) {
        return;
    }

    missing_command_index = 0xFFFF;
    row_bits_clear_mask = 0xFF80FFFF;
    row_one_bit = 0x10000;
    row_one = 1;
    part_byte_offset = part_index;

    do {
        row_index = 1;
        entry_byte_offset = part_byte_offset + sizeof(slot->animation_command_indices[0]);
        row_one_command = &slot->animation_commands[slot->animation_command_indices[1][part_index]];

        do {
            entry_command_index = *(u16 *)((u8 *)slot + entry_byte_offset + MODEL_SLOT_ROW_KEY_TABLE_OFFSET);
            row_command = &slot->animation_commands[entry_command_index];
            if (entry_command_index != missing_command_index) {
                pre_row_command = row_command - 1;
                while (1) {
                    command_word = *row_command;
                    if (command_word < 0) {
                        target_row = (u32)command_word >> 16;
                        target_row = target_row & 0x7F;
                        target_entry = (u16 *)((u8 *)slot
                            + (part_byte_offset + (u32)&((u16 (*)[MODEL_SLOT_PART_COUNT])0)[target_row])
                            + MODEL_SLOT_ROW_KEY_TABLE_OFFSET);
                        target_command_index = *target_entry;
                        target_command = &slot->animation_commands[target_command_index];
                        if (target_row == 0) {
                            goto zero;
                        }
                        *row_command = (command_word & 0xC07FFFFF)
                            | ((((u32)command_word >> 16) & 0x7F) << 23);
                        if (target_command_index != missing_command_index) {
                            if (target_row >= 2) {
                                if (target_command != (s32 *)0) {
                                    do {
                                        target_word = *target_command;
                                        if (target_word < 0) {
                                            if ((((u32)target_word >> 16) & 0x7F)
                                                == target_row) {
                                                *target_command = (target_word & row_bits_clear_mask) | row_one_bit;
                                            }
                                            if ((((u16 *)target_command)[1]
                                                & 0x7F) == row_one) {
                                                goto hit;
                                            }
                                        }
                                        target_command++;
                                    } while (target_command != (s32 *)0);
                                }
                            }
                        }
                    }
cont:
                    row_command++;
                }
hit:
                *(s16 *)target_command = row_one_command - slot->animation_commands;
                *(s16 *)row_command = target_command - slot->animation_commands;
                goto cont;
zero:
                /* Reloaded rather than reusing the base the links above
                   already hold: retail reads the member again here. */
                *(s16 *)pre_row_command = (row_command - (s32 *)*(volatile s32 *)&slot->animation_commands) - 1;
                *(s16 *)row_command = (pre_row_command - slot->animation_commands) + 1;
            }
            row_index++;
            entry_byte_offset += sizeof(slot->animation_command_indices[0]);
        } while (row_index < MODEL_SLOT_ROW_COUNT);

        part_byte_offset += 2;
        part_index++;
    } while (part_index < slot->animation_sequence_count);
}
#endif
