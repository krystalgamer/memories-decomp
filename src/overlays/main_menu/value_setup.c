#include "../../types.h"
#include "../../ygo_types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../unmatched.h"
#define GINPUT_PAD1_REPEAT_SIZED_VOLATILE
#define GINPUT_PAD1_PRESSED_SIZED_VOLATILE
#include "../../game/input.h"
#include "../../game/card_constants.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_helpers.h"
#include "../../game/display_object_layout.h"
#include "../../game/main_services.h"
#include "../../game/sound.h"
#include "entrypoints.h"
#include "ordering_tables.h"

/* The value-setup screen: the three lifecycle entry points func_8002DC38.c
   calls - start, update per tick, finish - and the four helpers that only
   they and each other reach.

   value_setup.h, now folded in below, listed three symbols as deliberately
   absent because their
   declarers disagreed. One translation unit forces the choice, and these are
   the results, each verified byte-identical rather than argued:

     D_801845B0  was u8 *[], void *[] and ValueWidgetView *[]. The typed
                 spelling won because the drawing code dereferences it
                 twenty-odd times through named fields.
     D_801845A0  was void * and u8 *, and u8 * won on the byte arithmetic
                 the starter did on it.
     D_801845A4  same pair, same reason.

   All of them, and D_801845B8, are display objects out of func_800400AC,
   and every offset any of them was reached at is a DisplayObject member:
   flags at 0x08, update at 0x24, the position pair at 0x30, the saved pair
   at 0x36/0x38, the timer at 0x60 and the two widget bytes at 0x6B/0x6C.
   ValueWidgetView named four of those, so it is retired and the byte
   arithmetic with it; all five handles and the tween's own pointer are
   DisplayObject *, which builds byte-identically.

   D_801845BC[2] and D_801845BE remain two names for one byte, as
   that header recorded. That overlap is untouched here: the two are
   distinct symbols at distinct addresses as far as C is concerned, so one
   unit does not force it and naming it is #2602's business. */

/* Value-setup screen state. PR #3069 gave these a shared header when four
   sources drove them and each carried its own copy; with those sources now
   one unit the sharing has no readers left, so the declarations and
   everything #3069 established about them live here instead. Nothing it
   found is dropped.

     D_801845BC  At least three bytes. [0] is the live value, [1] its start,
                 and [2] the toggle result the screen computes.
     D_801845D8  Points at the toggle byte the caller handed in; the finish
                 step writes through it.
     D_801845C0  The two values being edited, each a ValueSetupEntry: the
                 value, the shown value tweened toward it, and the pointer
                 the finish step writes it back through. It was a u16 array
                 indexed [0]/[1] and [6]/[7], with the two pointers reached
                 by byte offset; the entry spelling builds byte-identically.

   D_801845BC[2] and D_801845BE are the same storage, since D_801845BC is at
   0x801845BC: the drawing code writes it through the array and the finisher
   reads it by its own name. Left exactly as it is. They are distinct symbols
   at distinct addresses as far as C is concerned, so one unit does not force
   the question, and naming it is what #2602 exists for. */
extern u8 D_801845BC[];
extern u8 *D_801845D8;
extern ValueSetupEntry D_801845C0[2];

extern DisplayObject *D_801845A0;
extern DisplayObject *D_801845A4;
extern DisplayObject *D_801845B0[];
extern DisplayObject *D_801845B8;
extern u8 D_801845BE;
void MainMenu_StartValueWidgetTween(s32 index, s32 value);
void MainMenu_DrawValueSetup(void);
s32 MainMenu_CountDecimalDigits(s32 value);

