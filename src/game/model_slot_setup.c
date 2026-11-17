#include "../types.h"
#include "scratchpad.h"
#include "../game/ordering_tables.h"
#include "../game/high_memory_addresses.h"
#include "../game/model.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../game/model_slot_setup.h"
#include "../game/model_primitive_handler.h"
#include "../game/model_handler_registry.h"
#include "../game/model_packet_handlers.h"
#include "../game/model_scene_runtime.h"
#include "../game/model_slot_control.h"

#define MODEL_CHANNEL_HALFWORD(cursor) (*(u16 *)(cursor))

/* One model slot's setup: Model_InitSlot, then the primitive decoder it runs
   on each textured unit and the animation-table reset, build and relink
   passes, one contiguous run in every build.

   The model loader resets the tables before sending each event through the
   decoder; slot setup later walks the imported list. The reset fills keys
   with 0xFFFF, zeroes rows and maxima, and stores the command list at 0xDD8;
   the walk claims those keys, accumulates rows, and reads the same list. The
   byte cursors reach the part bitfield through ModelSlot::extended_animation_part_mask. */

/* Initializes one model channel rooted at gModel_aSlots[slot_index]: resets its
   state, maps the HMD block, scans every unit's type records into the
   handlers, links the coordinate slots, and picks the first coordinate
   whose parent is not claimed by a live unit.

   The render parent search descends from the candidate to its branching
   child until some live unit's coordinate hangs off the candidate. GCC 2.8.1
   weights a register's references by loop nesting, so the nested search
   loops make the shared unit cursor heavy; the record loop takes the
   scratchpad base from a function-scope copy, which keeps the per-record
   pointer free of a constant equivalence (local-alloc would double its live
   length) so it still outranks the cursor for $s3. The event loop needs the
   one-pass block around the HMD cursor reads to keep the command cursor in
   $s5 and the channel index in $s6. */
