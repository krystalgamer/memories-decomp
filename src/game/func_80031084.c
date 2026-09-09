#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "duel_effect.h"
#include "func_8003B6AC.h"
#include "../psyq/rand.h"
#include "input.h"
#include "text_box_lifecycle.h"
#include "../unmatched.h"
#include "frontend_step_tables.h"

extern u8 D_8009AF54[];
extern u8 D_8009B2F0;
extern s8 D_8009B2F1;

void func_80031084(void) {
    DuelEffectChannel *boxes;
    s32 i;
    s8 d;
    s32 one;
    s32 k;
    s32 e;
    s32 t;

    rand();
    e = D_8009B2EB;
    i = 0;
    if (e != 0) {
        if (D_8009B2F0 != 0) {
            D_80090D7C[e & 0x1F]();
        } else {
            D_80090D84[e & 0x1F]();
        }
        return;
    }
    for (; i < 0x16; i++) {
        FntPrint(D_8009AF54);
    }
    if ((gInput_wPad1Repeat & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Repeat & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                if (D_8009B2F1 + 0xA < 0x14) {
                    D_8009B2F1 = D_8009B2F1 + 0xA;
                }
            } else {
                if (D_8009B2F1 - 0xA >= 0) {
                    D_8009B2F1 = D_8009B2F1 - 0xA;
                }
            }
        }
        if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
            t = (u8)D_8009B2F1;
            if (D_8009B2F1 >= 0xA) {
                d = t - 1;
                D_8009B2F1 = d;
                if (d < 0xA) {
                    D_8009B2F1 = 0x13;
                }
            } else {
                d = t - 1;
                D_8009B2F1 = d;
                if (d < 0) {
                    D_8009B2F1 = 9;
                }
            }
        }
        if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
            t = (u8)D_8009B2F1;
            if (D_8009B2F1 >= 0xA) {
                d = t + 1;
                D_8009B2F1 = d;
                if (d >= 0x14) {
                    D_8009B2F1 = 0xA;
                }
            } else {
                d = t + 1;
                D_8009B2F1 = d;
                if (d >= 0xA) {
                    D_8009B2F1 = 0;
                }
            }
        }
        func_800300C8();
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
        k = 0x13;
        if (D_8009B2F1 != k) {
            D_8009B2F1 = k;
            func_800300C8();
            return;
        }
        D_8009B2EB = 0x14;
        return;
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_SELECT) != 0) {
        one = 1;
        func_8003B6AC(one, one);
        D_8009B2F0 = D_8009B2F0 ^ one;
        TextBox_Create(1, D_8009B2F0 + 0xF, 0x10, 0x10, 0x120, 0xA0);
        /* The array base has to stay in a local: writing `&D_800EB0F8[1]`
         * directly folds the record offset into the address computation and
         * drops the `addiu` retail keeps for the call argument. */
        boxes = D_800EB0F8;
        boxes[1].field_5A = 0x10;
        boxes[1].field_5B = 0x10;
        func_80039A14(&boxes[1]);
        return;
    }
    if ((gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK) != 0) {
        D_8009B2EB = D_8009B2F1 + 1;
    }
}
