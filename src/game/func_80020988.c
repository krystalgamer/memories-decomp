#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "input.h"

extern u8 D_8009B152;
extern DisplayObject *D_8009B188;
extern DisplayObject *D_8009B18C;
extern DisplayObject *D_8009B1CC;

s32 func_80020988(void) {
    u8 *q;
    /* Direct flags members let GCC coalesce accesses and shorten retail code. */
    s32 one;
    s32 b;
    s32 c;
    s32 v;
    u8 w;

    if ((D_8009B152 & 0x80) == 0) {
        D_8009B152 = D_8009B152 | 0x80;
        one = 1;
        q = func_800400AC(func_8004002C(), 2);
        b = 0xB;
        c = 0x20C;
        func_800404CC(
            q,
            *(s16 *)&D_8009B1CC->field_30.h.field_30 - 8,
            *(s16 *)&D_8009B1CC->field_30.h.field_32 + 0x1E,
            3, one, 2, b, c
        );
        func_80042918(q);
        func_800428EC(q, 0xA);
        *(u16 *)(q + 8) = *(u16 *)(q + 8) | 0x28;
        D_8009B188 = (DisplayObject *)q;

        q = func_800400AC(func_8004002C(), 2);
        func_800404CC(
            q,
            *(s16 *)&D_8009B1CC->field_30.h.field_30 + 0x3C,
            *(s16 *)&D_8009B1CC->field_30.h.field_32 + 0x1E,
            3, one, 0, b, c
        );
        func_80042918(q);
        func_800428EC(q, 0xA);
        *(u16 *)(q + 8) = *(u16 *)(q + 8) | 0x28;
        D_8009B18C = (DisplayObject *)q;
        return 0;
    }

    q = (u8 *)D_8009B1CC;

    if ((D_8009B152 & 0x40) != 0) {
        *(u16 *)(q + 8) =
            *(u16 *)(q + 8) | DISPLAY_OBJECT_FLAG_CLIP_TEST;
        v = q[0x21] + q[0x60];
        q[0x21] = v;
        w = v;
        if ((v & 0x7F) != 0) {
            return 0;
        }
        if ((w & 0xFF) == 0) {
            *(u16 *)(q + 8) =
                *(u16 *)(q + 8) & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
        do { D_8009B152 = D_8009B152 & 0xBF; } while (0);
        return 0;
    }

    if ((gInput_wPad1Repeat & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
        *(s16 *)(q + 0x60) = 0x10;
        if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
            *(s16 *)(q + 0x60) = -0x10;
        }
        D_8009B152 = D_8009B152 | 0x40;
        return 0;
    }

    if ((gInput_wPad1Pressed &
         (PAD_BUTTON_CONFIRM_MASK | PAD_BUTTON_CANCEL)) != 0) {
        func_8004036C(D_8009B188);
        func_8004036C(D_8009B18C);
        if ((gInput_wPad1Pressed & PAD_BUTTON_CANCEL) != 0) {
            return -1;
        }
        if (q[0x21] == 0) {
            return 1;
        }
        return 2;
    }
    return 0;
}