void Model_InitSlot(s32 slot_index, u8 *hmd, s32 hmd_size, s32 render_flags)
{
    ModelSlot *model_slot;
    ModelSlotHeadEntry *hmd_unit;
    u8 *hmd_cursor;
    GsOT *ordering_table;
    GsTYPEUNIT type_unit;
    s32 max_lit_color_index;
    s32 packet_bytes;
    s32 scan_index;
    u32 type_tag;
    s32 coordinate_index;
    s32 child_coordinate_index;
    s32 *group_words;
    GsCOORDUNIT *parent_coordinate;
    GsCOORDUNIT *root_scan_coordinate;
    GsCOORDUNIT *render_parent_candidate;
    GsCOORDUNIT *root_coordinate;
    s32 noop_handler_word;
    u8 *scratchpad;

    noop_handler_word = (s32)GsU_00000000;
    /* Like noop_handler_word, a constant reload rematerializes at each use;
       copying it per record keeps the record's pointer non-constant. */
    scratchpad = (u8 *)SCRATCH_ADDR(0);
    model_slot = &gModel_aSlots[slot_index];
    hmd_unit = model_slot->hmd_units;
    hmd_cursor = hmd;
    if (slot_index < 2) {
        ordering_table = gGraphics_apOrderingTables[2];
    } else {
        ordering_table = gGraphics_apOrderingTables[3];
    }
    packet_bytes = 0;
    max_lit_color_index = 0;
    model_slot->stone_texture_state = 0;
    model_slot->shadow_enabled = 0;
    if (render_flags >= 0) {
        model_slot->draw_mode_request = render_flags & 0x7F;
        if (render_flags & 0x80) {
            model_slot->tint[7] = 5;
        } else {
            model_slot->tint[7] = 0;
        }
    }
    model_slot->tint[6] = 0;
    model_slot->tint[5] = 0;
    model_slot->tint[4] = 0;
    model_slot->cached_dimensions[0] = 0;
    model_slot->cached_dimensions[1] = 0;
    model_slot->cached_dimensions[2] = 0;
    model_slot->cached_dimensions[3] = 0;
    model_slot->cached_position[0] = 0;
    model_slot->cached_position[3] = -0x15E;
    model_slot->cached_position[1] = -0x15E;
    switch (slot_index) {
    default:
        model_slot->cached_position[2] = 0;
        break;
    case 0:
        model_slot->cached_position[2] = 0x1C2;
        break;
    case 1:
        model_slot->cached_position[2] = -0x1C2;
        break;
    }
    model_slot->animation_control_state = 6;
    model_slot->active_animation_command_row = 0;
    model_slot->part_command_reset_pending = 0;
    model_slot->entry_count = 0;
    model_slot->root_coordinate_index = 0;
    model_slot->root_coordinate = 0;
    model_slot->entries = 0;
    model_slot->render_parent_coordinate = 0;
    model_slot->render_parent_coordinate_index = 0;
    model_slot->animation_sequence_count = 0;
    model_slot->field_E1C = 0;
    model_slot->primary_module_data_address = 0;
    model_slot->variant_module_data_address = 0;
    if (slot_index < 2) {
        if (slot_index != 0) {
            model_slot->primary_module_data_address = gModel_Slot1PrimaryDataAddress;
        } else {
            model_slot->primary_module_data_address = gModel_Slot0PrimaryDataAddress;
        }
        if (slot_index != 0) {
            model_slot->variant_module_data_address = gModel_Slot1VariantDataAddress;
        } else {
            model_slot->variant_module_data_address = gModel_Slot0VariantDataAddress;
        }
    }
    model_slot->lit_color_cache_address = 0;
    model_slot->packet_budget_bytes = 0;
    model_slot->lit_color_cache_entry_count = 0;
    model_slot->prepared_primitive_count = 0;
    model_slot->lit_color_cache_end_address = 0;
    model_slot->field_E15 = 0;
    model_slot->animation_sound_enabled = 0;
    model_slot->is_active = 0;
    if (hmd == 0) {
        model_slot->skip_sound_setup = 0;
        model_slot->load_step = 0xFF;
        return;
    }
    GsMapUnit((u32 *)hmd);
    hmd_cursor += 0xC;
    Model_ResetSlotAnimationTables(slot_index, hmd);
    model_slot->hmd_unit_count = *hmd_cursor;
    hmd_cursor += 4;
    if (model_slot->hmd_unit_count >= 0x3D) {
        model_slot->hmd_unit_count = 0x3C;
    }
    if (model_slot->hmd_unit_count != 0) {
        scan_index = 0;
        do {
            hmd_unit->coord = 0;
            /* A one-pass block: its loop weight lifts the HMD cursor above
               slot_index, so the cursor keeps $s5 and slot_index $s6. */
            do {
                group_words = *(s32 **)hmd_cursor;
                hmd_cursor += 4;
            } while (0);
            hmd_unit->primitive_data = group_words;
            if (group_words != 0) {
                GsScanUnit((u32 *)group_words, 0, 0, 0);
                while (GsScanUnit(0, &type_unit, ordering_table, (u32 *)scratchpad) != 0) {
                    u8 *sdk_scratch;

                    type_tag = (u32)type_unit.type >> 24;
                    sdk_scratch = scratchpad;
                    if (type_unit.type == 0) {
                        continue;
                    }
                    if ((type_tag >= 2 ? (type_tag < 7 ? type_tag >= 5 : 0) : 1) &&
                        (type_unit.type & 0x800000)) {
                        model_slot->entries = GsMapCoordUnit((u32 *)hmd, (u32 *)type_unit.ptr);
                        type_unit.type &= 0xFF7FFFFF;
                    }
                    switch (type_tag) {
                    case 0:
                        model_slot->prepared_primitive_count += tent_Model_RebaseTexturedPrimitives(
                            slot_index, &type_unit, (GsARGUNIT *)sdk_scratch, &max_lit_color_index, &packet_bytes);
                        Model_BindPrimitiveHandler((ModelHandlerObject *)&type_unit);
                        break;
                    case 1:
                        model_slot->prepared_primitive_count += tent_Model_RebaseTexturedPrimitives(
                            slot_index, &type_unit, (GsARGUNIT *)sdk_scratch, 0, &packet_bytes);
                        Model_BindSharedPrimitiveHandler(
                            (ModelHandlerObject *)&type_unit);
                        break;
                    case 2:
                        Model_ProcessType2Unit(
                            slot_index, (ModelTypeUnit *)&type_unit, sdk_scratch);
                        break;
                    case 3:
                        Model_LinkSlotAnimationUnit(&type_unit, model_slot);
                        break;
                    default:
                        *type_unit.ptr = noop_handler_word;
                        break;
                    }
                }
            }
            scan_index++;
            hmd_unit++;
        } while (scan_index < model_slot->hmd_unit_count);
    }
    hmd_unit = &model_slot->hmd_units[1];
    scan_index = 1;
    model_slot->packet_budget_bytes = packet_bytes;
    model_slot->lit_color_cache_entry_count = max_lit_color_index + scan_index;
    if (scan_index < model_slot->hmd_unit_count - 1) {
        coordinate_index = 0;
        do {
            model_slot->entry_count++;
            if (hmd_unit->primitive_data != 0) {
                hmd_unit->coord = &model_slot->entries[coordinate_index];
            }
            coordinate_index++;
            scan_index++;
            hmd_unit++;
        } while (scan_index < model_slot->hmd_unit_count - 1);
    }
    root_scan_coordinate = model_slot->entries;
    if (root_scan_coordinate != 0) {
        scan_index = 0;
        while (root_scan_coordinate->super != 0) {
            root_scan_coordinate++;
            scan_index++;
        }
        model_slot->root_coordinate_index = scan_index;
        root_coordinate = model_slot->entries + scan_index;
        model_slot->root_coordinate = root_coordinate;
        model_slot->render_parent_coordinate_index = Model_FindBranchingChildCoord(model_slot, root_coordinate);
        if (!(model_slot->render_parent_coordinate_index < model_slot->entry_count)) {
            model_slot->render_parent_coordinate_index = model_slot->root_coordinate_index;
        }
        render_parent_candidate = model_slot->entries + model_slot->render_parent_coordinate_index;
        model_slot->render_parent_coordinate = render_parent_candidate;
        for (;;) {
            hmd_unit = model_slot->hmd_units;
            for (scan_index = 0; scan_index < model_slot->hmd_unit_count; scan_index++, hmd_unit++) {
                /* A local terminator: loop.c hoists it only to this loop's
                   preheader, where the retry loop cannot move it again. */
                s32 chain_end = -1;
                s32 *unit_words;

                if (hmd_unit->coord != 0 && (unit_words = hmd_unit->primitive_data) != 0 &&
                    (unit_words[0] != chain_end || unit_words[2] != 0) &&
                    hmd_unit->coord->super == render_parent_candidate) {
                    break;
                }
            }
            if (scan_index < model_slot->hmd_unit_count) {
                break;
            }
            child_coordinate_index = Model_FindBranchingChildCoord(model_slot, render_parent_candidate);
            if (!(child_coordinate_index < model_slot->entry_count)) {
                break;
            }
            render_parent_candidate = model_slot->entries + child_coordinate_index;
        }
        parent_coordinate = render_parent_candidate->super;
        if (parent_coordinate != model_slot->root_coordinate) {
            model_slot->render_parent_coordinate = parent_coordinate;
            if (parent_coordinate != 0) {
                model_slot->render_parent_coordinate_index = parent_coordinate - model_slot->entries;
            }
        }
    }
    if (model_slot->animation_snapshot_buffer == 0) {
        model_slot->animation_snapshot_buffer = (hmd + hmd_size);
    }
}

