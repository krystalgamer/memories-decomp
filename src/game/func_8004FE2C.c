/*
 * D_8009AF9A phase dispatcher: `func_8004FE2C`
 *
 * The 1880-byte routine matches under the existing uniform
 * `gcc_2_8_1_g8_split` profile. It runs two module callbacks four bytes into
 * D_80010014 and D_80010018, then dispatches on (s8)(D_8009AF9A + 1) through a
 * 17-entry jump table.
 *
 * Three source dependencies are load-bearing; each was removed alone and
 * `make match` failed:
 *
 * - The eight-byte template at D_8009AFFC is copied as one ModelBytes8 block
 *   through its unknown-bound view. With the sized u32[2] declaration the
 *   executable is four bytes short.
 * - Step 15 compares `x - field_E0D * func_80058E1C() <= field_E06`. The
 *   operands the other way round, `field_E06 >= ...`, change a register
 *   byte at 0x8004FE76.
 * - Step 15 indexes its 0x76-byte record as `base + 0x7C4 - -(i * 0x76)`.
 *   The plain sum `base + i * 0x76 + 0x7C4` changes a register byte at
 *   0x8005051E.
 *
 * The two per-slot flags are read as D_800F2C40[0].field_E14 and
 * D_800F2C40[1].field_E15.
 *
 * No register pins, source-level inline assembly, symbol aliases, or new
 * profile is used.
 */
#define MODEL_HANDLER_TEMPLATE_AS_ARRAY
#include "../types.h"
#include "../psyq/libgte.h"
#include "fade.h"
#include "file_transfer.h"
#include "func_8004FE2C.h"
#include "func_80057AF4.h"
#include "func_80058E1C.h"
#include "func_8005D994.h"
#include "high_memory_addresses.h"
#include "model.h"
#include "model_effect_state.h"
#include "model_graphics_state.h"
#include "model_handler_state.h"
#include "model_slot_properties.h"
#include "model_slot_state_updates.h"
#include "model_state_setters.h"
#include "model_transfer_state.h"
#include "sound_output.h"
#include "../unmatched.h"

