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
#define MODEL_PARENT_SEARCH_COORD_VIEW
#include "../game/model_parent_search.h"
#define MODEL_SLOT_SETUP_EXPLICIT_TRANSFER_ARGS
#include "../game/model_slot_setup.h"

/* Initializes one model channel rooted at D_800F2C40[index]: resets its
   state, maps the HMD block, scans every unit's type records into the
   handlers, links the coordinate slots, and picks the first coordinate
   whose parent is not claimed by a live unit.

   The event, link and retry scans are goto loops. GCC 2.8.1 weights a
   register's references by loop nesting, and only these loop forms give
   the 0x1F800000 scratch base, the shared slot cursor, the command cursor
   and the channel index the retail callee-saved registers. */
void func_8004CB0C(s32 index, s32 hmd, s32 size, s32 flags)
{
    u8 *base;
    u8 *slot;
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
    base = (u8 *)&D_800F2C40[index];
    slot = base;
    cursor = (u8 *)hmd;
    if (index < 2) {
        table = D_800E9D90[2];
    } else {
        table = D_800E9D90[3];
    }
    count = 0;
    acc = 0;
    base[0xE13] = 0;
    base[0xE12] = 0;
    if (flags >= 0) {
        base[0xE11] = flags & 0x7F;
        if (flags & 0x80) {
            base[0xDC7] = 5;
        } else {
            base[0xDC7] = 0;
        }
    }
    base[0xDC6] = 0;
    base[0xDC5] = 0;
    base[0xDC4] = 0;
    *(s16 *)(base + 0xDC8) = 0;
    *(s16 *)(base + 0xDCA) = 0;
    *(s16 *)(base + 0xDCC) = 0;
    *(s16 *)(base + 0xDCE) = 0;
    *(s16 *)(base + 0xDD0) = 0;
    *(s16 *)(base + 0xDD6) = -0x15E;
    *(s16 *)(base + 0xDD2) = -0x15E;
    switch (index) {
    default:
        *(s16 *)(base + 0xDD4) = 0;
        break;
    case 0:
        *(s16 *)(base + 0xDD4) = 0x1C2;
        break;
    case 1:
        *(s16 *)(base + 0xDD4) = -0x1C2;
        break;
    }
    base[0xE0E] = 6;
    base[0xE0F] = 0;
    base[0xE10] = 0;
    base[0xE17] = 0;
    base[0xE18] = 0;
    *(s32 *)(base + 0xD18) = 0;
    *(s32 *)(base + 0xD14) = 0;
    *(s32 *)(base + 0xD1C) = 0;
    base[0xE19] = 0;
    base[0xE1B] = 0;
    base[0xE1C] = 0;
    *(s32 *)(base + 0xDE8) = 0;
    *(s32 *)(base + 0xDEC) = 0;
    if (index < 2) {
        if (index != 0) {
            *(s32 *)(base + 0xDE8) = D_80010020;
        } else {
            *(s32 *)(base + 0xDE8) = D_8001001C;
        }
        if (index != 0) {
            *(s32 *)(base + 0xDEC) = D_80010028;
        } else {
            *(s32 *)(base + 0xDEC) = D_80010024;
        }
    }
    *(s32 *)(base + 0xDF0) = 0;
    *(s16 *)(base + 0xE00) = 0;
    *(s16 *)(base + 0xE02) = 0;
    *(s16 *)(base + 0xE04) = 0;
    *(s32 *)(base + 0xDF4) = 0;
    base[0xE15] = 0;
    base[0xE1E] = 0;
    base[0xE1F] = 0;
    if (hmd == 0) {
        base[0xE1D] = 0;
        base[0xE14] = 0xFF;
        return;
    }
    GsMapUnit((u32 *)hmd);
    cursor += 0xC;
    func_8004D58C(index, (u8 *)hmd);
    base[0xE1A] = *cursor;
    cursor += 4;
    if ((u8)base[0xE1A] >= 0x3D) {
        base[0xE1A] = 0x3C;
    }
    if (base[0xE1A] != 0) {
        i = 0;
        do {
            *(s32 *)slot = 0;
            cmd = *(s32 **)cursor;
            cursor += 4;
            *(s32 **)(slot + 4) = cmd;
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
                        *(GsCOORDUNIT **)(base + 0xD14) =
                            GsMapCoordUnit((u32 *)hmd, (u32 *)ev.ptr);
                        ev.type &= 0xFF7FFFFF;
                    }
                dispatch:
                    switch (tag) {
                    case 0:
                        *(u16 *)(base + 0xE04) += func_8004D134(
                            index, (u16 *)&ev, scratch, &acc, &count);
                        func_8006086C((ModelHandlerObject *)&ev);
                        break;
                    case 1:
                        *(u16 *)(base + 0xE04) += func_8004D134(
                            index, (u16 *)&ev, scratch, 0, &count);
                        func_80060AEC((ModelHandlerObject *)&ev);
                        break;
                    case 2:
                        func_80060220(index, (u8 *)&ev, scratch);
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
            slot += 8;
        } while (i < base[0xE1A]);
    }
    slot = base + 8;
    i = 1;
    *(s16 *)(base + 0xE00) = count;
    *(s16 *)(base + 0xE02) = acc + i;
    if (i < base[0xE1A] - 1) {
        off = 0;
    link:
        base[0xE17]++;
        if (*(s32 *)(slot + 4) != 0) {
            *(s32 *)slot = *(s32 *)(base + 0xD14) + off;
        }
        off += 0x50;
        i++;
        slot += 8;
        if (i < base[0xE1A] - 1) {
            goto link;
        }
    }
    scan = *(GsCOORDUNIT **)(base + 0xD14);
    if (scan != 0) {
        i = 0;
        while (scan->super != 0) {
            scan++;
            i++;
        }
        base[0xE18] = i;
        q = *(GsCOORDUNIT **)(base + 0xD14) + i;
        *(GsCOORDUNIT **)(base + 0xD18) = q;
        base[0xE19] = func_8005A3D0(base, q);
        if (!(base[0xE19] < base[0xE17])) {
            base[0xE19] = base[0xE18];
        }
        cur = *(GsCOORDUNIT **)(base + 0xD14) + base[0xE19];
        *(GsCOORDUNIT **)(base + 0xD1C) = cur;
    retry:
        slot = base;
        loaded_limit = base[0xE1A];
        i = 0;
        if (loaded_limit != 0) {
            sentinel = -1;
            limit = loaded_limit;
        claimed:
            if (((GsUNIT *)slot)->coord != 0 &&
                ((GsUNIT *)slot)->primtop != 0) {
                if (*((GsUNIT *)slot)->primtop != sentinel ||
                    *(((GsUNIT *)slot)->primtop + 2) != 0) {
                    if (((GsUNIT *)slot)->coord->super == cur) {
                        goto scanned;
                    }
                }
            }
            i++;
            slot += 8;
            if (i < limit) {
                goto claimed;
            }
        scanned:
            if (i < base[0xE1A]) {
                goto found;
            }
        }
        next = func_8005A3D0(base, cur);
        if (!(next < base[0xE17])) {
            goto found;
        }
        cur = *(GsCOORDUNIT **)(base + 0xD14) + next;
        goto retry;
    found:
        rec = cur->super;
        if (rec != *(GsCOORDUNIT **)(base + 0xD18)) {
            *(GsCOORDUNIT **)(base + 0xD1C) = rec;
            if (rec != 0) {
                base[0xE19] = rec - *(GsCOORDUNIT **)(base + 0xD14);
            }
        }
    }
    if (*(s32 *)(base + 0xDE0) == 0) {
        *(s32 *)(base + 0xDE0) = hmd + size;
    }
}
