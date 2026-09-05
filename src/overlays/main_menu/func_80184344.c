#include "../../types.h"
#include "../../game/card_constants.h"

typedef struct {
    u8 pad[3];
    u8 len;
    u8 r;
    u8 g;
    u8 b;
    u8 code;
    s16 x0;
    s16 y0;
    u8 u0;
    u8 v0;
    u16 clut;
    s16 x1;
    s16 y1;
    u8 u1;
    u8 v1;
    u16 tpage;
    s16 x2;
    s16 y2;
    u8 u2;
    u8 v2;
    u16 pad2;
    s16 x3;
    s16 y3;
    u8 u3;
    u8 v3;
    u16 pad3;
} MainMenuSprite;

extern s32 D_801D4244[];
extern void *D_800E9D94;
extern void func_80084320(void *, void *, s32);

void func_80184344(s32 x, s32 y, s32 index)
{
    MainMenuSprite sprite;
    u32 palette;
    s32 attribute;

    attribute = (D_801D4244[index - 1] >> CARD_STAT_TYPE_SHIFT) &
                CARD_STAT_TYPE_MASK;
    if (attribute != CARD_TYPE_MAGIC && attribute != CARD_TYPE_EQUIP) {
        if (attribute != CARD_TYPE_TRAP) {
            if (attribute == CARD_TYPE_RITUAL) {
                palette = 0x290;
            } else {
                palette = 0x260;
            }
        } else {
            palette = 0x280;
        }
    } else {
        palette = 0x270;
    }
    sprite.len = 9;
    sprite.code = 0x2C;
    sprite.r = 0x80;
    sprite.g = 0x80;
    sprite.b = 0x80;
    sprite.tpage = 0xB;
    sprite.clut = (palette >> 4) | 0x3F00;
    sprite.x0 = x;
    sprite.y0 = y;
    sprite.x1 = x + 0x10;
    sprite.y1 = y;
    sprite.x2 = x;
    sprite.y2 = y + 0x10;
    sprite.x3 = x + 0x10;
    sprite.y3 = y + 0x10;
    sprite.u0 = 0;
    sprite.v0 = 0xC8;
    sprite.u1 = 0x10;
    sprite.v1 = 0xC8;
    sprite.u2 = 0;
    sprite.v2 = 0xD8;
    sprite.u3 = 0x10;
    sprite.v3 = 0xD8;
    func_80084320(&sprite, D_800E9D94, 0x20);
}
