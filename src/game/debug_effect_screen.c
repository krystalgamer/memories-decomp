#define GINPUT_PAD1_HELD_IN_DATA_VOLATILE
#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "func_800291E0.h"
#include "duel_effect_resource_setup.h"
#include "duel_effect_request.h"
#include "main_frame.h"
#include "display_object_core.h"
#include "duel_card.h"
#include "file_transfer.h"
#include "func_8001944C.h"
#include "input.h"
#include "view_state.h"
#include "display_object.h"
#include "duel_card_display_state.h"
#include "duel_scene_state.h"
#include "../unmatched.h"
#include "duel_screen_tables.h"
#include "duel_scene_state.h"
#define D_8009B16C_AS_BYTE_ARRAY
#include "duel_check_quit_input.h"
#define D_8009B30C_AS_SIGNED_DATA
#include "sorted_entry.h"
#include "debug_effect_screen.h"

/* The developer effect-preview screen: a pad-driven camera and viewport
   nudge, the controller that builds one of four preview pages and spawns an
   effect on CROSS, and the HUD line that prints the two tuned values. The
   three call each other in that order and are the whole gcc_2_8_1_g8_split
   run between func_80021F80 and func_80022674. */

/* One packed small-data window: byte 0 selects one of the two coordinate
   bytes at 2..3, byte 4 selects the preview page, and bytes 1 and 5 are
   unused. Keeping it as one object preserves the retail interior gap. */
u8 gDebugEffect_abPreviewState[6] __attribute__((section(".sdata"))) = {0};
#define gDebugEffect_bCoordinateAxis gDebugEffect_abPreviewState[0]
#define gDebugEffect_abCoordinates (&gDebugEffect_abPreviewState[2])
#define gDebugEffect_bPage gDebugEffect_abPreviewState[4]
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
   first call it initialises the gDuel_wSceneStateFlags mode flags and the cursor state.
   With the rebuild flag set it releases the two spawned objects and
   recreates the display for the current page (a message box, one or two
   card objects from D_801A7B80). Otherwise SELECT cycles the page (0..3),
   left/right toggles which coordinate the up/down repeat adjusts, and CROSS
   places a new object through DuelEffect_AllocateRequest at a page-dependent position. */
void func_800222F4(void) {
    DisplayObject *obj;
    DuelEffectRequest *p;

    if (gInput_wPad1Held & PAD_BUTTON_START) {
        func_800220B8();
    }
    if ((gDuel_wSceneStateFlags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags |= 0xC000;
        D_8009B16C[2] = 0;
        gDebugEffect_bPage = 0;
        gDebugEffect_bCoordinateAxis = 0;
        gDebugEffect_abCoordinates[1] = 0;
        D_8009B184 = 0;
        D_8009B180 = 0;
    }
    if (gDuel_wSceneStateFlags & 0x4000) {
        gDuel_wSceneStateFlags &= 0xBFFF;
        func_80029528(0);
        func_8004036C(D_8009B180);
        func_8004036C(D_8009B184);
        D_8009B184 = 0;
        D_8009B180 = 0;
        switch (gDebugEffect_bPage) {
        case 0:
            break;
        case 1:
            func_80029164(0, 1);
            File_WaitForTransfers();
            obj = (DisplayObject *)func_800291E0(0, -1, -1);
            obj->field_30.h.field_30 = 0x5A;
            obj->field_30.h.field_32 = 0x16;
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
        gDuel_wSceneStateFlags |= 0x4000;
        gDebugEffect_bPage++;
        if (gDebugEffect_bPage >= 4) {
            gDebugEffect_bPage = 0;
        }
    } else if (gInput_wPad1Pressed & (PAD_DIRECTION_LEFT | PAD_DIRECTION_RIGHT)) {
        gDebugEffect_bCoordinateAxis ^= 1;
    } else if (gInput_wPad1Repeat & (PAD_DIRECTION_UP | PAD_DIRECTION_DOWN)) {
        gDebugEffect_abCoordinates[gDebugEffect_bCoordinateAxis]++;
        if (gInput_wPad1Repeat & PAD_DIRECTION_DOWN) {
            gDebugEffect_abCoordinates[gDebugEffect_bCoordinateAxis] -= 2;
        }
    } else if (gInput_wPad1Pressed & PAD_BUTTON_CROSS) {
        p = (DuelEffectRequest *)DuelEffect_AllocateRequest(
            gDebugEffect_abCoordinates[0]);
        D_8009B16C[2] = (D_8009B16C[2] + 1) & 7;
        p->field_1A = gDebugEffect_abCoordinates[1];
        switch (gDebugEffect_bPage) {
        case 0:
            p->field_00 = D_800908A0[0xC];
            p->field_02 = -0x18;
            p->field_04 = D_800908A0[0xD];
            break;
        case 1:
            p->field_00 = 0xA0;
            p->field_02 = 0x78;
            break;
        case 2:
            p->field_00 = 0xA0;
            p->field_02 = 0x70;
            break;
        case 3:
            p->field_00 = 0xA0;
            p->field_02 = 0x70;
            break;
        }
    }
}

/* Prints the "EFFECT = %2d %2d" debug line, then one of two divider strings
   depending on the selected coordinate axis. */
void DuelScene_UpdateEffectPreview(void) {
    u8 v0;

    func_800222F4();
    FntPrint(
        D_80010074,
        gDebugEffect_abCoordinates[0],
        gDebugEffect_abCoordinates[1]);
    v0 = gDebugEffect_bCoordinateAxis;
    if (v0 != 0) {
        FntPrint(D_80010090);
    } else {
        FntPrint(D_800100A8);
    }
}
