/*
 * Current best under gcc_2_8_1_cc_g8_as_g4_split: 329/329 instructions and
 * opcode distance 0. Sized volatile pad globals stay small data to GCC at
 * -G8, while assembler -G4 bare-symbol loads rematerialize through each
 * destination register. The three local initialized tables produce 0x38
 * bytes of fingerprinted .rodata.
 *
 * The body follows levers measured one at a time on a parked copy of this
 * function in the ygofm tree:
 *  - each arm spells the row address as two assignments against one name,
 *    `r = (u8 *)&gDebug_nSceneOrSoundID; r = r + (s8)D_8009B2DC * 2;`;
 *  - the carry and division loops index the local tables directly, so GCC
 *    creates the stack-base givs inside the arm instead of in the entry block;
 *  - the entry arm's counter, the carry and the quotient each have their own
 *    name (`jj`, `cc`, `qq`); each alone measures as a regression, together
 *    they close the allocation;
 *  - the caret-clear block starts from the buffer address, with 0x20 named
 *    between the base and the counter;
 *  - `val = val + step; val = val & mask;` as two statements, and the masks
 *    table read as `db - -(e * 2)` for a base-first addu.
 *
 * Residual: in both arms the row-address sum emits the base
 * (`addiu $v1,$gp,%gp_rel(gDebug_nSceneOrSoundID)`) before the index shift,
 * and lands in $v1 where retail uses $v0.
 */
#define FRONTEND_DEBUG_ROW_VIEWS
#define D_8009AF4C_IS_AGGREGATE
#define GINPUT_PAD1_HELD_SIZED_VOLATILE
#define GINPUT_PAD2_HELD_SIZED_VOLATILE
#define GINPUT_PAD1_PRESSED_SIZED_VOLATILE
#define GINPUT_PAD2_PRESSED_SIZED_VOLATILE
#define GINPUT_PAD1_REPEAT_SIZED_VOLATILE
#define GINPUT_PAD2_REPEAT_SIZED_VOLATILE
#include "../types.h"
#include "../ygo_types.h"
#include "../game/debug_font_format_data.h"
#include "../game/frontend_debug_state.h"
#include "../game/frontend_debug_constants.h"
#include "../game/input.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../unmatched.h"

s32 func_80030294(void)
{
    s32 a[5] = {1, 10, 100, 1000, 10000};
    s32 b[4] = {0xA, 0x9A, 0x99A, 0x999A};
    s32 c[5] = {1, 0x10, 0x100, 0x1000, 0x10000};
    u8 d[8];
    s32 ret;
    s32 i;
    s32 n;
    s32 e;
    s32 step;
    u8 *db;
    s32 k;
    u8 *z;
    u16 *p;
    s32 val;
    s32 mask;
    u8 t2;
    u8 f;
    u8 *r;
    s32 sc;
    s32 jj;
    s32 cc;
    s32 qq;

    ret = 0;
    *(Bytes8 *)d = *(Bytes8 *)D_8009AF4C;
    db = d;
    t2 = D_8009B2C0[(s8)D_8009B2DC];
    if ((D_8009B2EA & 0x80) == 0) {
        D_8009B2EA = D_8009B2EA | 0x80;
        if ((D_8009B2EA & 0x40) != 0) {
            jj = (s8)t2 - 1;
            r = (u8 *)&gDebug_nSceneOrSoundID;
            r = r + (s8)D_8009B2DC * 2;
            p = (u16 *)r;
            val = *p;
            *p = 0;
            do {
                k = a[jj];
                qq = val / k;
                *p = *p | (qq << (jj * 4));
                jj = jj - 1;
                val = val - k * qq;
            } while (jj >= 0);
        }
        goto fill;
    }
    if ((D_8009B2EA & 1) != 0) {
        goto fill;
    }
    if (((gInput_wPad1Held[0] | gInput_wPad2Held[0]) & 0x800) != 0) {
        goto fill;
    }
    if (((gInput_wPad1Pressed[0] | gInput_wPad2Pressed[0]) & 0xC0) != 0) {
        ret = 1;
        goto out;
    }
    if (((gInput_wPad1Pressed[0] | gInput_wPad2Pressed[0]) & 0x20) != 0) {
        ret = -1;
        goto out;
    }
    if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x5000) != 0) {
        r = (u8 *)&gDebug_nSceneOrSoundID;
        r = r + (s8)D_8009B2DC * 2;
        p = (u16 *)r;
        e = (s8)D_8009B2E9;
        val = *p;
        step = c[e];
        if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x4000) != 0) {
            step = -step;
        }
        if ((D_8009B2EA & 0x40) != 0) {
            mask = *(u16 *)(db - -(e * 2));
            i = e;
            if (i < (s8)t2) {
                n = (s8)t2;
                do {
                    cc = val & mask;
                    val = val & ~mask;
                    cc = cc + step;
                    if (step >= 0) {
                        if (cc < b[i]) {
                            goto joined;
                        }
                    } else {
                        if (cc >= 0) {
                            goto joined;
                        }
                        val = val | ((b[i] - 1) & mask);
                    }
                    cc = 0;
                    mask = mask * 0x10;
                    step = step * 0x10;
                    i = i + 1;
                } while (i < n);
            }
        joined:
            val = val | cc;
        } else {
            val = val + step;
            val = val & (c[(s8)t2] - 1);
        }
        ((u16 *)&gDebug_nSceneOrSoundID)[(s8)D_8009B2DC] = val;
    }
    if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0xA000) == 0) {
        goto out;
    }
    if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x2000) != 0) {
        D_8009B2E9 = D_8009B2E9 - 1;
        if ((s8)D_8009B2E9 < 0) {
            D_8009B2DC = D_8009B2DC + 1;
            if ((s8)D_8009B2DC >= (s8)D_8009B2E0) {
                D_8009B2DC = D_8009B2E0 - 1;
                D_8009B2E9 = 0;
                goto fill;
            }
            f = D_8009B2C0[(s8)D_8009B2DC] - 1;
            goto setpos;
        }
    } else {
        D_8009B2E9 = D_8009B2E9 + 1;
        if ((s8)D_8009B2E9 >= (s8)t2) {
            D_8009B2E9 = 0;
            D_8009B2DC = D_8009B2DC - 1;
            if ((s8)D_8009B2DC < 0) {
                f = D_8009B2C0[0] - 1;
                D_8009B2DC = 0;
            setpos:
                D_8009B2E9 = f;
            }
        }
    }
fill:
    r = D_800EAED8;
    sc = 0x20;
    i = 0x27;
    z = r + i;
    do {
        *z = sc;
        i = i - 1;
        z = z - 1;
    } while (i >= 0);
    r = &D_800EAED8[(s8)D_8009B2B4[(s8)D_8009B2DC] - (s8)D_8009B2E9];
    r[0] = 0x2A;
    r[1] = 0;
out:
    i = (s8)D_8009B2B8;
    if (i != 0) {
        do {
            FntPrint(D_8009AF54);
            i = i - 1;
        } while (i != 0);
    }
    FntPrint((char *)D_8009B2EC, gDebug_nSceneOrSoundID, D_8009B2CA, D_8009B2CC);
    FntPrint(D_8009AF58, D_800EAED8);
    return ret;
}
