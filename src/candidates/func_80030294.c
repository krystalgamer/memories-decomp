/*
 * Current best under gcc_2_8_1_cc_g8_as_g4_split: 324/329 instructions and
 * opcode distance 5. Sized volatile pad globals stay small data to GCC at
 * -G8, while assembler -G4 bare-symbol loads rematerialize through each
 * destination register. The three local initialized tables produce 0x38
 * bytes of fingerprinted .rodata. Residual: frame-address placement,
 * carry-add scheduling, one shared carry-zero store and the caret-clear loop.
 */
#define FRONTEND_DEBUG_ROW_VIEWS
#include "../types.h"
#include "../ygo_types.h"
#include "../game/debug_font_format_data.h"
#include "../game/frontend_debug_state.h"
#include "../unmatched.h"

extern u8 D_8009AF4C[];

extern volatile u16 gInput_wPad1Held[4];
extern volatile u16 gInput_wPad2Held[4];
extern volatile u16 gInput_wPad1Pressed[4];
extern volatile u16 gInput_wPad2Pressed[4];
extern volatile u16 gInput_wPad1Repeat[4];
extern volatile u16 gInput_wPad2Repeat[4];

extern void FntPrint(char *, ...);

s32 func_80030294(void)
{
    s32 dec[5] = {1, 10, 100, 1000, 10000};
    s32 rnd[4] = {0xA, 0x9A, 0x99A, 0x999A};
    s32 hex[5] = {1, 0x10, 0x100, 0x1000, 0x10000};
    u16 masks[4];
    s32 ret;
    s32 row;
    s32 digits;
    s32 flags;
    s32 i;
    s32 cursor;
    s32 value;
    s32 step;
    s32 mask;
    s32 carry;
    s32 n;
    u16 *slot;
    s32 *p;
    s32 d;
    u8 *text;

    *(Bytes8 *)masks = *(Bytes8 *)D_8009AF4C;
    ret = 0;
    row = (s8)D_8009B2DC;
    flags = D_8009B2EA;
    digits = D_8009B2C0[row];
    if ((flags & 0x80) == 0) {
        D_8009B2EA = flags | 0x80;
        if (((flags | 0x80) & 0x40) != 0) {
            i = (s8)digits - 1;
            slot = ((FrontendDebugValues *)&gDebug_nSceneOrSoundID)->row + row;
            p = dec;
            p = p + i;
            value = *slot;
            *slot = 0;
            do {
                d = *p;
                n = value / d;
                *slot = *slot | (n << (i * 4));
                value -= d * n;
                p--;
                i--;
            } while (i >= 0);
        }
        goto draw;
    }
    if (flags & 1) {
        goto draw;
    }
    if ((gInput_wPad1Held[0] | gInput_wPad2Held[0]) & 0x800) {
        goto draw;
    }
    if ((gInput_wPad1Pressed[0] | gInput_wPad2Pressed[0]) & 0xC0) {
        ret = 1;
        goto print;
    }
    if ((gInput_wPad1Pressed[0] | gInput_wPad2Pressed[0]) & 0x20) {
        ret = -1;
        goto print;
    }
    if ((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x5000) {
        slot = ((FrontendDebugValues *)&gDebug_nSceneOrSoundID)->row + row;
        cursor = (s8)D_8009B2E9;
        value = *slot;
        step = hex[cursor];
        if ((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x4000) {
            step = -step;
        }
        if (flags & 0x40) {
            mask = masks[cursor];
            for (i = cursor; i < (s8)digits; i++) {
                carry = (value & mask) + step;
                value = value & ~mask;
                if (step >= 0) {
                    if (carry < rnd[i]) {
                        break;
                    }
                    carry = 0;
                } else {
                    if (carry >= 0) {
                        break;
                    }
                    value = value | ((rnd[i] - 1) & mask);
                    carry = 0;
                }
                mask = mask << 4;
                step = step << 4;
            }
            value = value | carry;
        } else {
            value = (value + step) & (hex[(s8)digits] - 1);
        }
        ((FrontendDebugValues *)&gDebug_nSceneOrSoundID)->row[(s8)D_8009B2DC] = value;
    }
    if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0xA000) == 0) {
        goto print;
    }
    if ((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x2000) {
        D_8009B2E9 = D_8009B2E9 - 1;
        if ((s8)D_8009B2E9 < 0) {
            D_8009B2DC = D_8009B2DC + 1;
            if ((s8)D_8009B2DC >= (s8)D_8009B2E0) {
                D_8009B2DC = D_8009B2E0 - 1;
                D_8009B2E9 = 0;
            } else {
                D_8009B2E9 = D_8009B2C0[(s8)D_8009B2DC] - 1;
            }
        }
    } else {
        D_8009B2E9 = D_8009B2E9 + 1;
        if ((s8)D_8009B2E9 >= (s8)digits) {
            D_8009B2E9 = 0;
            D_8009B2DC = D_8009B2DC - 1;
            if ((s8)D_8009B2DC < 0) {
                D_8009B2DC = 0;
                D_8009B2E9 = D_8009B2C0[0] - 1;
            }
        }
    }
draw:
    text = D_800EAED8;
    for (i = 0x27; i >= 0; i--) {
        text[i] = ' ';
    }
    text = &D_800EAED8[D_8009B2B4[(s8)D_8009B2DC] - (s8)D_8009B2E9];
    text[0] = '*';
    text[1] = 0;
print:
    i = (s8)D_8009B2B8;
    if (i != 0) {
        do {
            FntPrint(D_8009AF54);
            i--;
        } while (i != 0);
    }
    FntPrint((char *)D_8009B2EC, gDebug_nSceneOrSoundID, D_8009B2CA, D_8009B2CC);
    FntPrint(D_8009AF58, D_800EAED8);
    return ret;
}
