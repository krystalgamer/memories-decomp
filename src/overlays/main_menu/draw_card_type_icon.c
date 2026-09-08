#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/card_constants.h"
#include "trade_helpers.h"

extern s32 D_801D4244[];
extern GsOT *D_800E9D94;

void MainMenu_DrawCardTypeIcon(s32 x, s32 y, s32 cardID)
{
    POLY_FT4 sprite;
    u32 palette;
    s32 cardType;

    cardType = (D_801D4244[cardID - 1] >> CARD_STAT_TYPE_SHIFT) &
                CARD_STAT_TYPE_MASK;
    if (cardType != CARD_TYPE_MAGIC && cardType != CARD_TYPE_EQUIP) {
        if (cardType != CARD_TYPE_TRAP) {
            if (cardType == CARD_TYPE_RITUAL) {
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
    setPolyFT4(&sprite);
    sprite.r0 = 0x80;
    sprite.g0 = 0x80;
    sprite.b0 = 0x80;
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
    GsSortPoly(&sprite, D_800E9D94, 0x20);
}
