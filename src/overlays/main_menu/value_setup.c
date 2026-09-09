#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../unmatched.h"
#include "../../game/input.h"
#include "../../game/card_constants.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_helpers.h"
#include "../../game/display_object_layout.h"
#include "../../game/main_services.h"
#include "entrypoints.h"

/* The value-setup screen: the three lifecycle entry points func_8002DC38.c
   calls - start, update per tick, finish - and the four helpers that only
   they and each other reach.

   value_setup.h, now folded in below, listed three symbols as deliberately
   absent because their
   declarers disagreed. One translation unit forces the choice, and these are
   the results, each verified byte-identical rather than argued:

     D_801845B0  was u8 *[], void *[] and ValueWidgetView *[]. The typed
                 spelling wins because the drawing code dereferences it
                 twenty-odd times through named fields, while the other three
                 users only do byte arithmetic on it, which a cast covers.
     D_801845A0  was void * and u8 *. u8 * wins: the starter does pointer
                 arithmetic on it, the finisher only passes and clears it.
     D_801845A4  same pair, same reason.

   D_801845BC[2] and D_801845BE remain two names for one byte, as
   that header recorded. That overlap is untouched here: the two are
   distinct symbols at distinct addresses as far as C is concerned, so one
   unit does not force it and naming it is #2602's business. */

typedef struct {
    u8 pad0[8];
    u16 flags;
    u8 pad0A[36 - 10];
    s32 updateCallbackAddress;
    u8 pad28[48 - 40];
    s16 x;
    s16 y;
} ValueWidgetView;

/* Value-setup screen state. PR #3069 gave these a shared header when four
   sources drove them and each carried its own copy; with those sources now
   one unit the sharing has no readers left, so the declarations and
   everything #3069 established about them live here instead. Nothing it
   found is dropped.

     D_801845BC  At least three bytes. [0] is the live value, [1] its start,
                 and [2] the toggle result the screen computes.
     D_801845D8  Points at the toggle byte the caller handed in; the finish
                 step writes through it.
     D_801845C0  The two tweened value pairs: [0]/[1] is the first value and
                 its target, [6]/[7] the second. u16 elements, measured
                 rather than preferred.

   D_801845BC[2] and D_801845BE are the same storage, since D_801845BC is at
   0x801845BC: the drawing code writes it through the array and the finisher
   reads it by its own name. Left exactly as it is. They are distinct symbols
   at distinct addresses as far as C is concerned, so one unit does not force
   the question, and naming it is what #2602 exists for. */
extern u8 D_801845BC[];
extern u8 *D_801845D8;
extern u16 D_801845C0[];

extern u8 *D_801845A0;
extern u8 *D_801845A4;
extern ValueWidgetView *D_801845B0[];
extern u8 *D_801845B8;
extern u8 D_801845BE;
extern GsOT *D_800E9D90[];
extern volatile u16 D_8009B394[];
extern volatile u16 D_8009B398[];
extern void func_80048658(s32, s32, s32);
void MainMenu_StartValueWidgetTween(s32 index, s32 value);
void MainMenu_DrawValueSetup(void);
s32 MainMenu_CountDecimalDigits(s32 value);

