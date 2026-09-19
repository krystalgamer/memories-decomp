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
#define MODEL_SLOT_SETUP_EXPLICIT_TRANSFER_ARGS
#include "../game/model_slot_setup.h"

#define GS_COORD_UNIT_VIEW(unit) ((GsCOORDUNIT *)(unit))

/* Initializes one model channel rooted at D_800F2C40[index]: resets its
   state, maps the HMD block, scans every unit's type records into the
   handlers, links the coordinate slots, and picks the first coordinate
   whose parent is not claimed by a live unit.

   The event, link and retry scans are goto loops. GCC 2.8.1 weights a
   register's references by loop nesting, and only these loop forms give
   the 0x1F800000 scratch base, the shared slot cursor, the command cursor
   and the channel index the retail callee-saved registers. */
void func_8004CB0C(s32 index, u8 *hmd, s32 size, s32 flags)
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
    base = &D_800F2C40[index];
    slot = (GsUNIT *)base->field_000;
    cursor = hmd;
    if (index < 2) {
        table = D_800E9D90[2];
    } else {
        table = D_800E9D90[3];
    }
    count = 0;
    acc = 0;
    base->field_E13 = 0;
    base->field_E12 = 0;
    if (flags >= 0) {
        base->field_E11 = flags & 0x7F;
        if (flags & 0x80) {
            base->field_DC0[7] = 5;
        } else {
            base->field_DC0[7] = 0;
        }
    }
    base->field_DC0[6] = 0;
    base->field_DC0[5] = 0;
    base->field_DC0[4] = 0;
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
    base->field_E0E = 6;
    base->field_E0F = 0;
    base->field_E10 = 0;
    base->entry_count = 0;
    base->field_E18 = 0;
    base->field_D18 = 0;
    base->entries = 0;
    base->field_D1C = 0;
    base->field_E19 = 0;
    base->field_E1B = 0;
    base->field_E1C = 0;
    base->field_DE8 = 0;
    base->field_DEC = 0;
    if (index < 2) {
        if (index != 0) {
            base->field_DE8 = D_80010020;
        } else {
            base->field_DE8 = D_8001001C;
        }
        if (index != 0) {
            base->field_DEC = D_80010028;
        } else {
            base->field_DEC = D_80010024;
        }
    }
    base->field_DF0 = 0;
    base->field_E00 = 0;
    base->field_E02 = 0;
    base->field_E04 = 0;
    base->field_DF4 = 0;
    base->field_E15 = 0;
    base->field_E1E = 0;
    base->field_E1F = 0;
    if (hmd == 0) {
        base->field_E1D = 0;
        base->field_E14 = 0xFF;
        return;
    }
    GsMapUnit((u32 *)hmd);
    cursor += 0xC;
    func_8004D58C(index, hmd);
    base->field_E1A = *cursor;
    cursor += 4;
    if (base->field_E1A >= 0x3D) {
        base->field_E1A = 0x3C;
    }
    if (base->field_E1A != 0) {
        i = 0;
        do {
            slot->coord = 0;
            cmd = *(s32 **)cursor;
            cursor += 4;
            slot->primtop = (unsigned long *)cmd;
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
                        base->field_E04 += func_8004D134(
                            index, (u16 *)&ev, scratch, &acc, &count);
                        func_8006086C((ModelHandlerObject *)&ev);
                        break;
                    case 1:
                        base->field_E04 += func_8004D134(
                            index, (u16 *)&ev, scratch, 0, &count);
                        func_80060AEC((ModelHandlerObject *)&ev);
                        break;
                    case 2:
                        Model_ProcessType2Unit(
                            index, (ModelTypeUnit *)&ev, scratch);
                        break;
                    case 3:
                        func_8005C6A0((s32 *)&ev, base);
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
        } while (i < base->field_E1A);
    }
    slot = (GsUNIT *)base->field_000 + 1;
    i = 1;
    base->field_E00 = count;
    base->field_E02 = acc + i;
    if (i < base->field_E1A - 1) {
        off = 0;
    link:
        base->entry_count++;
        if (slot->primtop != 0) {
            slot->coord = GS_COORD_UNIT_VIEW(base->entries + off);
        }
        off += MODEL_SLOT_DATA_ENTRY_SIZE;
        i++;
        slot++;
        if (i < base->field_E1A - 1) {
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
        base->field_E19 = func_8005A3D0(base, q);
        if (!(base->field_E19 < base->entry_count)) {
            base->field_E19 = base->field_E18;
        }
        cur = GS_COORD_UNIT_VIEW(base->entries) + base->field_E19;
        base->field_D1C = cur;
    retry:
        slot = (GsUNIT *)base->field_000;
        loaded_limit = base->field_E1A;
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
            if (i < base->field_E1A) {
                goto found;
            }
        }
        next = func_8005A3D0(base, cur);
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