/* Primitive decoder: walks the header's record count through records of the
   kind selected by unit->type's low halfword. For slot half `slot_index` it
   rebases two halfwords in each record, keeps *max_lit_color_index as the running maximum
   of the record's index halfwords, and adds the kind's step to *packet_bytes; it returns the count, or 0
   for an unknown kind. Every selector arm is written out in both switches,
   the two pairs with equal strides included, and carries its own final
   compare; cross-jumping folds them into retail's shared tails. */
/* Europe also supports mode 2's packed-channel conversion and reads the
   records through typed Psy-Q HMD polygon layouts selected by the kind
   switch. */
#ifdef VERSION_EUROPE
s32 tent_Model_RebaseTexturedPrimitives(
    s32 slot_index, const GsTYPEUNIT *unit, GsARGUNIT *ctx, s32 *max_lit_color_index, s32 *packet_bytes
)
{
    ModelPrimitiveBlockHeader *primitive_header;
    u8 *primitive_record;
    u8 *primitive_base;
    s32 record_count;
    s32 remaining_records;
    s32 tsb_offset;
    s32 record_stride;
    s32 packet_size;
    s32 page_bias;
    u16 *tsb;
    u32 kind;
    s32 tpage;
    s32 depth;
    s32 current;
    s32 clut;
    s32 clut_row;
    s32 clut_col;
    /* The kind's CLUT field offset, then each index candidate. */
    s32 value;

    primitive_header = (ModelPrimitiveBlockHeader *)ctx->primp;
    primitive_base = (u8 *)((GsARGUNIT_NORMAL *)ctx)->primtop;
    record_count = primitive_header->record_count;
    primitive_record = primitive_base + primitive_header->word_offset * 4;
    remaining_records = record_count;
    if ((u32)slot_index >= 3) {
        return 0;
    }
    kind = (u16)unit->type;
    switch (kind) {
    case 9:
        tsb_offset = OFFSETOF(ModelPrimFT3, tsb);
        value = OFFSETOF(ModelPrimFT3, cba);
        record_stride = sizeof(ModelPrimFT3);
        packet_size = sizeof(POLY_FT3);
        break;
    case 0x209:
        tsb_offset = OFFSETOF(ModelPrimFT3C, prim.tsb);
        value = OFFSETOF(ModelPrimFT3C, prim.cba);
        record_stride = sizeof(ModelPrimFT3C);
        packet_size = sizeof(POLY_FT3);
        break;
    case 0xD:
        tsb_offset = OFFSETOF(ModelPrimGT3, tsb);
        value = OFFSETOF(ModelPrimGT3, cba);
        record_stride = sizeof(ModelPrimGT3);
        packet_size = sizeof(POLY_GT3);
        break;
    case 0x20D:
        tsb_offset = OFFSETOF(ModelPrimGT3C, prim.tsb);
        value = OFFSETOF(ModelPrimGT3C, prim.cba);
        record_stride = sizeof(ModelPrimGT3C);
        packet_size = sizeof(POLY_GT3);
        break;
    case 0x11:
        tsb_offset = OFFSETOF(ModelPrimFT4, tsb);
        value = OFFSETOF(ModelPrimFT4, cba);
        record_stride = sizeof(ModelPrimFT4);
        packet_size = sizeof(POLY_FT4);
        break;
    case 0x211:
        tsb_offset = OFFSETOF(ModelPrimFT4C, prim.tsb);
        value = OFFSETOF(ModelPrimFT4C, prim.cba);
        record_stride = sizeof(ModelPrimFT4C);
        packet_size = sizeof(POLY_FT4);
        break;
    case 0x15:
        tsb_offset = OFFSETOF(ModelPrimGT4, tsb);
        value = OFFSETOF(ModelPrimGT4, cba);
        record_stride = sizeof(ModelPrimGT4);
        packet_size = sizeof(POLY_GT4);
        break;
    case 0x215:
        tsb_offset = OFFSETOF(ModelPrimGT4C, prim.tsb);
        value = OFFSETOF(ModelPrimGT4C, prim.cba);
        record_stride = sizeof(ModelPrimGT4C);
        packet_size = sizeof(POLY_GT4);
        break;
    default:
        return 0;
    }

    if (--remaining_records != -1) {
        s32 cba_offset = value;

        page_bias = (slot_index << 2) - 0xA;
        do {
            tsb = (u16 *)(primitive_record + tsb_offset);
            tpage = *tsb;
            depth = ((u32)tpage >> 7) & 3;
            if (slot_index < 2) {
                tpage += page_bias;
                *tsb = tpage;
                if (depth >= 3) {
                    *tsb = tpage & 0xFF7F;
                }
                if (depth < 2) {
                    u32 packed;

                    clut = *(u16 *)(primitive_record + cba_offset);
                    clut_row = ((u32)clut >> 6) & 0xF;
                    clut_row += 0xD0;
                    clut_row += slot_index << 4;
                    packed = (u32)clut_row << 6;
                    clut &= 0x3F;
                    packed |= clut;
                    *(u16 *)(primitive_record + cba_offset) = packed;
                }
            } else if (depth < 2) {
                clut = *(u16 *)(primitive_record + cba_offset);
                clut_row = (u32)clut >> 6;
                if (clut_row < 0x100) {
                    u32 page_row;

                    clut_col = (clut & 0x3F) << 4;
                    page_row = clut_row & 0xF;
                    clut_row = page_row + 0xD0;
                    if (clut_col >= 0x200) {
                        clut_row = page_row + 0xF0;
                    }
                    clut_col = (clut_col & 0xFF) + 0x280;
                    {
                        s32 result = clut_row << 6;

                        result |= (clut_col >> 4) & 0x3F;
                        *(u16 *)(primitive_record + cba_offset) = result;
                    }
                }
            }
            if (max_lit_color_index != 0) {
                kind = (u16)unit->type;
                switch (kind) {
                case 9:
                    current = *max_lit_color_index;
                    value = ((ModelPrimFT3 *)primitive_record)->norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                case 0x209:
                    current = *max_lit_color_index;
                    value = ((ModelPrimFT3C *)primitive_record)->prim.norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                case 0xD:
                    current = *max_lit_color_index;
                    value = ((ModelPrimGT3 *)primitive_record)->norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    value = ((ModelPrimGT3 *)primitive_record)->norm1;
                    if (current < value) {
                        current = value;
                        *max_lit_color_index = value;
                    }
                    value = ((ModelPrimGT3 *)primitive_record)->norm2;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                case 0x20D:
                    current = *max_lit_color_index;
                    value = ((ModelPrimGT3C *)primitive_record)->prim.norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    value = ((ModelPrimGT3C *)primitive_record)->prim.norm1;
                    if (current < value) {
                        current = value;
                        *max_lit_color_index = value;
                    }
                    value = ((ModelPrimGT3C *)primitive_record)->prim.norm2;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                case 0x11:
                    current = *max_lit_color_index;
                    value = ((ModelPrimFT4 *)primitive_record)->norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                case 0x211:
                    current = *max_lit_color_index;
                    value = ((ModelPrimFT4C *)primitive_record)->prim.norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                case 0x15:
                    current = *max_lit_color_index;
                    value = ((ModelPrimGT4 *)primitive_record)->norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    value = ((ModelPrimGT4 *)primitive_record)->norm1;
                    if (current < value) {
                        current = value;
                        *max_lit_color_index = value;
                    }
                    value = ((ModelPrimGT4 *)primitive_record)->norm2;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    value = ((ModelPrimGT4 *)primitive_record)->norm3;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                case 0x215:
                    current = *max_lit_color_index;
                    value = ((ModelPrimGT4C *)primitive_record)->prim.norm0;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    value = ((ModelPrimGT4C *)primitive_record)->prim.norm1;
                    if (current < value) {
                        current = value;
                        *max_lit_color_index = value;
                    }
                    value = ((ModelPrimGT4C *)primitive_record)->prim.norm2;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    value = ((ModelPrimGT4C *)primitive_record)->prim.norm3;
                    if (current < value) {
                        current = value;
                    }
                    *max_lit_color_index = current;
                    break;
                }
            }
            primitive_record += record_stride;
            *packet_bytes += packet_size;
        } while (--remaining_records != -1);
    }
    return record_count;
}
#else
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
    u32 v;
    u32 adjusted;
    /* x and y carry the second rebase's row and sum as well as the running
       maximum and its candidate; one pseudo each keeps them in $a0/$a2. */
    s32 x;
    s32 y;
    u32 primitive_kind;

    primitive_header = (ModelPrimitiveBlockHeader *)ctx->primp;
    primitive_base = (u8 *)((GsARGUNIT_NORMAL *)ctx)->primtop;
    record_count = primitive_header->record_count;
    primitive_record = primitive_base + primitive_header->word_offset * 4;
    remaining_records = record_count;
    if ((u32)slot_index >= 2) {
        return 0;
    }
    primitive_kind = (u16)unit->type;
    switch (primitive_kind) {
    case 9:
        page_word_offset = 6;
        off2 = 2;
        record_stride = 0x14;
        packet_size = 0x20;
        break;
    case 0x209:
        page_word_offset = 0xA;
        off2 = 6;
        record_stride = 0x18;
        packet_size = 0x20;
        break;
    case 0xD:
        page_word_offset = 6;
        off2 = 2;
        record_stride = 0x18;
        packet_size = 0x28;
        break;
    case 0x20D:
        page_word_offset = 0xA;
        off2 = 6;
        record_stride = 0x1C;
        packet_size = 0x28;
        break;
    case 0x11:
        page_word_offset = 6;
        off2 = 2;
        record_stride = 0x18;
        packet_size = 0x28;
        break;
    case 0x211:
        page_word_offset = 0xA;
        off2 = 6;
        record_stride = 0x1C;
        packet_size = 0x28;
        break;
    case 0x15:
        page_word_offset = 6;
        off2 = 2;
        record_stride = 0x1C;
        packet_size = 0x34;
        break;
    case 0x215:
        page_word_offset = 0xA;
        off2 = 6;
        record_stride = 0x20;
        packet_size = 0x34;
        break;
    default:
        return 0;
    }

    if (--remaining_records != -1) {
        page_bias = (slot_index << 2) - 0xA;
        clut_bias = (slot_index << 4) + 0x3BD8;
        do {
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
            if (max_lit_color_index != 0) {
#define MODEL_MAX_VALUE(offset) (*(u16 *)(primitive_record + (offset)))
                primitive_kind = (u16)unit->type;
                switch (primitive_kind) {
                case 9:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0xC);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                case 0x209:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0x10);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                case 0xD:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0xC);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    y = MODEL_MAX_VALUE(0x10);
                    if (x < y) {
                        x = y;
                        *max_lit_color_index = y;
                    }
                    y = MODEL_MAX_VALUE(0x14);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                case 0x20D:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0x10);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    y = MODEL_MAX_VALUE(0x14);
                    if (x < y) {
                        x = y;
                        *max_lit_color_index = y;
                    }
                    y = MODEL_MAX_VALUE(0x18);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                case 0x11:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0xE);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                case 0x211:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0x12);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                case 0x15:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0xA);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    y = MODEL_MAX_VALUE(0x10);
                    if (x < y) {
                        x = y;
                        *max_lit_color_index = y;
                    }
                    y = MODEL_MAX_VALUE(0x14);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    y = MODEL_MAX_VALUE(0x18);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                case 0x215:
                    x = *max_lit_color_index;
                    y = MODEL_MAX_VALUE(0xE);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    y = MODEL_MAX_VALUE(0x14);
                    if (x < y) {
                        x = y;
                        *max_lit_color_index = y;
                    }
                    y = MODEL_MAX_VALUE(0x18);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    y = MODEL_MAX_VALUE(0x1C);
                    if (x < y) {
                        x = y;
                    }
                    *max_lit_color_index = x;
                    break;
                }