void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle)
{
    u8 *object;
    u8 *state;

    object = func_800400AC(func_8004002C(), 2);
    D_801845A0 = object;
    if (object != 0) {
        func_800404CC(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        *(u16 *)(D_801845A0 + 8) |= 0x28;
        func_800428EC(D_801845A0, -2);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845A4 = object;
    if (object != 0) {
        func_800428A8(object, 0, 0xA, 6, 0, 0, 0xE, 5, D_801AF800);
        *(u16 *)(D_801845A4 + 8) |= 0x28;
        func_800428EC(D_801845A4, -1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[0] = (ValueWidgetView *)object;
    if (object != 0) {
        func_800404CC((u8 *)D_801845B0[0], 0, 0, 3, 4, 0, 0xB, 0x20C);
        *(u16 *)((u8 *)D_801845B0[0] + 8) |= 0x28;
        func_800428EC((u8 *)D_801845B0[0], 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[1] = (ValueWidgetView *)object;
    if (object != 0) {
        func_800404CC((u8 *)D_801845B0[1], 0, 0, 3, 4, 0, 0xB, 0x20C);
        *(u16 *)((u8 *)D_801845B0[1] + 8) |= 0x28;
        func_800428EC((u8 *)D_801845B0[1], 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845B0[2] = (ValueWidgetView *)object;
    if (object != 0) {
        func_800404CC((u8 *)D_801845B0[2], 0, 0, 3, 4, 0, 0xB, 0x20C);
        *(u16 *)((u8 *)D_801845B0[2] + 8) |= 0x28;
        func_800428EC((u8 *)D_801845B0[2], 1);
    }

    state = (u8 *)D_801845C0;
    D_801845D8 = toggle;
    D_801845BC[2] = (*toggle == 0);
    D_801845BC[0] = D_801845BC[1] = 2;
    *(u16 **)(state + 4) = first;
    *(u16 **)(state + 0x10) = second;
    *(u16 *)state = *(u16 *)(state + 2) = *first;
    *(u16 *)(state + 0xC) = *(u16 *)(state + 0xE) = *second;
    D_800E9DB0[0] = MainMenu_DrawValueSetup;
}

s32 MainMenu_UpdateValueSetup(void)
{
    s32 busyA;
    s32 busyB;
    s32 step;
    u16 value;

    busyA = (*(void **)((u8 *)D_801845B0[0] + 0x24) != 0);
    busyB = (*(void **)((u8 *)D_801845B0[1] + 0x24) != 0);

    if (D_801845C0[0] != D_801845C0[1]) {
        step = D_801845C0[0] - D_801845C0[1];
        if (step < 0) {
            step = D_801845C0[1] - D_801845C0[0];
        }
        if (D_801845C0[1] < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (D_801845C0[1] < D_801845C0[0]) {
            D_801845C0[1] = D_801845C0[1] + step;
        } else {
            D_801845C0[1] = D_801845C0[1] - step;
        }
        busyA++;
    }

    if (D_801845C0[6] != D_801845C0[7]) {
        step = D_801845C0[6] - D_801845C0[7];
        if (step < 0) {
            step = D_801845C0[7] - D_801845C0[6];
        }
        if (D_801845C0[7] < 2) {
            step = 0x63;
        } else if (step >= 0x65) {
            step = 0x64;
        }
        if (D_801845C0[7] < D_801845C0[6]) {
            D_801845C0[7] = D_801845C0[7] + step;
        } else {
            D_801845C0[7] = D_801845C0[7] - step;
        }
        busyB++;
    }

    if (busyA == 0 && busyB == 0) {
        if ((D_8009B398[0] & PAD_BUTTON_CANCEL) || (D_8009B398[1] & PAD_BUTTON_CANCEL)) {
            func_80048658(8, 0xFF, 0);
            return -1;
        }
        if ((D_8009B398[0] & PAD_BUTTON_START) || (D_8009B398[1] & PAD_BUTTON_START)) {
            func_80048658(7, 0xFF, 0);
            return 1;
        }
    }

    if (busyA == 0) {
        if (D_801845BC[0] < 2) {
            if (D_8009B394[0] & PAD_DIRECTION_HORIZONTAL_MASK) {
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[0] & PAD_DIRECTION_LEFT) {
                    D_801845BC[2] = 0;
                } else {
                    D_801845BC[2] = 1;
                }
                D_801845BC[0] = D_801845BC[2];
                if (D_801845BC[1] < 2) {
                    D_801845BC[1] = D_801845BC[2];
                }
            } else if (D_8009B394[0] & PAD_DIRECTION_DOWN) {
                MainMenu_StartValueWidgetTween(0, 2);
            }
        } else {
            if (D_8009B394[0] & PAD_DIRECTION_HORIZONTAL_MASK) {
                value = D_801845C0[0];
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[0] & PAD_DIRECTION_LEFT) {
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
                D_801845C0[0] = value;
            } else if (D_8009B394[0] & PAD_DIRECTION_UP) {
                MainMenu_StartValueWidgetTween(0, D_801845BC[2]);
            }
        }
    }

    if (busyB == 0) {
        if (D_801845BC[1] < 2) {
            if (D_8009B394[1] & PAD_DIRECTION_HORIZONTAL_MASK) {
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[1] & PAD_DIRECTION_LEFT) {
                    D_801845BC[2] = 0;
                } else {
                    D_801845BC[2] = 1;
                }
                D_801845BC[1] = D_801845BC[2];
                if (D_801845BC[0] < 2) {
                    D_801845BC[0] = D_801845BC[2];
                }
            } else if (D_8009B394[1] & PAD_DIRECTION_DOWN) {
                MainMenu_StartValueWidgetTween(1, 2);
            }
        } else {
            if (D_8009B394[1] & PAD_DIRECTION_HORIZONTAL_MASK) {
                value = D_801845C0[6];
                func_80048658(6, 0xFF, 0);
                if (D_8009B394[1] & PAD_DIRECTION_LEFT) {
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
                D_801845C0[6] = value;
            } else if (D_8009B394[1] & PAD_DIRECTION_UP) {
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
    ValueWidgetView *w;
    ValueWidgetView *mk;
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

    first = D_801845C0[1];
    second = D_801845C0[7];
    mk = D_801845B0[2];
    if (D_801845BC[2] == 0) {
        x = 116;
    } else {
        x = 220;
    }
    mk->x = x;
    D_801845B0[2]->y = 74;

    w = D_801845B0[0];
    if (w->updateCallbackAddress != 0 || D_801845BC[0] == 2) {
        w->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        w = D_801845B0[0];
        if (w->updateCallbackAddress == 0) {
            w->x = first * 128 / DUEL_STARTING_LIFE_POINTS + 176;
            D_801845B0[0]->y = 111;
        }
    } else {
        w->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        D_801845B0[0]->x = D_801845B0[2]->x;
        D_801845B0[0]->y = D_801845B0[2]->y;
    }

    w = D_801845B0[1];
    if (w->updateCallbackAddress != 0 || D_801845BC[1] == 2) {
        w->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        w = D_801845B0[1];
        if (w->updateCallbackAddress == 0) {
            w->x = second * 128 / DUEL_STARTING_LIFE_POINTS + 176;
            D_801845B0[1]->y = 139;
        }
    } else {
        w->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        D_801845B0[1]->x = D_801845B0[2]->x;
        D_801845B0[1]->y = D_801845B0[2]->y;
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
    u8 *widget;
    s32 targetX;
    s32 targetY;
    s32 valueA;
    s32 valueB;
    s16 remaining;

    widget = D_801845B8;
    targetX = *(s16 *)(widget + 0x30);
    targetY = *(s16 *)(widget + 0x32);
    valueA = D_801845C0[1];
    valueB = D_801845C0[7];
    if (obj[0x6C] == 2) {
        if (obj[0x6B] == 0) {
            targetX = (valueA * 128) / DUEL_STARTING_LIFE_POINTS + 176;
            targetY = 111;
        } else {
            targetX = (valueB * 128) / DUEL_STARTING_LIFE_POINTS + 176;
            targetY = 139;
        }
    }
    remaining = *(u16 *)(obj + 0x60) - 1;
    *(s16 *)(obj + 0x60) = remaining;
    if (remaining <= 0) {
        D_801845BC[obj[0x6B]] = obj[0x6C];
        *(s32 *)(obj + 0x24) = 0;
    }
    *(s16 *)(obj + 0x30) =
        (*(s16 *)(obj + 0x36) * *(s16 *)(obj + 0x60)) / 10 +
        (targetX * (10 - *(s16 *)(obj + 0x60))) / 10;
    *(s16 *)(obj + 0x32) =
        (*(s16 *)(obj + 0x38) * *(s16 *)(obj + 0x60)) / 10 +
        (targetY * (10 - *(s16 *)(obj + 0x60))) / 10;
}

void MainMenu_FinishValueSetup(void)
{
    u8 *state = (u8 *)D_801845C0;

    *(u16 *)(*(u8 **)(state + 4)) = *(u16 *)state;
    *(u16 *)(*(u8 **)(state + 0x10)) = *(u16 *)(state + 0xC);
    *D_801845D8 = (D_801845BE != 1);
    func_8004036C(D_801845A0);
    D_801845A0 = 0;
    func_8004036C(D_801845A4);
    D_801845A4 = 0;
    func_8004036C((u8 *)D_801845B0[0]);
    D_801845B0[0] = 0;
    func_8004036C((u8 *)D_801845B0[1]);
    D_801845B0[1] = 0;
    func_8004036C((u8 *)D_801845B0[2]);
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
    u8 *object = (u8 *)D_801845B0[index];

    object[0x6B] = index;
    *(s16 *)(object + 0x60) = 0xA;
    object[0x6C] = value;
    *(void **)(object + 0x24) = (void *)MainMenu_UpdateValueWidgetTween;
    *(s16 *)(object + 0x36) = *(u16 *)(object + 0x30);
    *(s16 *)(object + 0x38) = *(u16 *)(object + 0x32);
}
