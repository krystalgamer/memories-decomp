/*
 * Initializes one model/secondary-driver channel rooted at D_800F2C40.
 * Current best under gcc_2_8_1_g0: 394/394 instructions, opcode multiset
 * distance 0, and 14 differing words with no hard register assignments.
 *
 * The source has the complete reset, command scan, event dispatch, coordinate
 * linking, free-record selection, and retry shape. A function-scope fallback
 * handler and separate retry cursor reduce the residue without pins.
 *
 * Residual: four words in the D_800E9D98/D_800E9D9C address loads, eight in
 * retry-scan allocation, and two in the final stack-value reload order.
 * Original ASPSX reproduces MASPSX's symbolic-load expansion, so the remaining
 * work is source ownership and lifetime rather than assembler normalization.
 */
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

extern s32 func_8004D134(s32, GsTYPEUNIT *, void *, s32 *, s32 *);

void func_8004CB0C(s32 index, u8 *arg1, s32 arg2, s32 arg3)
{
    u8 *base;
    u8 *slot;
    u8 *search_slot;
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
    cursor = arg1;
    if (index < 2) {
        table = D_800E9D98;
    } else {
        table = D_800E9D9C;
    }
    count = 0;
    acc = 0;
    base[0xE13] = 0;
    base[0xE12] = 0;
    if (arg3 >= 0) {
        base[0xE11] = arg3 & 0x7F;
        if (arg3 & 0x80) {
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
    if (arg1 == 0) {
        base[0xE1D] = 0;
        base[0xE14] = 0xFF;
        return;
    }
    GsMapUnit((u32 *)arg1);
    cursor += 0xC;
    func_8004D58C(index, arg1);
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
                if (GsScanUnit(0, &ev, table, (u32 *)0x1F800000) == 0) {
                    goto evdone;
                }
                {
                    void *scratch;

                    tag = (u32)ev.type >> 24;
                    scratch = (void *)0x1F800000;
                    if (ev.type == 0) {
                        goto evloop;
                    }
                    {
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
                            GsMapCoordUnit((u32 *)arg1, (u32 *)ev.ptr);
                        ev.type &= 0xFF7FFFFF;
                    }
                dispatch:
                    switch (tag) {
                    case 0:
                        *(u16 *)(base + 0xE04) += func_8004D134(index, &ev, scratch, &acc, &count);
                        func_8006086C((ModelHandlerObject *)&ev);
                        break;
                    case 1:
                        *(u16 *)(base + 0xE04) += func_8004D134(index, &ev, scratch, 0, &count);
                        func_80060AEC((ModelHandlerObject *)&ev);
                        break;
                    case 2:
                        func_80060220(index, (u8 *)&ev, (u8 *)scratch);
                        break;
                    case 3:
                        func_8005C6A0((s32 *)&ev, base);
                        break;
                    default:
                    {
                        *ev.ptr = handler;
                        break;
                    }
                    }
                    }
                    goto evloop;
                }
            evdone: ;
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
        do {
            base[0xE17]++;
            if (*(s32 *)(slot + 4) != 0) {
                *(s32 *)slot = *(s32 *)(base + 0xD14) + off;
            }
            off += 0x50;
            i++;
            slot += 8;
        } while (i < base[0xE1A] - 1);
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
        for (;;) {
            search_slot = base;
            loaded_limit = base[0xE1A];
            i = 0;
            if (loaded_limit != 0) {
                sentinel = -1;
                limit = loaded_limit;
                do {
                    if (((GsUNIT *)search_slot)->coord != 0 &&
                        ((GsUNIT *)search_slot)->primtop != 0) {
                        if (*((GsUNIT *)search_slot)->primtop != sentinel ||
                            *(((GsUNIT *)search_slot)->primtop + 2) != 0) {
                            if (((GsUNIT *)search_slot)->coord->super == cur) {
                                break;
                            }
                        }
                    }
                    i++;
                    search_slot += 8;
                } while (i < limit);
                if (i < base[0xE1A]) {
                    break;
                }
            }
            next = func_8005A3D0(base, cur);
            if (!(next < base[0xE17])) {
                break;
            }
            cur = *(GsCOORDUNIT **)(base + 0xD14) + next;
        }
        rec = cur->super;
        if (rec != *(GsCOORDUNIT **)(base + 0xD18)) {
            *(GsCOORDUNIT **)(base + 0xD1C) = rec;
            if (rec != 0) {
                base[0xE19] = rec - *(GsCOORDUNIT **)(base + 0xD14);
            }
        }
    }
    if (*(s32 *)(base + 0xDE0) == 0) {
        *(s32 *)(base + 0xDE0) = arg2 + (s32)arg1;
    }
}
