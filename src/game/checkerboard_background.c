#include "../types.h"

typedef struct {
    u8 pad_00[4];
    s32 field_04;
    u8 pad_08[4];
    s32 field_0C;
    u8 pad_10[4];
    s16 field_14;
} CheckerboardState;

/* PS1 scratchpad SPRT-primitive builder, reused for every sprite drawn. */
typedef struct {
    u32 field_00;
    u16 field_04;
    u16 field_06;
    u32 field_08;
    u16 field_0C;
    u8 field_0E;
    u8 field_0F;
    u16 field_10;
    u16 field_12;
    u32 field_14;
} CheckerboardSprite;

extern void func_800849F0(void *, void *, s32);

void func_8003D32C(void)
{
}

/* Tiles the 320x240 background with alternating 64x32 sprite cells. */
void func_8003D334(CheckerboardState *state, void *ordering_table)
{
    CheckerboardSprite *sprite = (CheckerboardSprite *)0x1F800000;
    s32 y;
    s32 flip;
    s32 mode;
    s32 x;
    s32 field_04;
    s32 field_0C;

    field_04 = state->field_04;
    sprite->field_08 = 0x200020;
    sprite->field_00 = field_04;
    field_0C = state->field_0C;
    sprite->field_0C = 0xB;
    sprite->field_10 = 0x2C0;
    sprite->field_12 = 0xFC;
    mode = state->field_14;
    *(u16 *)&sprite->field_0E = 0x3030;
    sprite->field_14 = field_0C;

    flip = 0;
    y = 0;
    do {
        x = 0;
        sprite->field_04 = flip << 5;
        sprite->field_0E = 0x30;
        sprite->field_06 = y;
        sprite->field_10 = 0x2C0;
        do {
            func_800849F0(sprite, ordering_table, mode & 0xFFFF);
            x += 0x40;
            sprite->field_04 = sprite->field_04 + 0x40;
        } while (x < 0x140);

        x = 0;
        sprite->field_04 = (flip ^ 1) << 5;
        sprite->field_0E = 0x50;
        sprite->field_10 = 0x2D0;
        do {
            func_800849F0(sprite, ordering_table, mode & 0xFFFF);
            x += 0x40;
            sprite->field_04 = sprite->field_04 + 0x40;
        } while (x < 0x140);

        y += 0x20;
        flip ^= 1;
    } while (y < 0xF0);
}
