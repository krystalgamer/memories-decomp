#include "../types.h"
#include "checkerboard_background.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

#define CHECKERBOARD_CELL_WIDTH 0x40
#define CHECKERBOARD_CELL_HEIGHT 0x20
#define CHECKERBOARD_SCREEN_WIDTH 0x140
/* Regional values: the European build (src/game/european/) defines its own. */
#ifndef CHECKERBOARD_SCREEN_HEIGHT
#define CHECKERBOARD_SCREEN_HEIGHT 0xF0
#endif
/* GsSPRITE cx: the CLUT x of the first cell colour; the second is 0x10 on. */
#ifndef CHECKERBOARD_CLUT_X
#define CHECKERBOARD_CLUT_X 0x2C0
#endif
#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))

void func_8003D32C(void)
{
}

/* Tiles the 320x240 background with alternating 64x32 sprite cells. */
void Checkerboard_DrawBackground(
    CheckerboardState *state, GsOT *ordering_table)
{
    CheckerboardSprite *sprite = (CheckerboardSprite *)0x1F800000;
    s32 y;
    s32 flip;
    s32 mode;
    s32 x;
    s32 attribute;
    s32 rgb;

    attribute = state->attribute;
    sprite->wh = 0x200020;
    sprite->attribute = attribute;
    rgb = state->rgb;
    sprite->tpage = 0xB;
    sprite->cx = CHECKERBOARD_CLUT_X;
    sprite->cy = 0xFC;
    mode = state->ot_depth;
    sprite->uv.word = 0x3030;
    sprite->rgb = rgb;

    flip = 0;
    y = 0;
    do {
        x = 0;
        sprite->x = flip << 5;
        sprite->uv.bytes.u = 0x30;
        sprite->y = y;
        sprite->cx = CHECKERBOARD_CLUT_X;
        do {
            GsSortFastSprite(GS_SPRITE_VIEW(sprite), ordering_table,
                             mode & 0xFFFF);
            x += CHECKERBOARD_CELL_WIDTH;
            sprite->x =
                sprite->x + CHECKERBOARD_CELL_WIDTH;
        } while (x < CHECKERBOARD_SCREEN_WIDTH);

        x = 0;
        sprite->x = (flip ^ 1) << 5;
        sprite->uv.bytes.u = 0x50;
        sprite->cx = CHECKERBOARD_CLUT_X + 0x10;
        do {
            GsSortFastSprite(GS_SPRITE_VIEW(sprite), ordering_table,
                             mode & 0xFFFF);
            x += CHECKERBOARD_CELL_WIDTH;
            sprite->x =
                sprite->x + CHECKERBOARD_CELL_WIDTH;
        } while (x < CHECKERBOARD_SCREEN_WIDTH);

        y += CHECKERBOARD_CELL_HEIGHT;
        flip ^= 1;
    } while (y < CHECKERBOARD_SCREEN_HEIGHT);
}