void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle)
{
    DisplayObject *object;

    object = func_800400AC(func_8004002C(), 2);
    D_801845A0 = object;
    if (object != 0) {
        func_800404CC(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        D_801845A0->flags |= 0x28;
        func_800428EC((u8 *)D_801845A0, -2);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845A4 = object;
    if (object != 0) {
        func_800428A8(object, 0, 0xA, 6, 0, 0, 0xE, 5, D_801AF800);
        D_801845A4->flags |= 0x28;
        func_800428EC((u8 *)D_801845A4, -1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[0] = object;
    if (object != 0) {
        func_800404CC(D_801845B0[0], 0, 0, 3, 4, 0, 0xB, 0x20C);
        D_801845B0[0]->flags |= 0x28;
        func_800428EC((u8 *)D_801845B0[0], 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[1] = object;
    if (object != 0) {
        func_800404CC(D_801845B0[1], 0, 0, 3, 4, 0, 0xB, 0x20C);
        D_801845B0[1]->flags |= 0x28;
        func_800428EC((u8 *)D_801845B0[1], 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[2] = object;
    if (object != 0) {
        func_800404CC(D_801845B0[2], 0, 0, 3, 4, 0, 0xB, 0x20C);
        D_801845B0[2]->flags |= 0x28;
        func_800428EC((u8 *)D_801845B0[2], 1);
    }

    D_801845D8 = toggle;
    D_801845BC[2] = (*toggle == 0);
    D_801845BC[0] = D_801845BC[1] = 2;
    D_801845C0[0].out = first;
    D_801845C0[1].out = second;
    D_801845C0[0].value = D_801845C0[0].shown = *first;
    D_801845C0[1].value = D_801845C0[1].shown = *second;
    D_800E9DB0[0] = MainMenu_DrawValueSetup;
}

s32 MainMenu_UpdateValueSetup(void)
{
    s32 busyA;
    s32 busyB;
    s32 step;
    u16 value;

    busyA = (D_801845B0[0]->update != 0);
    busyB = (D_801845B0[1]->update != 0);

    if (D_801845C0[0].value != D_801845C0[0].shown) {
        step = D_801845C0[0].value - D_801845C0[0].shown;
        if (step < 0) {
            step = D_801845C0[0].shown - D_801845C0[0].value;
        }
        if (D_801845C0[0].shown < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (D_801845C0[0].shown < D_801845C0[0].value) {
            D_801845C0[0].shown = D_801845C0[0].shown + step;
        } else {
            D_801845C0[0].shown = D_801845C0[0].shown - step;
        }
        busyA++;
    }

    if (D_801845C0[1].value != D_801845C0[1].shown) {
        step = D_801845C0[1].value - D_801845C0[1].shown;
        if (step < 0) {
            step = D_801845C0[1].shown - D_801845C0[1].value;
        }
        if (D_801845C0[1].shown < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (D_801845C0[1].shown < D_801845C0[1].value) {
            D_801845C0[1].shown = D_801845C0[1].shown + step;
        } else {
            D_801845C0[1].shown = D_801845C0[1].shown - step;
        }
        busyB++;
    }

    if (busyA == 0 && busyB == 0) {
        if ((gInput_wPad1Pressed[0] & PAD_BUTTON_CANCEL) || (gInput_wPad1Pressed[1] & PAD_BUTTON_CANCEL)) {
            SD_SEPlay(8, 0xFF, 0);
            return -1;
        }
        if ((gInput_wPad1Pressed[0] & PAD_BUTTON_START) || (gInput_wPad1Pressed[1] & PAD_BUTTON_START)) {
            SD_SEPlay(7, 0xFF, 0);
            return 1;
        }
    }

    if (busyA == 0) {
        if (D_801845BC[0] < 2) {
            if (gInput_wPad1Repeat[0] & PAD_DIRECTION_HORIZONTAL_MASK) {
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_wPad1Repeat[0] & PAD_DIRECTION_LEFT) {
                    D_801845BC[2] = 0;
                } else {
                    D_801845BC[2] = 1;
                }
                D_801845BC[0] = D_801845BC[2];
                if (D_801845BC[1] < 2) {
                    D_801845BC[1] = D_801845BC[2];
                }
            } else if (gInput_wPad1Repeat[0] & PAD_DIRECTION_DOWN) {
                MainMenu_StartValueWidgetTween(0, 2);
            }
        } else {
            if (gInput_wPad1Repeat[0] & PAD_DIRECTION_HORIZONTAL_MASK) {
                value = D_801845C0[0].value;
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_wPad1Repeat[0] & PAD_DIRECTION_LEFT) {
                    value = (value - DUEL_LIFE_POINT_SELECTION_STEP > 0)
                                ? (value - DUEL_LIFE_POINT_SELECTION_STEP)
                                : 1;
                } else if (value < 2) {
                    value = DUEL_LIFE_POINT_SELECTION_STEP;
                } else {
                    value = (
                        value + DUEL_LIFE_POINT_SELECTION_STEP <
                        DUEL_STARTING_LIFE_POINTS + 1
                    ) ? (value + DUEL_LIFE_POINT_SELECTION_STEP)
                      : DUEL_STARTING_LIFE_POINTS;
                }
                D_801845C0[0].value = value;
            } else if (gInput_wPad1Repeat[0] & PAD_DIRECTION_UP) {
                MainMenu_StartValueWidgetTween(0, D_801845BC[2]);
            }
        }
    }

    if (busyB == 0) {
        if (D_801845BC[1] < 2) {
            if (gInput_wPad1Repeat[1] & PAD_DIRECTION_HORIZONTAL_MASK) {
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_wPad1Repeat[1] & PAD_DIRECTION_LEFT) {
                    D_801845BC[2] = 0;
                } else {
                    D_801845BC[2] = 1;
                }
                D_801845BC[1] = D_801845BC[2];
                if (D_801845BC[0] < 2) {
                    D_801845BC[0] = D_801845BC[2];
                }
            } else if (gInput_wPad1Repeat[1] & PAD_DIRECTION_DOWN) {
                MainMenu_StartValueWidgetTween(1, 2);
            }
        } else {
            if (gInput_wPad1Repeat[1] & PAD_DIRECTION_HORIZONTAL_MASK) {
                value = D_801845C0[1].value;
                SD_SEPlay(6, 0xFF, 0);
                if (gInput_wPad1Repeat[1] & PAD_DIRECTION_LEFT) {
                    value = (value - DUEL_LIFE_POINT_SELECTION_STEP > 0)
                                ? (value - DUEL_LIFE_POINT_SELECTION_STEP)
                                : 1;
                } else if (value < 2) {
                    value = DUEL_LIFE_POINT_SELECTION_STEP;
                } else {
                    value = (
                        value + DUEL_LIFE_POINT_SELECTION_STEP <
                        DUEL_STARTING_LIFE_POINTS + 1
                    ) ? (value + DUEL_LIFE_POINT_SELECTION_STEP)
                      : DUEL_STARTING_LIFE_POINTS;
                }
                D_801845C0[1].value = value;
            } else if (gInput_wPad1Repeat[1] & PAD_DIRECTION_UP) {
                MainMenu_StartValueWidgetTween(1, D_801845BC[2]);
            }
        }
    }

    return 0;
}

void MainMenu_DrawValueSetup(void)
{
    POLY_GT4 digit;
    POLY_G4 bar;
    DisplayObject *w;
    DisplayObject *mk;
    s32 first;
    s32 second;
    s32 x;
    s32 i;
    s32 count;
    s32 width;
    s32 c106;
    s32 c114;
    s32 c112;
    s32 c120;
    s32 d134;
    s32 d142;
    s32 d112;
    s32 d120;

    first = D_801845C0[0].shown;
    second = D_801845C0[1].shown;
    mk = D_801845B0[2];
    if (D_801845BC[2] == 0) {
        x = 116;
    } else {
        x = 220;
    }
    mk->field_30.h.field_30 = x;
    D_801845B0[2]->field_30.h.field_32 = 74;

    w = D_801845B0[0];
    if (w->update != 0 || D_801845BC[0] == 2) {
        w->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        w = D_801845B0[0];
        if (w->update == 0) {
            w->field_30.h.field_30 = first * 128 / DUEL_STARTING_LIFE_POINTS + 176;
            D_801845B0[0]->field_30.h.field_32 = 111;
        }
    } else {
        w->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        D_801845B0[0]->field_30.h.field_30 = D_801845B0[2]->field_30.h.field_30;
        D_801845B0[0]->field_30.h.field_32 = D_801845B0[2]->field_30.h.field_32;
    }

    w = D_801845B0[1];
    if (w->update != 0 || D_801845BC[1] == 2) {
        w->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        w = D_801845B0[1];
        if (w->update == 0) {
            w->field_30.h.field_30 = second * 128 / DUEL_STARTING_LIFE_POINTS + 176;
            D_801845B0[1]->field_30.h.field_32 = 139;
        }
    } else {
        w->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        D_801845B0[1]->field_30.h.field_30 = D_801845B0[2]->field_30.h.field_30;
        D_801845B0[1]->field_30.h.field_32 = D_801845B0[2]->field_30.h.field_32;
    }

    setPolyG4(&bar);
    bar.r0 = 64;
    bar.g0 = 32;
    bar.b0 = 32;
    bar.r1 = 255;
    bar.g1 = 32;
    bar.b1 = 32;
    bar.r2 = 64;
    bar.g2 = 32;
    bar.b2 = 32;
    bar.r3 = 255;
    bar.g3 = 32;
    bar.b3 = 32;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 107;
    bar.y1 = 107;
    bar.y2 = 115;
    bar.y3 = 115;
    bar.x1 = first * 128 / DUEL_STARTING_LIFE_POINTS + 176;
    bar.x3 = bar.x1;
    GsSortPoly(&bar, D_800E9D90[2], 2048);

    bar.r0 = 32;
    bar.g0 = 32;
    bar.b0 = 64;
    bar.r1 = 32;
    bar.g1 = 32;
    bar.b1 = 255;
    bar.r2 = 32;
    bar.g2 = 32;
    bar.b2 = 64;
    bar.r3 = 32;
    bar.g3 = 32;
    bar.b3 = 255;
    bar.x0 = 176;
    bar.x2 = 176;
    bar.y0 = 135;
    bar.y1 = 135;
    bar.y2 = 143;
    bar.y3 = 143;
    bar.x1 = second * 128 / DUEL_STARTING_LIFE_POINTS + 176;
    bar.x3 = bar.x1;
    GsSortPoly(&bar, D_800E9D90[2], 2048);

    setPolyGT4(&digit);
    digit.tpage = 11;
    digit.clut = 16041;
    digit.r0 = 255;
    digit.g0 = 255;
    digit.b0 = 255;
    digit.r1 = 255;
    digit.g1 = 255;
    digit.b1 = 255;
    digit.r2 = 255;
    digit.g2 = 255;
    digit.b2 = 255;
    digit.r3 = 255;
    digit.g3 = 255;
    digit.b3 = 255;

    width = MainMenu_CountDecimalDigits(DUEL_STARTING_LIFE_POINTS);
    count = MainMenu_CountDecimalDigits(first);
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 + 126 - i * 8;
        c106 = 106;
        c114 = 114;
        c112 = 112;
        c120 = 120;
        digit.y0 = c106;
        digit.x1 = digit.x0 + 8;
        digit.y1 = c106;
        digit.x2 = digit.x0;
        digit.y2 = c114;
        digit.x3 = digit.x1;
        digit.y3 = c114;
        digit.v0 = c112;
        digit.v1 = c112;
        digit.v2 = c120;
        digit.v3 = c120;
        digit.u0 = (first % 10) * 8 - 128;
        digit.u1 = (first % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        GsSortPoly(&digit, D_800E9D90[2], 2048);
        first = first / 10;
    }

    count = MainMenu_CountDecimalDigits(second);
    for (i = 0; i < count; i++) {
        digit.x0 = width * 8 + 126 - i * 8;
        d134 = 134;
        d142 = 142;
        d112 = 112;
        d120 = 120;
        digit.y0 = d134;
        digit.x1 = digit.x0 + 8;
        digit.y1 = d134;
        digit.x2 = digit.x0;
        digit.y2 = d142;
        digit.x3 = digit.x1;
        digit.y3 = d142;
        digit.v0 = d112;
        digit.v1 = d112;
        digit.v2 = d120;
        digit.v3 = d120;
        digit.u0 = (second % 10) * 8 - 128;
        digit.u1 = (second % 10) * 8 - 120;
        digit.u2 = digit.u0;
        digit.u3 = digit.u1;
        GsSortPoly(&digit, D_800E9D90[2], 2048);
        second = second / 10;
    }
}

void MainMenu_UpdateValueWidgetTween(u8 *obj)
{
    DisplayObject *o = (DisplayObject *)obj;
    DisplayObject *widget;
    s32 targetX;
    s32 targetY;
    s32 valueA;
    s32 valueB;
    s16 remaining;

    widget = D_801845B8;
    targetX = (s16)widget->field_30.h.field_30;
    targetY = (s16)widget->field_30.h.field_32;
    valueA = D_801845C0[0].shown;
    valueB = D_801845C0[1].shown;
    if (o->field_6C == 2) {
        if (o->field_6B == 0) {
            targetX = (valueA * 128) / DUEL_STARTING_LIFE_POINTS + 176;
            targetY = 111;
        } else {
            targetX = (valueB * 128) / DUEL_STARTING_LIFE_POINTS + 176;
            targetY = 139;
        }
    }
    remaining = o->field_60 - 1;
    o->field_60 = remaining;
    if (remaining <= 0) {
        D_801845BC[o->field_6B] = o->field_6C;
        o->update = 0;
    }
    o->field_30.h.field_30 =
        (o->field_34.h.field_36 * o->field_60) / 10 +
        (targetX * (10 - o->field_60)) / 10;
    o->field_30.h.field_32 =
        (o->field_38.h.field_38 * o->field_60) / 10 +
        (targetY * (10 - o->field_60)) / 10;
}

void MainMenu_FinishValueSetup(void)
{
    *D_801845C0[0].out = D_801845C0[0].value;
    *D_801845C0[1].out = D_801845C0[1].value;
    *D_801845D8 = (D_801845BE != 1);
    func_8004036C(D_801845A0);
    D_801845A0 = 0;
    func_8004036C(D_801845A4);
    D_801845A4 = 0;
    func_8004036C(D_801845B0[0]);
    D_801845B0[0] = 0;
    func_8004036C(D_801845B0[1]);
    D_801845B0[1] = 0;
    func_8004036C(D_801845B0[2]);
    D_801845B0[2] = 0;
    D_800E9DB0[0] = 0;
}

s32 MainMenu_CountDecimalDigits(s32 value)
{
    s32 digits = 0;
    s32 next;

    do {
        digits++;
        next = value / 10;
        value = next;
    } while (next != 0);

    return digits;
}

void MainMenu_StartValueWidgetTween(s32 index, s32 value)
{
    DisplayObject *object = D_801845B0[index];

    object->field_6B = index;
    object->field_60 = 0xA;
    object->field_6C = value;
    object->update = MainMenu_UpdateValueWidgetTween;
    object->field_34.h.field_36 = object->field_30.h.field_30;
    object->field_38.h.field_38 = object->field_30.h.field_32;
}
