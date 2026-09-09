#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/card_constants.h"
#include "../../game/display_object_layout.h"
#include "value_setup.h"

typedef struct {
    u8 pad0[8];
    u16 flags;
    u8 pad0A[36 - 10];
    s32 updateCallbackAddress;
    u8 pad28[48 - 40];
    s16 x;
    s16 y;
} ValueWidgetView;

extern ValueWidgetView *D_801845B0[];
extern u16 D_801845C0[];
extern u8 *D_801845B8;
extern GsOT *D_800E9D90[];

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
