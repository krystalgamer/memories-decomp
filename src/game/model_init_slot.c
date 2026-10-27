#include "../types.h"
#include "../game/ordering_tables.h"
#include "../game/high_memory_addresses.h"
#include "../game/model.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../game/model_slot_row_tables.h"
#include "../game/model_primitive_handler.h"
#include "../game/model_handler_registry.h"
#include "../game/model_packet_handlers.h"
#include "../game/model_parent_search.h"
#include "../game/model_slot_control.h"

#define GS_COORD_UNIT_VIEW(unit) ((GsCOORDUNIT *)(unit))
#define GS_UNIT_VIEW(unit) ((GsUNIT *)(unit))

/* Initializes one model channel rooted at gModel_aSlots[slot_index]: resets its
   state, maps the HMD block, scans every unit's type records into the
   handlers, links the coordinate slots, and picks the first coordinate
   whose parent is not claimed by a live unit.

   The event, link and retry scans are goto loops. GCC 2.8.1 weights a
   register's references by loop nesting, and only these loop forms give
   the 0x1F800000 scratch base, the shared slot cursor, the command cursor
   and the channel index the retail callee-saved registers. */
void Model_InitSlot(s32 slot_index, u8 *hmd, s32 hmd_size, s32 render_flags)
{
    ModelSlot *model_slot;
    GsUNIT *hmd_unit;
    u8 *hmd_cursor;
    GsOT *ordering_table;
    GsTYPEUNIT type_unit;
    s32 max_lit_color_index;
    s32 packet_bytes;
    s32 scan_index;
    u32 type_tag;
    s32 coordinate_byte_offset;
    s32 captured_unit_count;
    s32 unit_count_limit;
    s32 chain_end_marker;
    s32 child_coordinate_index;
    s32 *group_words;
    GsCOORDUNIT *parent_coordinate;
    GsCOORDUNIT *root_scan_coordinate;
    GsCOORDUNIT *render_parent_candidate;
    GsCOORDUNIT *root_coordinate;
    s32 noop_handler_word;

    noop_handler_word = (s32)GsU_00000000;
    model_slot = &gModel_aSlots[slot_index];
    hmd_unit = GS_UNIT_VIEW(model_slot->hmd_units);
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
            group_words = *(s32 **)hmd_cursor;
            hmd_cursor += 4;
            hmd_unit->primtop = (unsigned PSXLONG *)group_words;
            if (group_words != 0) {
                GsScanUnit((u32 *)group_words, 0, 0, 0);
            evloop:
                if (GsScanUnit(0, &type_unit, ordering_table, (u32 *)0x1F800000) != 0) {
                    u8 *sdk_scratch;

                    type_tag = (u32)type_unit.type >> 24;
                    sdk_scratch = (u8 *)0x1F800000;
                    if (type_unit.type == 0) {
                        goto evloop;
                    }
                    if (type_tag < 2) {
                        goto masktest;
                    }
                    if (type_tag < 7) {
                        if (type_tag < 5) {
                            goto dispatch;
                        }
                    } else {
                        goto dispatch;
                    }
                masktest:
                    if (type_unit.type & 0x800000) {
                        model_slot->entries = (u8 *)GsMapCoordUnit((u32 *)hmd, (u32 *)type_unit.ptr);
                        type_unit.type &= 0xFF7FFFFF;
                    }
                dispatch:
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
                        Model_LinkSlotAnimationUnit((s32 *)&type_unit, model_slot);
                        break;
                    default:
                        *type_unit.ptr = noop_handler_word;
                        break;
                    }
                    goto evloop;
                }
            }
            scan_index++;
            hmd_unit++;
        } while (scan_index < model_slot->hmd_unit_count);
    }
    hmd_unit = GS_UNIT_VIEW(model_slot->hmd_units) + 1;
    scan_index = 1;
    model_slot->packet_budget_bytes = packet_bytes;
    model_slot->lit_color_cache_entry_count = max_lit_color_index + scan_index;
    if (scan_index < model_slot->hmd_unit_count - 1) {
        coordinate_byte_offset = 0;
    link:
        model_slot->entry_count++;
        if (hmd_unit->primtop != 0) {
            hmd_unit->coord = GS_COORD_UNIT_VIEW(model_slot->entries + coordinate_byte_offset);
        }
        coordinate_byte_offset += MODEL_SLOT_DATA_ENTRY_SIZE;
        scan_index++;
        hmd_unit++;
        if (scan_index < model_slot->hmd_unit_count - 1) {
            goto link;
        }
    }
    root_scan_coordinate = GS_COORD_UNIT_VIEW(model_slot->entries);
    if (root_scan_coordinate != 0) {
        scan_index = 0;
        while (root_scan_coordinate->super != 0) {
            root_scan_coordinate++;
            scan_index++;
        }
        model_slot->root_coordinate_index = scan_index;
        root_coordinate = GS_COORD_UNIT_VIEW(model_slot->entries) + scan_index;
        model_slot->root_coordinate = root_coordinate;
        model_slot->render_parent_coordinate_index = Model_FindBranchingChildCoord(model_slot, root_coordinate);
        if (!(model_slot->render_parent_coordinate_index < model_slot->entry_count)) {
            model_slot->render_parent_coordinate_index = model_slot->root_coordinate_index;
        }
        render_parent_candidate = GS_COORD_UNIT_VIEW(model_slot->entries) + model_slot->render_parent_coordinate_index;
        model_slot->render_parent_coordinate = render_parent_candidate;
    retry:
        hmd_unit = GS_UNIT_VIEW(model_slot->hmd_units);
        captured_unit_count = model_slot->hmd_unit_count;
        scan_index = 0;
        if (captured_unit_count != 0) {
            chain_end_marker = -1;
            unit_count_limit = captured_unit_count;
        claimed:
            if (hmd_unit->coord != 0 && hmd_unit->primtop != 0) {
                if (*hmd_unit->primtop != chain_end_marker || *(hmd_unit->primtop + 2) != 0) {
                    if (hmd_unit->coord->super == render_parent_candidate) {
                        goto scanned;
                    }
                }
            }
            scan_index++;
            hmd_unit++;
            if (scan_index < unit_count_limit) {
                goto claimed;
            }
        scanned:
            if (scan_index < model_slot->hmd_unit_count) {
                goto found;
            }
        }
        child_coordinate_index = Model_FindBranchingChildCoord(model_slot, render_parent_candidate);
        if (!(child_coordinate_index < model_slot->entry_count)) {
            goto found;
        }
        render_parent_candidate = (GsCOORDUNIT *)model_slot->entries + child_coordinate_index;
        goto retry;
    found:
        parent_coordinate = render_parent_candidate->super;
        if (parent_coordinate != model_slot->root_coordinate) {
            model_slot->render_parent_coordinate = parent_coordinate;
            if (parent_coordinate != 0) {
                model_slot->render_parent_coordinate_index = parent_coordinate - GS_COORD_UNIT_VIEW(model_slot->entries);
            }
        }
    }
    if (model_slot->animation_snapshot_buffer == 0) {
        model_slot->animation_snapshot_buffer = (u8 *)(hmd + hmd_size);
    }
}