#undef MODEL_MAX_VALUE
            }
            primitive_record = primitive_record + record_stride;
            *packet_bytes = *packet_bytes + packet_size;
        } while (--remaining_records != -1);
    }
#undef off2
    return record_count;
}
#endif

void Model_ResetSlotAnimationTables(s32 slot_index, u8 *hmd)
{
    ModelSlot *slot;
    u8 *mask_clear_cursor;
    u8 *owner;
    void **kind3_payload;
    void **kind2_payload;
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
            kind3_payload = 0;
        } while (0);
    } while (0);
    kind2_payload = 0;
    index = 0;
    missing_command_index = 0xFFFF;
    duration_row_offset = 0;
    entry_row_offset = 0;
    slot = &gModel_aSlots[slot_index];
    slot->animation_position = 0;
    slot->animation_sound_position = 0;
    slot->animation_commands = 0;
    slot->animation_parameter_base = 0;
    slot->animation_snapshot_buffer = 0;
    slot->field_DE4 = 0;
    slot->lit_color_cache_address = 0;
    /* Keys, row values and the part mask stay slot-relative byte cursors with
       the member offset applied at the access: indexing those arrays, or
       typed pointers into them, folds the member offset into the induction
       variable and changes the loop code. */
    do {
        slot->animation_duration_rows[index].max = 0;
        part_index = 0;
        duration_value_offset = duration_row_offset;
        entry_key_offset = entry_row_offset;
        do {
            duration_value_cursor = (u8 *)slot + duration_value_offset;
            duration_value_offset += 2;
            entry_key_cursor = (u8 *)slot + entry_key_offset;
            entry_key_offset += 2;
            part_index++;
            *(u16 *)(entry_key_cursor + OFFSETOF(ModelSlot, animation_command_indices)) = missing_command_index;
            *(u16 *)(duration_value_cursor + OFFSETOF(ModelSlot, animation_duration_rows)) = 0;
        } while (part_index < 0x3A);
        duration_row_offset += sizeof(ModelSlotRow);
        entry_row_offset += sizeof(slot->animation_command_indices[0]);
        index++;
    } while (index < 0xA);
    index = 7;
    mask_clear_cursor = (u8 *)slot + index;
    do {
        mask_clear_cursor[OFFSETOF(ModelSlot, extended_animation_part_mask)] = 0;
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
                kind3_payload = *(void ***)(owner + 4);
            }
            if (owner_kind == 2) {
                kind2_payload = *(void ***)(owner + 4);
            }
        }
        owner = *(u8 **)owner;
    } while (owner != (u8 *)-1);
    if (kind3_payload != 0) {
        kind3_payload += 2;
        part_flags_base = *kind3_payload;
        kind3_payload++;
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
                part_mask_cursor = (u8 *)slot + mask_byte_index;
                mask_bit_base = mask_byte_index << 3;
                mask_byte = part_mask_cursor[OFFSETOF(ModelSlot, extended_animation_part_mask)];
                extended_flag = *(volatile s32 *)(part_flags_cursor + 4) & 0x100;
                if (extended_flag != 0) {
                    updated_mask_byte = mask_byte | (bit_one << (index - mask_bit_base));
                } else {
                    updated_mask_byte = mask_byte;
                }
                part_mask_cursor[OFFSETOF(ModelSlot, extended_animation_part_mask)] = updated_mask_byte;
                part_flags_cursor += 4;
                index++;
            } while ((u32)index < *(u16 *)part_flags_base);
        }
        slot->animation_commands = kind3_payload[0];
        slot->animation_parameter_base = kind3_payload[1];
    }
    if (kind2_payload != 0) {
        kind2_payload++;
        slot->animation_snapshot_buffer = kind2_payload[0];
        slot->field_DE4 = kind2_payload[1];
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
                /* Entered through a constant switch so loop.c treats the scan
                   like retail's goto loop (no invariant hoisting); a
                   row-0 target breaks out to the pre-row link below. */
                switch (0) {
                    for (;;) {
                    }
                    for (;;) {
            case 0:
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
                                break;
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
                                                    *(s16 *)target_command = row_one_command - slot->animation_commands;
                                                    *(s16 *)row_command = target_command - slot->animation_commands;
                                                    break;
                                                }
                                            }
                                            target_command++;
                                        } while (target_command != (s32 *)0);
                                    }
                                }
                            }
                        }
                        row_command++;
                    }
                }
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
