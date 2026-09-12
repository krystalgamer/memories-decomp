#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GSD_BOUTPUTTYPE_IN_DATA
#define D_8009AF5C_AS_BYTE_ARRAY
#include "../types.h"
#include "../ygo_types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "duel_effect.h"
#include "fade.h"
#include "input.h"
#include "options.h"
#include "options_layout_position_data.h"
#include "options_screen.h"
#include "sound.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_constants.h"
#include "../unmatched.h"

/* The options screen: its text-colour and text-box setup, the layout pass
   that places its two cursor objects, the init that creates them, and the
   input handler and per-frame dispatcher that drive it. All five work on the
   gOptions_* state options.h describes, and each is reached from the one
   after it - Options_Init calls func_8003C4E0 and Options_UpdateLayout,
   Options_HandleInput calls Options_UpdateLayout on every output-type change,
   and Options_Update dispatches to Options_HandleInput.

   The four former sources were recorded at four profiles. Every member
   compiles to an identical object at gcc_2_8_1_g8_split, including
   Options_Init, whose gcc_2_8_1_cc_g8_as_g4_no_split dated from when it
   reached gSD_bOutputType through an oversized array; it now takes the
   .data arm of that declaration, which leaves the assembler threshold with
   nothing to decide. */

void func_8003C4E0(s32 arg0) {
    u8 *t = gText_abColorSlots;
    t[0]=4; t[1]=4; t[2]=4; t[3]=4; t[4]=4;
    t[arg0]=0;
    if (arg0 != 0) t[3]=2; else t[4]=2;
    TextBox_Create(1,0xEF,0x18,0x38,0x120,0x100);
    func_80039A14(&D_800EB15C);
}

void Options_UpdateLayout(s32 selection) {
    u8 sp0[12];
    DisplayObject *a;
    DisplayObject *b;
    s32 k;
    u32 v;

    *(OptionsLayoutPositionBlock *)sp0 =
        *(OptionsLayoutPositionBlock *)D_8009AF5C;
    *(s16 *)(sp0 + 8) = 0x68;
    k = gOptions_bOutputType;
    a = D_8009B380;
    b = D_8009B388;
    *(s16 *)(sp0 + 0xA) = 0xC8;
    v = *(u16 *)(sp0 + 8 - -(k * 2));
    a->field_30.h.field_32 = 0x48;
    a->field_30.h.field_30 = v;
    b->field_30.h.field_30 = 0x20;
    b->field_30.h.field_32 = *(u16 *)(sp0 - -(selection * 2)) + 8;
    if (selection == 0) {
        a->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        b->field_30.h.field_30 = a->field_30.h.field_30 + 8;
        b->field_30.h.field_32 = a->field_30.h.field_32 + 8;
    } else {
        a->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
    }
}

/* Creates 3 objects via func_800400AC(func_8004002C(),
   kind) and configures each: obj1 gets an 8-arg func_800428A8 setup
   plus a func_800428EC(obj1, -5), then sets the options state and output type
   from gSD_bOutputType (clearing the output type back to 0 when its sign bit
   is set) and calls func_8003C4E0(0); obj2 gets an
   8-arg func_800404CC setup and is stashed in D_8009B388;
   obj3 gets a 10-arg func_80040510 setup. Finally Options_UpdateLayout is called
   with the selection flag (set to 0 earlier), D_8009B380 is set to obj3,
   and SD_BGMPlay(0x7350) runs last.

   gSD_bOutputType is reached through its .data arm (sound.h) so the load
   is absolute (lui+lbu) rather than gp-relative. The single load of it is
   cached in a local and
   reused for both the output-type store and the `< 0` sign test, matching
   how gcc schedules the real target (interleaved with obj1->flags's
   load-early/store-late around the intervening `ori`). */

void Options_Init(void) {
    DisplayObject *obj;
    s32 s0;
    s32 s2;

    obj = func_800400AC(func_8004002C(), 2);
    s2 = 0x10;
    func_800428A8(obj, 0, 0, 0, 0, 0, s2, 0x100, D_801AF000);
    func_800428EC((u8 *)obj, -5);
    gOptions_bState = 1;
    {
        s8 flag408 = gSD_bOutputType;
        obj->flags |= 0x28;
        gOptions_bOutputType = flag408;
        if (flag408 < 0) {
            gOptions_bOutputType = 0;
        }
    }
    gOptions_bSelection = 0;
    func_8003C4E0(0);

    obj = func_800400AC(func_8004002C(), 2);
    s0 = 0xB;
    func_800404CC(obj, 0x18, 0x48, 3, 4, 0, s0, 0x20C);
    D_8009B388 = obj;
    obj->flags |= 0x28;

    obj = func_800400AC(func_8004002C(), 1);
    func_80040510((DisplayObjectConfigView *)obj, 0x68, 0x48, 0x10, s2, 0x50, 0x80, s0, 0x210, 0xFC);

    {
        s32 flag = gOptions_bSelection;
        D_8009B380 = obj;
        Options_UpdateLayout(flag);
    }
    SD_BGMPlay(0x7350);
}

/* Retail performs a fresh absolute load for each input-state test. */

void Options_HandleInput(void)
{
    s32 value;

    if (gOptions_bSelection == 0 &&
        (gInput_wPad1Pressed & PAD_DIRECTION_HORIZONTAL_MASK)) {
        if (gInput_wPad1Pressed & PAD_DIRECTION_RIGHT) {
            if (gOptions_bOutputType != 0) {
                return;
            }
            value = 1;
            gSD_bOutputType = value;
            gOptions_bOutputType = 1;
        } else {
            if (gOptions_bOutputType == 0) {
                return;
            }
            value = 0;
            gSD_bOutputType = value;
            gOptions_bOutputType = 0;
        }
        SD_SetOutputType(value);
        SD_SEPlayFull(0x2F);
        Options_UpdateLayout(gOptions_bSelection);
        return;
    }

    if (gOptions_bSelection != 0 &&
        (gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK)) {
        SD_SEPlayFull(7);
        gOptions_bState = *(u8 *)&gOptions_bSelection + 1;
        return;
    }

    if (gInput_wPad1Pressed & PAD_BUTTON_CANCEL) {
        gOptions_bState = 0;
        SD_SEPlayFull(8);
    }
}

s32 Options_Update(void)
{
    switch (gOptions_bState & 0xF) {
    case 0: Fade_WaitOut(); break;
    case 1: Options_HandleInput(); break;
    case 2: break;
    case 3: gOptions_bState = 1; break;
    }
    return gOptions_bState;
}
