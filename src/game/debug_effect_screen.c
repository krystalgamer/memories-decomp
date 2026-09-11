#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "func_800291E0.h"
#include "duel_effect_resource_setup.h"
#include "main_frame.h"
#include "display_object_api.h"
#include "duel_card.h"
#include "file_transfer.h"
#include "func_8001944C.h"
#include "input.h"
#include "view_state.h"
#include "display_object.h"
#include "duel_card_display_state.h"
#include "../unmatched.h"
#include "duel_screen_tables.h"
#include "debug_effect_screen.h"

/* The developer effect-preview screen: a pad-driven camera and viewport
   nudge, the controller that builds one of four preview pages and spawns an
   effect on CROSS, and the HUD line that prints the two tuned values. The
   three call each other in that order and are the whole gcc_2_8_1_g8_split
   run between func_80021F80 and func_80022674. */

extern s32 D_8009B30C __attribute__((section(".data")));

extern u8 D_8009B16C[4];
/* The tuned pair the HUD prints: D_8009AF2A selects which of the two
   D_8009AF2C entries the up/down repeat adjusts. */
extern u8 D_8009AF2E;
extern u8 *D_8009B180;
extern u8 *D_8009B184;
extern DuelCardRecord D_801A7B80[];

/* "~c0702D EFFECT = %2d %2d\n" */
/* "               **\n~c777\0" */
/* "            **\n~c777\0" */

void func_800220B8(void) {
    ViewState *b;
    ViewState *c;
    s32 a;
    s32 v;
    s32 x;
    s32 y;

    if ((gInput_wPad1Pressed & PAD_BUTTON_SELECT) != 0) {
        D_8009B30C = D_8009B30C ^ 1;
    }

    b = &D_800F2848;

    if ((gInput_wPad1Repeat & PAD_BUTTON_L1_R1_MASK) != 0) {
        a = 2;
        if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
            a = 0x10;
        }
        v = D_800F2848.field_00 + a;
        if ((gInput_wPad1Repeat & PAD_BUTTON_L1) != 0) {
            v -= a * 2;
        }
        D_800F2848.field_00 = v;
        func_8001352C();
    }

    if ((gInput_wPad1Repeat & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_BUTTON_TRIANGLE) != 0) {
            a = 2;
            if ((gInput_wPad1Held & PAD_BUTTON_CROSS) != 0) {
                a = 0x10;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                b->view.vrx = b->view.vrx + a;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
                b->view.vrz = b->view.vrz - a;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_LEFT) != 0) {
                b->view.vrx = b->view.vrx - a;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
                b->view.vrz = b->view.vrz + a;
            }
        } else {
            v = 0x20;
            if ((gInput_wPad1Held & PAD_BUTTON_CIRCLE) != 0) {
                v = 0x80;
            }
            y = b->angle;
            x = b->field_04;
            if ((gInput_wPad1Repeat & PAD_DIRECTION_UP) != 0) {
                x += v;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_DOWN) != 0) {
                x -= v;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                y -= v;
            }
            if ((gInput_wPad1Repeat & PAD_DIRECTION_LEFT) != 0) {
                y += v;
            }
            c = &D_800F2848;
            c->angle = y;
            c->field_04 = x;
        }
        func_8001352C();
    }
}

/* Debug display controller: START hands the pad to func_800220B8; on the
   first call it initialises the D_8009B23A mode flags and the cursor state.
   With the rebuild flag set it releases the two spawned objects and
   recreates the display for the current page (a message box, one or two
   card objects from D_801A7B80). Otherwise SELECT cycles the page (0..3),
   left/right toggles which coordinate the up/down repeat adjusts, and CROSS
   places a new object through func_8002C604 at a page-dependent position. */
void func_800222F4(void) {
    DisplayObject *obj;
    u8 *p;

    if (gInput_wPad1Held & PAD_BUTTON_START) {
        func_800220B8();
    }
    if ((D_8009B23A & 0x8000) == 0) {
        D_8009B23A |= 0xC000;
        D_8009B16C[2] = 0;
        D_8009AF2E = 0;
        D_8009AF2A = 0;
        D_8009AF2D = 0;
        D_8009B184 = 0;
        D_8009B180 = 0;
    }
    if (D_8009B23A & 0x4000) {
        D_8009B23A &= 0xBFFF;
        func_80029528(0);
        func_8004036C(D_8009B180);
        func_8004036C(D_8009B184);
        D_8009B184 = 0;
        D_8009B180 = 0;
        switch (D_8009AF2E) {
        case 0:
            break;
        case 1:
            func_80029164(0, 1);
            File_WaitForTransfers();
            obj = (DisplayObject *)func_800291E0(0, -1, -1);
            *(s16 *)&obj->field_30.h.field_30 = 0x5A;
            *(s16 *)&obj->field_30.h.field_32 = 0x16;
            func_80012D84(4);
            func_8001944C(obj);
            break;
        case 2:
            D_8009B180 = func_80017F04(D_801A7B80, 0x86, 0x52);
            break;
        case 3:
            D_8009B180 = func_80017F04(D_801A7B80, 0x5C, 0x52);
            D_8009B184 = func_80017F04(&D_801A7B80[1], 0xB0, 0x52);
            break;
        }
    } else if (gInput_wPad1Pressed & PAD_BUTTON_SELECT) {
        D_8009B23A |= 0x4000;
        D_8009AF2E++;
        if (D_8009AF2E >= 4) {
            D_8009AF2E = 0;
        }
    } else if (gInput_wPad1Pressed & (PAD_DIRECTION_LEFT | PAD_DIRECTION_RIGHT)) {
        D_8009AF2A ^= 1;
    } else if (gInput_wPad1Repeat & (PAD_DIRECTION_UP | PAD_DIRECTION_DOWN)) {
        D_8009AF2C[D_8009AF2A]++;
        if (gInput_wPad1Repeat & PAD_DIRECTION_DOWN) {
            D_8009AF2C[D_8009AF2A] -= 2;
        }
    } else if (gInput_wPad1Pressed & PAD_BUTTON_CROSS) {
        p = func_8002C604(D_8009AF2C[0]);
        D_8009B16C[2] = (D_8009B16C[2] + 1) & 7;
        *(s16 *)(p + 0x1A) = D_8009AF2D;
        switch (D_8009AF2E) {
        case 0:
            *(s16 *)(p + 0) = D_800908A0[0xC];
            *(s16 *)(p + 2) = -0x18;
            *(s16 *)(p + 4) = D_800908A0[0xD];
            break;
        case 1:
            *(s16 *)(p + 0) = 0xA0;
            *(s16 *)(p + 2) = 0x78;
            break;
        case 2:
            *(s16 *)(p + 0) = 0xA0;
            *(s16 *)(p + 2) = 0x70;
            break;
        case 3:
            *(s16 *)(p + 0) = 0xA0;
            *(s16 *)(p + 2) = 0x70;
            break;
        }
    }
}

/* Prints the "EFFECT = %2d %2d" debug line, then one of two divider strings
   depending on D_8009AF2A. */
void func_80022618(void) {
    u8 v0;

    func_800222F4();
    FntPrint(D_80010074, D_8009AF2C[0], D_8009AF2D);
    v0 = D_8009AF2A;
    if (v0 != 0) {
        FntPrint(D_80010090);
    } else {
        FntPrint(D_800100A8);
    }
}
