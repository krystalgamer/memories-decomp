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

/* Initializes one model channel rooted at gModel_aSlots[index]: resets its
   state, maps the HMD block, scans every unit's type records into the
   handlers, links the coordinate slots, and picks the first coordinate
   whose parent is not claimed by a live unit.

   The event, link and retry scans are goto loops. GCC 2.8.1 weights a
   register's references by loop nesting, and only these loop forms give
   the 0x1F800000 scratch base, the shared slot cursor, the command cursor
   and the channel index the retail callee-saved registers. */
void Model_InitSlot(s32 index, u8 *hmd, s32 size, s32 flags)
{
    ModelSlot *base;
    GsUNIT *slot;
    u8 *cursor;
    GsOT *table;
    GsTYPEUNIT ev;
    s32 acc;
    s32 count;
    s32 i;
    u32 tag;
    s32 off;
    s32 loaded_limit;
    s32 limit;
    s32 sentinel;
    s32 next;
    s32 *cmd;
    GsCOORDUNIT *rec;
    GsCOORDUNIT *scan;
    GsCOORDUNIT *cur;
    GsCOORDUNIT *q;
    s32 handler;

    handler = (s32)GsU_00000000;
    base = &gModel_aSlots[index];
    slot = GS_UNIT_VIEW(base->hmd_units);
    cursor = hmd;
    if (index < 2) {
        table = gGraphics_apOrderingTables[2];
    } else {
        table = gGraphics_apOrderingTables[3];
    }
    count = 0;
    acc = 0;
    base->field_E13 = 0;
    base->field_E12 = 0;
    if (flags >= 0) {
        base->draw_mode_request = flags & 0x7F;
        if (flags & 0x80) {
            base->tint[7] = 5;
        } else {
            base->tint[7] = 0;
        }
    }
    base->tint[6] = 0;
    base->tint[5] = 0;
    base->tint[4] = 0;
    base->field_DC8[0] = 0;
    base->field_DC8[1] = 0;
    base->field_DC8[2] = 0;
    base->field_DC8[3] = 0;
    base->field_DD0[0] = 0;
    base->field_DD0[3] = -0x15E;
    base->field_DD0[1] = -0x15E;
    switch (index) {
    default:
        base->field_DD0[2] = 0;
        break;
    case 0:
        base->field_DD0[2] = 0x1C2;
        break;
    case 1:
        base->field_DD0[2] = -0x1C2;
        break;
    }
    base->animation_control_state = 6;
    base->active_animation_command_row = 0;
    base->part_command_reset_pending = 0;
    base->entry_count = 0;
    base->field_E18 = 0;
    base->field_D18 = 0;
    base->entries = 0;
    base->field_D1C = 0;
    base->field_E19 = 0;
    base->animation_sequence_count = 0;
    base->field_E1C = 0;
    base->field_DE8 = 0;
    base->field_DEC = 0;
    if (index < 2) {
        if (index != 0) {
            base->field_DE8 = gModel_Slot1PrimaryDataAddress;
        } else {
            base->field_DE8 = gModel_Slot0PrimaryDataAddress;
        }
        if (index != 0) {
            base->field_DEC = gModel_Slot1VariantDataAddress;
        } else {
            base->field_DEC = gModel_Slot0VariantDataAddress;
        }
    }
    base->lit_color_cache_address = 0;
    base->packet_budget_bytes = 0;
    base->lit_color_cache_entry_count = 0;
    base->prepared_primitive_count = 0;
    base->lit_color_cache_end_address = 0;
    base->field_E15 = 0;
    base->field_E1E = 0;
    base->is_active = 0;
    if (hmd == 0) {
        base->field_E1D = 0;
        base->load_step = 0xFF;
        return;
    }
    GsMapUnit((u32 *)hmd);
    cursor += 0xC;
    Model_ResetSlotAnimationTables(index, hmd);
    base->hmd_unit_count = *cursor;
    cursor += 4;
    if (base->hmd_unit_count >= 0x3D) {
        base->hmd_unit_count = 0x3C;
    }
    if (base->hmd_unit_count != 0) {
        i = 0;
        do {
            slot->coord = 0;
            cmd = *(s32 **)cursor;
            cursor += 4;
            slot->primtop = (unsigned PSXLONG *)cmd;
            if (cmd != 0) {
                GsScanUnit((u32 *)cmd, 0, 0, 0);
            evloop:
                if (GsScanUnit(0, &ev, table, (u32 *)0x1F800000) != 0) {
                    u8 *scratch;

                    tag = (u32)ev.type >> 24;
                    scratch = (u8 *)0x1F800000;
                    if (ev.type == 0) {
                        goto evloop;
                    }
                    if (tag < 2) {
                        goto masktest;
                    }
                    if (tag < 7) {
                        if (tag < 5) {
                            goto dispatch;
                        }
                    } else {
                        goto dispatch;
                    }
                masktest:
                    if (ev.type & 0x800000) {
                        base->entries = (u8 *)GsMapCoordUnit((u32 *)hmd, (u32 *)ev.ptr);
                        ev.type &= 0xFF7FFFFF;
                    }
                dispatch:
                    switch (tag) {
                    case 0:
                        base->prepared_primitive_count += tent_Model_RebaseTexturedPrimitives(
                            index, &ev, (GsARGUNIT *)scratch, &acc, &count);
                        Model_BindPrimitiveHandler((ModelHandlerObject *)&ev);
                        break;
                    case 1:
                        base->prepared_primitive_count += tent_Model_RebaseTexturedPrimitives(
                            index, &ev, (GsARGUNIT *)scratch, 0, &count);
                        Model_BindSharedPrimitiveHandler(
                            (ModelHandlerObject *)&ev);
                        break;
                    case 2:
                        Model_ProcessType2Unit(
                            index, (ModelTypeUnit *)&ev, scratch);
                        break;
                    case 3:
                        Model_LinkSlotAnimationUnit((s32 *)&ev, base);
                        break;
                    default:
                        *ev.ptr = handler;
                        break;
                    }
                    goto evloop;
                }
            }
            i++;
            slot++;
        } while (i < base->hmd_unit_count);
    }
    slot = GS_UNIT_VIEW(base->hmd_units) + 1;
    i = 1;
    base->packet_budget_bytes = count;
    base->lit_color_cache_entry_count = acc + i;
    if (i < base->hmd_unit_count - 1) {
        off = 0;
    link:
        base->entry_count++;
        if (slot->primtop != 0) {
            slot->coord = GS_COORD_UNIT_VIEW(base->entries + off);
        }
        off += MODEL_SLOT_DATA_ENTRY_SIZE;
        i++;
        slot++;
        if (i < base->hmd_unit_count - 1) {
            goto link;
        }
    }
    scan = GS_COORD_UNIT_VIEW(base->entries);
    if (scan != 0) {
        i = 0;
        while (scan->super != 0) {
            scan++;
            i++;
        }
        base->field_E18 = i;
        q = GS_COORD_UNIT_VIEW(base->entries) + i;
        base->field_D18 = q;
        base->field_E19 = Model_FindBranchingChildCoord(base, q);
        if (!(base->field_E19 < base->entry_count)) {
            base->field_E19 = base->field_E18;
        }
        cur = GS_COORD_UNIT_VIEW(base->entries) + base->field_E19;
        base->field_D1C = cur;
    retry:
        slot = GS_UNIT_VIEW(base->hmd_units);
        loaded_limit = base->hmd_unit_count;
        i = 0;
        if (loaded_limit != 0) {
            sentinel = -1;
            limit = loaded_limit;
        claimed:
            if (slot->coord != 0 && slot->primtop != 0) {
                if (*slot->primtop != sentinel || *(slot->primtop + 2) != 0) {
                    if (slot->coord->super == cur) {
                        goto scanned;
                    }
                }
            }
            i++;
            slot++;
            if (i < limit) {
                goto claimed;
            }
        scanned:
            if (i < base->hmd_unit_count) {
                goto found;
            }
        }
        next = Model_FindBranchingChildCoord(base, cur);
        if (!(next < base->entry_count)) {
            goto found;
        }
        cur = (GsCOORDUNIT *)base->entries + next;
        goto retry;
    found:
        rec = cur->super;
        if (rec != base->field_D18) {
            base->field_D1C = rec;
            if (rec != 0) {
                base->field_E19 = rec - GS_COORD_UNIT_VIEW(base->entries);
            }
        }
    }
    if (base->field_DE0 == 0) {
        base->field_DE0 = (u8 *)(hmd + size);
    }
}