void func_8004FE2C(void)
{
    ModelBytes8 v;
    u8 *base;
    u8 *b0;
    u8 *b2;
    s32 (*cb1)(s32, s32);
    s32 (*cb2)(s32, s32);
    s32 x;

    v = *(ModelBytes8 *)D_8009AFFC;
    base = (u8 *)D_800F2C40;
    cb1 = (s32 (*)(s32, s32))(D_80010014 + 4);
    cb2 = (s32 (*)(s32, s32))(D_80010018 + 4);
    func_80059AEC(1);
    if ((u8)D_8009AF9A - 8U < 2 && base[0xE15] == 0) {
        D_8009AFA0 = 0;
        if (cb1(D_80010024, -1) == 2) {
            base[0xE15]++;
        }
    }
    if (D_8009AF9A >= 0xA && D_800F2C40[1].field_E15 == 0) {
        D_8009AFA0 = 0;
        if (cb2(D_80010028, -1) == 2) {
            func_80059700(0, 0);
            func_800156DC();
            D_8009AF9A = 0xF;
        }
    }
    switch ((s8)(D_8009AF9A + 1)) {
    case 0:
        if (D_800F2C40[0].field_E14 == 0xFF) {
            if (((D_8009B0F4_abs & 0x2000030) | D_8009B134_abs) == 0) {
                Model_LoadMonsterMerge(0, 0x309, 0, 0, 0, 0, 4);
            }
        } else {
            func_80056828(0);
        }
        b0 = (u8 *)D_800F2C40;
        if (b0[0xE1F] != 0) {
            func_8005F3B8(0, 0x2710, 0xE00, 0, 0);
            func_800597C8(0, 1, 0);
            if (base[0xE0F] != 0) {
                func_80057AF4(0, 0, 0);
            }
            func_80059700(0, 1);
            func_80059590(0, 5, 0, 0, 0);
            base[0xBF6] = 1;
            base[0xBF4] = 2;
            func_80047314(0x7310);
            *(s32 *)(b0 + 0xD08) = -1;
            *(s32 *)(b0 + 0xD0C) = -1;
            *(s32 *)(b0 + 0xD10) = -1;
            D_8009AFA0 = 0;
            cb1(D_80010024, 0);
            cb2(D_80010028, 0);
            b0[0xE15] = 0;
            b0[0x1C35] = 0;
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 1:
        if (func_8004703C() & 0x80) {
            *(s16 *)((u8 *)&v + 2) = -0x7D0;
            func_800597C8(0, 0, 0xA);
            func_8005D994(0, 0x7D0, 0x100, 0x100, (u8 *)&v, 0x6E);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 2:
        b2 = (u8 *)D_800F2C40;
        if ((s8)b2[0xDC0] >= 0) {
            b2[0xDC0] += 2;
            b2[0xDC1] += 2;
            b2[0xDC2] += 2;
        } else {
            func_80059590(0, 0, 0x80, 0x80, 0x80);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 3:
        if (func_8005FB08() != 0 && *(u16 *)(base + 0xE06) >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 4:
        if (*(u16 *)(base + 0xE06) >= 0x780) {
            func_800597C8(0, 0, 0);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 5:
        if (*(u16 *)(base + 0xE06) >= 0x1E0) {
            func_800597C8(0, 0, 0);
            *(s16 *)((u8 *)&v + 0) = -0x9C4;
            *(s16 *)((u8 *)&v + 2) = -0x5DC;
            *(s16 *)((u8 *)&v + 4) = 0x3E8;
            func_8005F3B8(0, 0xBB8, -0x100, -0x100, (SVECTOR *)&v);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 6:
        if (*(u16 *)(base + 0xE06) >= 0x1E0) {
            func_800597C8(0, 0, 0);
            *(s16 *)((u8 *)&v + 0) = 0x9C4;
            *(s16 *)((u8 *)&v + 2) = -0x5DC;
            *(s16 *)((u8 *)&v + 4) = 0x3E8;
            func_8005F3B8(0, 0xBB8, 0x100, -0x100, (SVECTOR *)&v);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 7:
        if (*(u16 *)(base + 0xE06) >= 0x1E0) {
            func_800597C8(0, 0, 0);
            *(s16 *)((u8 *)&v + 0) = -0x1F4;
            *(s16 *)((u8 *)&v + 2) = 0x5DC;
            *(s16 *)((u8 *)&v + 4) = 0x3E8;
            func_8005F3B8(0, 0xBB8, -0x200, -0x100, (SVECTOR *)&v);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 8:
        if (*(u16 *)(base + 0xE06) >= 0x1E0) {
            func_800597C8(0, 0, 0);
            *(s16 *)((u8 *)&v + 0) = 0x1F4;
            *(s16 *)((u8 *)&v + 2) = 0x5DC;
            *(s16 *)((u8 *)&v + 4) = 0x3E8;
            func_8005F3B8(0, 0xBB8, 0x200, -0x100, (SVECTOR *)&v);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 9:
        if (*(u16 *)(base + 0xE06) >= 0x1E0) {
            *(s16 *)((u8 *)&v + 2) = -0x7D0;
            func_8005F3B8(0, 0xDAC, 0x40, 0x200, (SVECTOR *)&v);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 10:
        if (base[0xBF5] == 2) {
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 11:
        if (*(u16 *)(base + 0xE06) >= 0x8C0) {
            *(s16 *)((u8 *)&v + 2) = -0x5DC;
            func_8005D994(0, 0xDAC, 0, -0x80, (u8 *)&v, 0x28);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 12:
        if (*(u16 *)(base + 0xE06) >= 0xE60) {
            *(s16 *)((u8 *)&v + 4) = -0x3E8;
            func_8005F3B8(0, 0x1388, 0x200, -0x100, (SVECTOR *)&v);
            func_800597C8(0, 0, 0xBE);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 13:
        if (*(u16 *)(base + 0xE06) >= 0xE60) {
            *(s16 *)((u8 *)&v + 4) = -0x3E8;
            func_8005F3B8(0, 0x1388, -0x200, -0x100, (SVECTOR *)&v);
            func_800597C8(0, 0, 0xBE);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 14:
        if (*(u16 *)(base + 0xE06) >= 0xE60) {
            *(s16 *)((u8 *)&v + 2) = -0x3E8;
            func_8005F3B8(0, 0xBB8, 0, -0x80, (SVECTOR *)&v);
            func_800597C8(0, 0, 0xBE);
            D_8009AF9A = D_8009AF9A + 1;
        }
        break;
    case 15:
        x = *(u16 *)(base + 0x7C4 - -(base[0xBF5] * 0x76)) * 16;
        if (x - base[0xE0D] * func_80058E1C() <= *(u16 *)(base + 0xE06)) {
            func_80059700(0, 0);
        }
        break;
    case 16:
        D_8009AF9A = -2;
        break;
    }
}
