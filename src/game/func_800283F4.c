#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "input.h"
#include "fade.h"

#include "display_object_layout.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "display_object_api.h"
#include "display_object_helpers.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "file_transfer.h"
#include "text_box_runtime.h"

extern u8 *D_8009B240;
extern u8 D_8009B248;
extern u8 D_8009B24B;
extern u8 *D_8009B24C;
extern DuelEffectChannel *D_8009B250;
extern u8 D_8009B254;
extern u16 gDuel_wViewerCardID;
extern u8 D_8009AF76 __attribute__((section(".data")));
extern u8 D_8009B0C0 __attribute__((section(".data")));
extern u32 D_8009B134 __attribute__((section(".data")));
extern u8 D_8009B140 __attribute__((section(".data")));
extern u8 D_8009B26C __attribute__((section(".data")));
extern s16 gDuel_wSelectedCardID __attribute__((section(".data")));
extern volatile u16 gInput_wPad2Pressed __attribute__((section(".data")));
extern u8 D_800EA0E8[];
extern s32 gDuel_adwCardStats[];

extern s32 func_800282E8(void);
extern void func_80029164(s32, s32);
extern void func_80029528(s32);
extern void func_80029574(s32);
extern u8 *func_800291E0(s32, s32, s32);

void func_800283F4(void)
{
    /* Pinned: unpinned, gcc puts the slide value in $s1 and the object in
       $s0, the opposite of retail, which costs 56 positions. */
    register s32 slide __asm__("$16");
    register s32 channel __asm__("$4");
    u8 *obj;
    u8 *bg;
    u8 *p;
    u8 *next_obj;
    s32 *stats;
    DuelEffectChannel *box;
    DuelEffectChannel *chan;
    DuelEffectChannel *pos_box;
    DuelEffectChannel *dead_box;
    s32 adj;
    s32 i;
    s32 kind;
    s32 masked;
    s32 speed;
    s32 test;
    u16 flags;
    u16 id;
    u16 objflags;
    u8 closing;
    u8 state;

    if (func_800282E8() == 0) {
        channel = 3;
        slide = -0x400;
        flags = channel;
        D_8009B0C0 = flags - 2;
        func_80029574(flags);
        p = D_800EA0E8;
        *(s16 *)(p + 0xEA) = 0x100;
        *(s16 *)(p + 0xE8) = 0;
        *(s16 *)(p + 0xEC) = 0;
        *(s16 *)(p + 0xEE) = 0xFF;
        func_80029164(3, (s16)gDuel_wViewerCardID);
        obj = func_800291E0(3, -1, -1);
        adj = D_8009B24B;
        *(s16 *)(obj + 0x30) = -0x8C;
        obj[0x21] = 0x80;
        *(u16 *)(obj + 0x32) += adj;
        *(u16 *)(obj + 8) |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        func_80043178(obj);
        *(s16 *)(obj + 0x60) = slide;
        func_80042918(obj);
        func_800428EC(obj, 0x14);
        D_8009B24C = obj;
        obj = func_800400AC(func_8004002C(), 2);
        func_800404CC(obj, 0x148, D_8009B24B + 0xE, 0, 2, 0, 0xD, 0x107);
        *(s16 *)(obj + 0x60) = slide;
        *(u16 *)(obj + 8) |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_80042918(obj);
        func_800428EC(obj, 0x14);
        func_80043178(obj);
        *(s16 *)(obj + 0x60) = slide;
        D_8009B240 = obj;
        D_8009B250 = 0;
        i = 0;
        stats = gDuel_adwCardStats;
        chan = D_800EB0F8;
        for (; i < 3; i++, chan++) {
            if ((chan->flags_34 & 0x8000) == 0) {
                id = gDuel_wViewerCardID;
                gDuel_wSelectedCardID = id;
                kind = 3;
                if (((stats[(s16)id - 1] >> 26) & 0x1F) >= 0x14) {
                    kind = 4;
                }
                box = TextBox_Create(i, kind, 0x148, 0xE, 0xA8, 0xC0);
                box->field_53 = 1;
                box->field_54 = 0;
                box->field_59 = 0x15;
                D_8009B250 = box;
                func_80039A14(box);
                break;
            }
        }
        func_80015BD8(0x40, 2);
        D_8009B140 = D_8009AF76 - 0x13;
        SD_SEPlayFull(0x34);
        D_8009B248 |= 0x40;
        return;
    }

    state = D_8009B248;
    if (state & 0x40) {
        obj = D_8009B240;
        speed = *(s16 *)(obj + 0x60);
        if (speed != 0) {
            if (state & 0x10) {
                func_80043230(obj, 0x148, *(s16 *)(obj + 0x32), speed);
                flags = *(u16 *)(obj + 0x60) - 0x55;
                *(s16 *)(obj + 0x60) = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)(obj + 0x30) = 0x400;
                    *(s16 *)(obj + 0x60) = 0;
                }
            } else {
                func_80043230(obj, 0x94, *(s16 *)(obj + 0x32), speed);
                flags = *(u16 *)(obj + 0x60) + 0x55;
                *(s16 *)(obj + 0x60) = flags;
                if ((s16)flags >= 0) {
                    *(s16 *)(obj + 0x30) = 0x94;
                    *(s16 *)(obj + 0x60) = 0;
                }
            }
            pos_box = D_8009B250;
            if (pos_box != 0) {
                TextBox_SetPos(pos_box, *(s16 *)(obj + 0x30),
                               *(s16 *)(obj + 0x32));
            }
        }
        obj = D_8009B24C;
        speed = *(s16 *)(obj + 0x60);
        if (speed != 0) {
            if (D_8009B248 & 0x10) {
                func_80043230(obj, -0x8C, *(s16 *)(obj + 0x32), speed);
                flags = *(u16 *)(obj + 0x60) - 0x55;
                *(s16 *)(obj + 0x60) = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)(obj + 0x30) = 0x400;
                    *(s16 *)(obj + 0x60) = 0;
                }
            } else {
                func_80043230(obj, 2, *(s16 *)(obj + 0x32), speed);
                flags = *(u16 *)(obj + 0x60) + 0x55;
                *(s16 *)(obj + 0x60) = flags;
                if ((s16)flags >= 0) {
                    *(s16 *)(obj + 0x30) = 2;
                    *(s16 *)(obj + 0x60) = 0;
                }
            }
        }
        if (((D_8009B0F4_abs & 0x02000030) | D_8009B134) != 0) {
            return;
        }
        bg = D_8009B240;
        if (*(s16 *)(bg + 0x60) != 0) {
            return;
        }
        if (*(s16 *)(D_8009B24C + 0x60) != 0) {
            return;
        }
        closing = D_8009B248;
        masked = closing & 0xBF;
        test = closing & 0x10;
        D_8009B248 = masked;
        if (test == 0) {
            return;
        }
        func_8004036C(bg);
        func_80029528(3);
        dead_box = D_8009B250;
        if (dead_box != 0) {
            TextBox_Destroy(dead_box);
        }
        D_8009B0C0 = 0;
        D_8009B254 |= 0x40;
        return;
    }

    if ((state & 0x20) == 0) {
        obj = D_8009B24C;
        obj[0x21] += 0xC;
        if (obj[0x21] < 0x40) {
            objflags = *(u16 *)(obj + 8);
            obj[0x21] = 0;
            state = D_8009B248 | 0x20;
            *(u16 *)(obj + 8) = objflags & 0xFFFB;
            D_8009B248 = state;
        }
        return;
    }

    if ((D_8009B26C & 0x1F) == 0xE) {
        if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & 0x20) != 0) {
            goto press;
        }
        return;
    }
    if ((gInput_wPad1Pressed & 0x20) == 0) {
        return;
    }
press:
    slide = 0x400;
    func_80043178(D_8009B240);
    next_obj = D_8009B24C;
    *(s16 *)(D_8009B240 + 0x60) = slide;
    func_80043178(next_obj);
    *(s16 *)(D_8009B24C + 0x60) = slide;
    func_80015BD8(0xFF, 2);
    SD_SEPlayFull(0x34);
    D_8009B248 |= 0x50;
}
