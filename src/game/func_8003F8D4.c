#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "data_transfer_request.h"
#include "input.h"
#include "duel_effect.h"
#include "func_8003F8D4.h"
#include "text_box_lifecycle.h"

#include "mem_card.h"
#include "save_data.h"
#include "display_object_api.h"
#include "../unmatched.h"
#include "func_80039794.h"
#include "mem_card_dialog_runtime.h"
extern u8 D_801D2200[];

/* MATCH 2026-09-06, first-day function from the m2c draft (257 instructions,
 * the memory-card save state machine: format, load, verify, retry). Jump
 * table on the mode nibble, so only the full build proves it. Levers, in
 * order: the scalar arm for D_8009B3EA; gInput_wPad1Pressed on its .data VOLATILE arm
 * (retail reloads it for the second bit test and reads it bare); the call
 * arguments D_8009B3EE and D_8009B3C0 hoisted into locals BEFORE the +0x60
 * store that precedes the sprite call (-6 -> -1); ONE name for the sprite
 * pointer in cases 0 and 0xA, which is what puts it in $s0 (its live range
 * crosses the wait loop's calls in 0xA); the record passed to TextBox_Destroy
 * as its ADDRESS, index-first (`&D_800EB0F8[i]`; m2c had read it as a load);
 * case 3's `v = 0` before the D_8009B3F9 store reaching a SHARED
 * `return v` through a goto, so the zero stays a variable; and the flags read
 * through a block-local base `q = D_800EB0F8` so the 0x34 is a load
 * displacement rather than folded into %lo.
 */

s32 func_8003F8D4(void) {
    DuelEffectChannel *o;
    s32 v;
    s32 m;
    DuelEffectChannel *q;
    s32 a;
    s32 b;

    m = D_8009B3EA & 0xF;
    switch (m) {
    case 0:
        if ((D_8009B3EA & 0x80) == 0) {
            D_8009B3EA |= 0x80;
            func_8003F388();
            a = D_8009B3EE;
            *(s16 *)(D_8009B3D8 + 0x60) = -0x400;
            o = TextBox_Create(a, 0xC2, 0x20, 0x50, 0x100, 0x30);
            o->field_59 = 0x10;
            func_80039A14(o);
        }
        if (func_8003F2B0((DisplayObject *)D_8009B3D8, 0x20, 0x50,
                          D_8009B3EE) == 0) {
            D_8009B3EA = 1;
        }
        return 0;
    case 1:
        if (gInput_wPad1Pressed & 0x20) {
            SD_SEPlayFull(8);
            D_8009B3EA = 0x82;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        } else if (gInput_wPad1Pressed & 0x40) {
            SD_SEPlayFull(7);
            D_8009B3EA = 2;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        }
        return 0;
    case 2:
        if (func_8003F2B0((DisplayObject *)D_8009B3D8, 0x20, 0x100,
                          D_8009B3EE) == 0) {
            TextBox_Destroy(&D_800EB0F8[D_8009B3EE]);
            func_8004036C(D_8009B3D8);
            D_8009B3D8 = (u8 *)0;
            if ((D_8009B3EA & 0x80) == 0) {
                D_8009B3EA = 3;
                func_8003F758(
                    D_801D1200,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                return 0;
            }
            return 2;
        }
        return 0;
    case 3:
        v = func_8003F70C();
        if (v != 0) {
            if (v == 1) {
                if (D_8009B3EA & 0x40) {
                    if (SaveData_HasSameDuelistCode(
                            (SaveDataState *)D_801D1200,
                            (SaveDataState *)(D_801D1200 + 0x1000)) == 0) {
                        return 1;
                    }
                    D_8009B3EA = 0xA;
                    return 0;
                }
                D_8009B3EA |= 0x40;
                func_8003F758(
                    D_801D2200,
                    SAVE_DATA_STATE_SIZE,
                    gMemCard_szSaveFileName,
                    1
                );
                v = 0;
                D_8009B3F9 = 0x10;
                goto done;
            }
            return v;
        }
    done:
        return v;
    case 0xA:
        if ((D_8009B3EA & 0x80) == 0) {
            D_8009B3EA |= 0xC0;
            func_8003F388();
            a = D_8009B3EE;
            b = D_8009B3C0;
            *(s16 *)(D_8009B3D8 + 0x60) = -0x400;
            o = TextBox_CreateFlagged(a, b, 0x20, 0x50, 0x100, 0x30, 0x1008);
            o->field_59 = 0x10;
            do {
                func_80039794();
            } while ((o->flags_34 & TEXT_BOX_FLAG_DONE) == 0);
        }
        if (D_8009B3EA & 0x40) {
            if (func_8003F2B0((DisplayObject *)D_8009B3D8, 0x20, 0x50,
                              D_8009B3EE) == 0) {
                D_8009B3EA &= 0xBF;
            }
            return 0;
        }
        func_80039794();
        q = D_800EB0F8;
        if ((q[D_8009B3EE].flags_34 & 8) == 0) {
            D_8009B3EA = 0xB;
        }
        return 0;
    case 0xB:
        if ((D_8009B3EA & 0x80) == 0) {
            D_8009B3EA |= 0x80;
            *(s16 *)(D_8009B3D8 + 0x60) = 0x400;
        }
        if (func_8003F2B0((DisplayObject *)D_8009B3D8, 0x20, 0x100,
                          D_8009B3EE) == 0) {
            TextBox_Destroy(&D_800EB0F8[D_8009B3EE]);
            func_8004036C(D_8009B3D8);
            D_8009B3D8 = (u8 *)0;
            return 2;
        }
        return 0;
    }
    return 0;
}
