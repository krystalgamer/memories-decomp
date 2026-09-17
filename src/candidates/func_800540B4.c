/*
 * Per-slot model draw pass: selects the slot's ordering table, programs the
 * three flat lights and the ambient colour, steps the slot's colour fade into
 * the shared primitive templates at D_8009AFAC..D_8009AFE8, sorts every unit
 * of the slot, then updates the slot's bounding volume, draws the ground
 * shadow fan and runs the palette flash. Current best under
 * gcc_2_8_1_g8_split: 1419 instructions against 1421 with opcode distance 52
 * (25 surplus, 27 missing), with no hard register assignments and no inline
 * assembly. This is a structural candidate: the control flow, call sequence
 * and store set are in place, the register and stack-slot assignment is not.
 *
 * Levers measured on this body:
 * - D_800E9D98 through its array arm and D_800E9D9C from .data, which is
 *   retail's %hi/%lo form for both ordering-table slots;
 * - the fade-complete test compares the whole colour word against 0x808080
 *   and the third channel unsigned;
 * - the slot's +0xCFC..+0xCFE offsets are signed bytes;
 * - the part rewrite index is a GsCOORDUNIT pointer difference, which retail
 *   computes as an exact division rather than a reciprocal multiply;
 * - both colour blocks' template switches take the default arm first,
 *   assign the template word in each case and store it once, which brings
 *   them close to retail's comparison chains;
 * - the light matrix normalisation counts down from 8;
 * - the six fade templates divide the colour bytes as s16 by 16, which
 *   gives retail's signed shifts;
 * - the bounding volume's maxx, maxy and maxz accumulators are u16, like
 *   minz, and are read through their (s16) casts;
 * - the shadow's hh is an s16, the width of every value it takes.
 *
 * Residual: the local frame is a single byte buffer indexed through SP();
 * retail keeps the CF8 block pointer in $s6 where this source spills it,
 * clamps the shadow vertices through named unsigned reloads, computes the
 * shadow's half height before its loop, and keeps the part rewrite constant
 * inside the unit loop where this source hoists it.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
#include "../game/model.h"
#include "../game/model_graphics_state.h"
#include "../game/model_handler_state.h"
#include "../game/model_primitive_templates.h"
#include "../game/model_record_tables.h"
#define ORDERING_TABLE_SLOT2_ARRAY
#include "../game/ordering_tables.h"
#include "../game/func_80058E1C.h"
#include "../game/func_80057AF4.h"
#include "../game/func_80057E20.h"
#include "../game/func_8005922C.h"
#include "../game/model_slot_state_updates.h"
#include "../game/func_8005A53C.h"
#include "../game/color.h"
#include "../game/gpu_packets.h"
#include "../game/screen_projection.h"
#include "../unmatched.h"

extern GsOT *D_800E9D9C __attribute__((section(".data")));

#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define SP(o) ((u8 *)buf + (o) - 0x20)

s32 func_8005FE44(s32 arg0);

void func_800540B4(s32 index)
{
    u8 buf[0x860];
    s32 sxy_z;
    s32 flag;
    GsOT *ot;
    s32 inrange;
    u8 *packet;
    s32 unused_894;
    u16 minz;
    ModelSlot *slot;
    u8 *blk;
    ModelSlotHeadEntry *e;
    GsCOORDUNIT *unit;
    GsCOORDUNIT *root;
    u8 *list;
    u8 *mp;
    s32 colour;
    s32 r;
    s32 g;
    s32 bl;
    s32 fl;
    s32 v;
    s32 t0;
    s32 lim;
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 hidden;
    s32 found;
    s32 max;
    s32 a;
    s32 count;
    s32 sx;
    s32 sy;
    s32 sz;
    s32 x;
    s32 y;
    s32 z;
    u16 maxx;
    s32 minx;
    u16 maxy;
    s32 miny;
    u16 maxz;
    s32 w;
    s32 hy;
    s32 dz;
    s32 cur;
    s32 half;
    s16 hh;
    u8 *tbl;
    s32 row;
    u16 *cp;

    if (index < 2) {
        ot = D_800E9D98[0];
    } else {
        ot = D_800E9D9C;
    }
    slot = &D_800F2C40[index];
    blk = (u8 *)&slot->field_CF8;
    if (slot->field_E1F == 0) {
        return;
    }
    if (index == 2 && slot->field_DA0[0] == -0x1000 && slot->field_DA0[1] == slot->field_DA0[0]
        && slot->field_DA0[2] == slot->field_DA0[1]) {
        return;
    }
    if (slot->field_E11 != 1 && slot->field_DC0[3] >= 2 && !(W(slot->field_DC0, 0) & 0xFFFFFF)) {
        return;
    }
    GsSetFlatLight(0, (GsF_LIGHT *)&slot->field_D70[0]);
    GsSetFlatLight(1, (GsF_LIGHT *)&slot->field_D70[1]);
    GsSetFlatLight(2, (GsF_LIGHT *)&slot->field_D70[2]);
    GsSetAmbient(slot->field_DA0[0], slot->field_DA0[1], slot->field_DA0[2]);
    D_8009AFE0 = slot->field_DF0;
    D_8009AFE4 = slot->field_E11;
    if (slot->field_E11 != 4) {
        slot->field_E11 = 0;
    }
    if (index < 2) {
        ot->length = 0xE;
        if ((slot->field_DC0[7] % 6) && slot->field_DC0[3] == 0 && D_8009AFE4 == 0) {
            t0 = func_80058E1C() * 2;
            bl = slot->field_DC0[6];
            g = slot->field_DC0[5];
            r = slot->field_DC0[4];
            fl = slot->field_DC0[7];
            colour = (bl << 16) | (g << 8) | r;
            if (fl != 0) {
                colour |= 0x02000000;
            }
            D_8009AFAC = colour | 0x24000000;
            D_8009AFB0 = colour | 0x2C000000;
            D_8009AFB4 = colour | 0x34000000;
            D_8009AFB8 = colour | 0x3C000000;
            D_8009AFCC = colour | 0x4A000000;
            D_8009AFD0 = colour | 0x4E000000;
            D_8009AFBC = colour | 0x26000000;
            D_8009AFC0 = colour | 0x2E000000;
            D_8009AFC4 = colour | 0x36000000;
            D_8009AFC8 = colour | 0x3E000000;
            B(&D_8009AFE6, 0) = r;
            B(&D_8009AFE6, 1) = g;
            D_8009AFE8 = bl;
            switch (fl) {
            default:
                D_8009AFD8 = 0;
                goto d8_done;
            case 4:
                v = 0x600000;
                break;
            case 2:
            case 5:
                v = 0x200000;
                break;
            case 3:
                v = 0x400000;
                break;
            }
            D_8009AFD8 = v;
            d8_done:
            if (fl == 5) {
                D_8009AFDC = 0x3C200000;
            } else {
                D_8009AFDC = 0;
            }
            D_8009AFE5 = (u32)(fl - 1) < 4;
            if (D_8009AFE5 != 0) {
                D_8009AFD4 = (((fl - 1) << 5) & 0x9FF) | 0xE1000200;
            } else {
                D_8009AFD4 = 0xE1000220;
            }
            D_8009AFE4 = 3;
            lim = slot->field_DC0[0];
            v = slot->field_DC0[4] + t0;
            if (v < lim) {
                lim = v;
            }
            cur = slot->field_DC0[1];
            slot->field_DC0[4] = lim;
            v = slot->field_DC0[5] + t0;
            if (v < cur) {
                cur = v;
            }
            a = slot->field_DC0[2];
            v = slot->field_DC0[6] + t0;
            slot->field_DC0[5] = cur;
            if (v < a) {
                a = v;
            }
            slot->field_DC0[6] = a;
            if (slot->field_DC0[4] >= slot->field_DC0[0] && slot->field_DC0[5] >= slot->field_DC0[1]
                && (u32)(a & 0xFF) >= slot->field_DC0[2]) {
                goto fade_done;
            }
        } else if (D_8009AFE4 != 1) {
            bl = slot->field_DC0[2];
            g = slot->field_DC0[1];
            r = slot->field_DC0[0];
            fl = slot->field_DC0[3];
            colour = (bl << 16) | (g << 8) | r;
            if (fl != 0) {
                colour |= 0x02000000;
            }
            D_8009AFAC = colour | 0x24000000;
            D_8009AFB0 = colour | 0x2C000000;
            D_8009AFB4 = colour | 0x34000000;
            D_8009AFB8 = colour | 0x3C000000;
            D_8009AFCC = colour | 0x4A000000;
            D_8009AFD0 = colour | 0x4E000000;
            D_8009AFBC = colour | 0x26000000;
            D_8009AFC0 = colour | 0x2E000000;
            D_8009AFC4 = colour | 0x36000000;
            D_8009AFC8 = colour | 0x3E000000;
            B(&D_8009AFE6, 0) = r;
            B(&D_8009AFE6, 1) = g;
            D_8009AFE8 = bl;
            switch (fl) {
            default:
                D_8009AFD8 = 0;
                goto d8b_done;
            case 4:
                v = 0x600000;
                break;
            case 3:
                v = 0x400000;
                break;
            case 2:
            case 5:
                v = 0x200000;
                break;
            }
            D_8009AFD8 = v;
            d8b_done:
            if (fl == 5) {
                D_8009AFDC = 0x3C200000;
            } else {
                D_8009AFDC = 0;
            }
            D_8009AFE5 = (u32)(fl - 1) < 4;
            if (D_8009AFE5 != 0) {
                D_8009AFD4 = (((fl - 1) << 5) & 0x9FF) | 0xE1000200;
            } else {
                D_8009AFD4 = 0xE1000220;
            }
            if (W(slot->field_DC0, 0) != 0x808080) {
                D_8009AFE4 = 3;
                slot->field_DC0[7] = 0;
            } else {
            fade_done:
                slot->field_DC0[7] = 0;
            }
        } else {
            goto neutral;
        }
    } else {
    neutral:
        B(&D_8009AFE6, 0) = 0x80;
        B(&D_8009AFE6, 1) = 0x80;
        D_8009AFE8 = 0x80;
        D_8009AFAC = 0x24808080;
        D_8009AFB0 = 0x2C808080;
        D_8009AFB4 = 0x34808080;
        D_8009AFB8 = 0x3C808080;
        D_8009AFCC = 0x4A808080;
        D_8009AFD0 = 0x4E808080;
        D_8009AFBC = 0x26808080;
        D_8009AFC0 = 0x2E808080;
        D_8009AFC4 = 0x36808080;
        D_8009AFC8 = 0x3E808080;
        D_8009AFD8 = 0;
        D_8009AFDC = 0;
        D_8009AFE5 = 0;
        D_8009AFD4 = 0xE1000220;
    }

    e = slot->field_000;
    if (D_8009AFE4 >= 3) {
        D_8009AFE0 = 0;
    }
    for (i = 0; i < slot->field_E1A; i++, e++) {
        hidden = 0;
        if (D_8009AF9B != 0 && slot->entry_count < i) {
            continue;
        }
        if (D_8009AF9C != 0 && i > 0 && !(slot->entry_count < i)) {
            j = i - 1;
            if (!((((u8 *)D_8009AF9C)[j / 8] >> (j % 8)) & 1)) {
                hidden = 1;
                if (D_8009AF9B != 0) {
                    continue;
                }
            }
        }
        if (e->field_04 == 0) {
            continue;
        }
        root = slot->field_D1C;
        if (root != 0) {
            unit = (GsCOORDUNIT *)e->field_00;
            if (unit->super == root) {
                k = 0;
                found = 0;
                n = slot->field_E1B;
                if (n != 0) {
                    do {
                        if (slot->field_1E0[k]->rewrite_idx
                            == (unit - (GsCOORDUNIT *)slot->entries) * 0x14 + 0x04000001) {
                            if (slot->field_1E0[k]->aframe == 0xFFFF
                                && ((slot->field_BEC[k / 8] >> (k % 8)) & 1)
                                && slot->field_750[slot->field_BF5].values[k] != 0) {
                                found = 1;
                            }
                            break;
                        }
                        k++;
                    } while (k < n);
                }
                if (found) {
                    ScaleMatrix((MATRIX *)((u8 *)e->field_00 + 4), (VECTOR *)&slot->field_DB0);
                } else {
                    func_8005922C((GsCOORDUNIT *)e->field_00, &slot->field_DB0);
                }
            }
        }
        list = e->field_04;
        if (W(list, 0) != -1 || W(list, 8) != 0) {
            if (hidden) {
                memset(SP(0x20), 0, 0x20);
                mp = SP(0x20);
                goto set_ls;
            }
            unit = (GsCOORDUNIT *)e->field_00;
            if (unit != 0) {
                if (D_8009AFE4 != 0) {
                    GsGetLwsUnit(unit, (MATRIX *)SP(0x60), (MATRIX *)SP(0x40));
                    max = 0;
                    cp = (u16 *)SP(0x60);
                    k = 8;
                    do {
                        a = (s16)*cp;
                        if (a < 0) {
                            a = -a;
                        }
                        if ((u16)max < a) {
                            max = a;
                        }
                        k--;
                        cp++;
                    } while (k >= 0);
                    max = (u16)max;
                    cp = (u16 *)SP(0x60);
                    if (max != 0) {
                        k = 0;
                        do {
                            k++;
                            *cp = ((s16)*cp << 12) / max;
                            cp++;
                        } while (k < 9);
                    } else {
                        S(SP(0x60), 16) = 0x1000;
                        S(SP(0x60), 8) = 0x1000;
                        S(SP(0x60), 0) = 0x1000;
                    }
                    GsSetLightMatrix((MATRIX *)SP(0x60));
                    mp = SP(0x40);
                } else {
                    GsGetLsUnit(unit, (MATRIX *)SP(0x40));
                    mp = SP(0x40);
                }
            set_ls:
                GsSetLsMatrix((MATRIX *)mp);
            }
            GsSortUnit((GsUNIT *)e, ot, (u32 *)0x1F800000);
        }
    }

    if (D_8009AF9C != 0) {
        if (index < 2) {
            ot->length = 0xC;
        }
        return;
    }
    if (index < 2) {
        minx = 0x7FFF;
        root = slot->field_D1C;
        if (root != 0) {
            miny = 0x7FFF;
            maxx = -0x8000;
            maxy = -0x8000;
            maxz = -0x8000;
            sz = 0;
            sy = 0;
            sx = 0;
            count = 0;
            minz = 0x7FFF;
            e = slot->field_000;
            for (i = 0; i < slot->field_E1A; i++, e++) {
                unit = (GsCOORDUNIT *)e->field_00;
                if (unit != 0 && e->field_04 != 0
                    && (W(e->field_04, 0) != -1 || W(e->field_04, 8) != 0)) {
                    count++;
                    sx += unit->workm.t[0];
                    sz += unit->workm.t[2];
                    sy += unit->workm.t[1];
                }
            }
            if (count != 0) {
                x = sx / count;
                y = sy / count;
                z = sz / count;
            } else {
                x = root->workm.t[0];
                y = root->workm.t[1];
                z = root->workm.t[2];
            }
            v = (s8)blk[4] * 0x10;
            if (index > 0) {
                x = x - v;
            } else {
                x = x + v;
            }
            slot->field_DD0[0] = x;
            slot->field_DD0[1] = y + (s8)blk[5] * 0x10;
            v = (s8)blk[6] * 0x10;
            if (index > 0) {
                z = z - v;
            } else {
                z = z + v;
            }
            sy = 0;
            count = 0;
            slot->field_DD0[2] = z;
            e = slot->field_000;
            for (i = 0; i < slot->field_E1A; i++, e++) {
                unit = (GsCOORDUNIT *)e->field_00;
                if (unit != root && unit != slot->field_D18 && unit != 0 && e->field_04 != 0) {
                    if (unit->super == root) {
                        count++;
                        sy += unit->workm.t[1];
                    }
                    if (W(e->field_04, 0) != -1 || W(e->field_04, 8) != 0) {
                        a = unit->workm.t[0];
                        cur = unit->workm.t[2];
                        v = unit->workm.t[1];
                        if ((s16)maxx < a) {
                            maxx = a;
                        }
                        if (a < (s16)minx) {
                            minx = a;
                        }
                        if ((s16)maxy < v) {
                            maxy = v;
                        }
                        if (v < (s16)miny) {
                            miny = v;
                        }
                        if ((s16)maxz < cur) {
                            maxz = cur;
                        }
                        if (cur < (s16)minz) {
                            minz = cur;
                        }
                    }
                }
            }
            if (count != 0) {
                y = sy / count;
            } else {
                y = root->workm.t[1];
            }
            w = (s16)maxx - (s16)minx;
            dz = (s16)maxz - (s16)minz;
            slot->field_DD0[3] = (y - 700) / 3 + (s8)blk[5] * 0x10;
            if ((s16)slot->field_DC8[3] < w) {
                slot->field_DC8[3] = w;
            }
            if ((s16)slot->field_DC8[3] < dz) {
                slot->field_DC8[3] = dz;
            }
            w = (s16)(w + 0x32);
            hy = (s16)((s16)maxy - (s16)miny + 0x32);
            dz = (s16)(dz + 0x32);
            v = (s16)slot->field_DC8[0];
            if (v != 0) {
                if (w < v) {
                    a = v - w;
                    if (a >= 0x15) {
                        a = 0x14;
                    }
                    slot->field_DC8[0] = slot->field_DC8[0] - a;
                } else {
                    a = w - v;
                    if (a >= 0x15) {
                        a = 0x14;
                    }
                    slot->field_DC8[0] = slot->field_DC8[0] + a;
                }
            } else {
                slot->field_DC8[0] = w;
            }
            v = (s16)slot->field_DC8[1];
            if (v != 0) {
                if (hy < v) {
                    a = v - hy;
                    if (a >= 0x15) {
                        a = 0x14;
                    }
                    slot->field_DC8[1] = slot->field_DC8[1] - a;
                } else {
                    a = hy - v;
                    if (a >= 0x15) {
                        a = 0x14;
                    }
                    slot->field_DC8[1] = slot->field_DC8[1] + a;
                }
            } else {
                slot->field_DC8[1] = hy;
            }
            v = (s16)slot->field_DC8[2];
            if (v != 0) {
                if (dz < v) {
                    a = v - dz;
                    if (a >= 0x15) {
                        a = 0x14;
                    }
                    slot->field_DC8[2] = slot->field_DC8[2] - a;
                } else {
                    a = dz - v;
                    if (a >= 0x15) {
                        a = 0x14;
                    }
                    slot->field_DC8[2] = slot->field_DC8[2] + a;
                }
            } else {
                slot->field_DC8[2] = dz;
            }
            if (slot->field_E12 != 0) {
                if (D_8009AFE4 == 1) {
                    goto finish_ot;
                }
                B(SP(0x60), 3) = 6;
                B(SP(0x60), 7) = 0x30;
                r = B(&D_8009AFE6, 0);
                B(SP(0x60), 4) = r;
                B(SP(0x60), 5) = B(&D_8009AFE6, 1);
                B(SP(0x60), 6) = D_8009AFE8;
                B(SP(0x60), 0xC) = (s16)r / 16;
                B(SP(0x60), 0xD) = (s16)B(&D_8009AFE6, 1) / 16;
                B(SP(0x60), 0xE) = (s16)D_8009AFE8 / 16;
                B(SP(0x60), 0x14) = (s16)r / 16;
                B(SP(0x60), 0x15) = (s16)B(&D_8009AFE6, 1) / 16;
                B(SP(0x60), 0x16) = (s16)D_8009AFE8 / 16;
                GsSetLsMatrix((MATRIX *)func_80059220());
                func_80057E20(index, (void *)SP(0x58));
                half = 0x12C;
                if (S(SP(0x58), 0) >= 0x12C) {
                    half = 0x320;
                    if (S(SP(0x58), 0) < 0x321) {
                        half = S(SP(0x58), 0);
                    }
                }
                if (S(SP(0x58), 4) >= 0x12C) {
                    hh = 0x320;
                    if (S(SP(0x58), 4) < 0x321) {
                        hh = S(SP(0x58), 4);
                    }
                } else {
                    hh = 0x12C;
                }
                if (D_8009AF92 != 0x8000) {
                    y = (s16)D_8009AF92;
                } else {
                    y = D_800F2C40[2].field_D18->matrix.t[1] + S(D_8009AF88, 0xA2);
                }
                half = half / 2;
                tbl = (u8 *)D_800914E8;
                inrange = x < 0x4B1;
                packet = SP(0x60);
                S(SP(0x40), 0) = x;
                S(SP(0x40), 2) = y;
                S(SP(0x40), 4) = z;
                S(SP(0x50), 0) = x + half;
                S(SP(0x50), 2) = y;
                S(SP(0x50), 4) = z;
                S(SP(0x48), 2) = y;
                for (i = 0; i < 8; i++, tbl += 4) {
                    S(SP(0x40), 0) = x;
                    S(SP(0x40), 4) = z;
                    S(SP(0x48), 0) = x + S(tbl, 0) * half / 4096;
                    S(SP(0x48), 4) = z + S(tbl, 2) * (hh / 2) / 4096;
                    if (B(D_8009AF88, 0xA1) & 1) {
                        v = -0x4B0;
                        if (x >= -0x4B0) {
                            v = x;
                            if (inrange == 0) {
                                v = 0x4B0;
                            }
                        }
                        S(SP(0x40), 0) = v;
                        if (S(SP(0x40), 4) < -0x4B0) {
                            S(SP(0x40), 4) = -0x4B0;
                        } else if (S(SP(0x40), 4) >= 0x4B1) {
                            S(SP(0x40), 4) = 0x4B0;
                        }
                        if (S(SP(0x48), 0) < -0x4B0) {
                            S(SP(0x48), 0) = -0x4B0;
                        } else if (S(SP(0x48), 0) >= 0x4B1) {
                            S(SP(0x48), 0) = 0x4B0;
                        }
                        if (S(SP(0x48), 4) < -0x4B0) {
                            S(SP(0x48), 4) = -0x4B0;
                        } else if (S(SP(0x48), 4) >= 0x4B1) {
                            S(SP(0x48), 4) = 0x4B0;
                        }
                        if (S(SP(0x50), 0) < -0x4B0) {
                            S(SP(0x50), 0) = -0x4B0;
                        } else if (S(SP(0x50), 0) >= 0x4B1) {
                            S(SP(0x50), 0) = 0x4B0;
                        }
                        if (S(SP(0x50), 4) < -0x4B0) {
                            S(SP(0x50), 4) = -0x4B0;
                        } else if (S(SP(0x50), 4) >= 0x4B1) {
                            S(SP(0x50), 4) = 0x4B0;
                        }
                        if (S(SP(0x48), 0) == S(SP(0x50), 0)) {
                            a = S(SP(0x48), 0);
                            v = S(SP(0x40), 0);
                            if (a < 0) {
                                a = -a;
                            }
                            if (v < 0) {
                                v = -v;
                            }
                            if (v >= a) {
                                goto skip;
                            }
                        }
                        if (S(SP(0x48), 4) == S(SP(0x50), 4)) {
                            a = S(SP(0x48), 4);
                            v = S(SP(0x40), 4);
                            if (a < 0) {
                                a = -a;
                            }
                            if (v < 0) {
                                v = -v;
                            }
                            if (v >= a) {
                                goto skip;
                            }
                        }
                    }
                    flag = RotAverage3((SVECTOR *)SP(0x40), (SVECTOR *)SP(0x48), (SVECTOR *)SP(0x50),
                        (long *)SP(0x68), (long *)SP(0x70), (long *)SP(0x78),
                        (long *)&sxy_z, (long *)&flag);
                    r = flag;
                    sxy_z = NormalClip(W(SP(0x68), 0), W(SP(0x70), 0), W(SP(0x78), 0));
                    if (r >= 0 && flag >= 0) {
                        func_8005B260((u32 *)packet, ot, 0xFFF, 2);
                    }
                skip:
                    *(ModelBytes8 *)SP(0x50) = *(ModelBytes8 *)SP(0x48);
                }
            }
        }
    }
    if (D_8009AFE4 == 1) {
    finish_ot:
        if (index < 2) {
            ot->length = 0xC;
        }
        return;
    }
    if (slot->field_E10 != 0) {
        func_80057AF4(index, -1, 0);
    } else {
        v = slot->field_E0F;
        if (v != 0) {
            if (v != slot->field_BF5) {
                func_800597C8(index, v, 0);
            } else if (slot->field_E16 != 0x23) {
                slot->field_BF6 = 0;
            }
        }
    }
    fl = slot->field_E13;
    if (fl != 0 && fl != 0xFF) {
        if (fl == 0xFE) {
            S(SP(0x40), 0) = index << 8;
            S(SP(0x40), 2) = 0xF0;
            S(SP(0x40), 4) = 0x100;
            S(SP(0x40), 6) = 4;
            for (i = 0, row = 0xF0; i < 8; i += 4, row += 4) {
                S(SP(0x40), 2) = row;
                while (IsIdleGPU(3) != 0) {
                }
                while (StoreImage2((RECT *)SP(0x40), (u32 *)SP(0x80)) != 0) {
                }
                while (IsIdleGPU(3) != 0) {
                }
                cp = (u16 *)SP(0x80);
                for (k = 0; k < 0x400; k++, cp++) {
                    if (*cp != 0) {
                        *cp |= 0x8000;
                    }
                }
                S(SP(0x40), 2) = row;
                while (IsIdleGPU(3) != 0) {
                }
                while (LoadImage2((RECT *)SP(0x40), (u32 *)SP(0x80)) != 0) {
                }
                while (IsIdleGPU(3) != 0) {
                }
            }
            func_80059700(index, 0);
            slot->field_E13 = 0;
        } else {
            slot->field_E13 = fl - 1;
            S(SP(0x68), 2) = 0xF8;
            S(SP(0x68), 4) = 2;
            S(SP(0x68), 6) = 8;
            S(SP(0x68), 0) = (index << 8) + ((fl - 1) & 0xFF) * 2;
            while (IsIdleGPU(3) != 0) {
            }
            while (StoreImage2((RECT *)SP(0x68), (u32 *)SP(0x48)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            cp = (u16 *)SP(0x48);
            for (k = 0; k < 0x10; k++, cp++) {
                *cp = func_8005AE68(*cp, 6, 0x1000) & 0x7FFF;
            }
            S(SP(0x68), 2) = 0xF0;
            while (IsIdleGPU(3) != 0) {
            }
            while (LoadImage2((RECT *)SP(0x68), (u32 *)SP(0x48)) != 0) {
            }
            while (IsIdleGPU(3) != 0) {
            }
            if (slot->field_E13 == 0) {
                ModelSlot *s = &D_800F2C40[index];
                if (s->field_E1F != 0) {
                    e = s->field_000;
                    for (i = 0; i < s->field_E1A; i++, e++) {
                        list = e->field_04;
                        if (list != 0) {
                            do {
                                if (W(list, 8) != 0) {
                                    func_8005A53C(func_8005FE44, (ModelHandlerRunEntry *)(list + 0xC),
                                                  W(list, 4), W(list, 8));
                                }
                                list = (u8 *)W(list, 0);
                            } while (list != (u8 *)-1);
                        }
                    }
                }
                slot->field_E13 = 0xFE;
            }
        }
    }
    if (index < 2) {
        ot->length = 0xC;
    }
}
