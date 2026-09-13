#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/card_constants.h"
#include "../../game/gpu_packets.h"
#define MAIN_MENU_TRADE_SCROLL_AS_WORDS
#include "trade_helpers.h"
#include "../../ygo_types.h"
#include "../../game/card_list_rows.h"
#include "card_tables.h"
#include "ordering_tables.h"

/* Three Trade screen helpers: the card-type icon (0x80184344) and the column
   dimming overlay (0x80184454) that the offer draw calls for each entry, and
   the inventory row rebuild (0x801844D8).

   The three are contiguous from 0x80184344 to 0x80184558 and are the whole
   gcc_2_8_1_g0_split run there: card_name_count_comparators.c below compiles
   at gcc_2_8_1_g0_no_sched2_split and the module's data section starts above,
   so both bounds are hard.

   Unlike the offer group they do not call each other. What ties them is the
   screen and its data: all three take their declarations from
   trade_helpers.h, and the row rebuild walks D_801845FC and D_801845E0, the
   same trade inventory and display handle the offer group writes. */

void MainMenu_DrawCardTypeIcon(s32 x, s32 y, s32 cardID)
{
    POLY_FT4 sprite;
    u32 palette;
    s32 cardType;

    cardType = (gDuel_adwCardStats[cardID - 1] >> CARD_STAT_TYPE_SHIFT) &
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

void MainMenu_DrawTradeColumnOverlay(s32 column)
{
    POLY_F4 quad;
    s32 left = column * 160;
    s32 right = left + 0xA0;

    setPolyF4(&quad);
    quad.r0 = 0x40;
    quad.g0 = 0x40;
    quad.b0 = 0x40;
    quad.x0 = left;
    quad.y0 = 0;
    quad.x1 = right;
    quad.y1 = 0;
    quad.x2 = left;
    quad.y2 = 0xF0;
    quad.x3 = right;
    quad.y3 = 0xF0;
    func_8005B260((u32 *)&quad, (GsOT *)D_800E9D94, 0x1F, 2);
}

void MainMenu_RebuildTradeInventoryRows(s32 side)
{
    s32 flags;

    flags = D_801845E0->frame - 4;
    func_80060E70((u16 *)(side * 2888 + (s32)D_801845FC + D_80185C8C[side][0] * 4), side,
                  flags & (1 << side), flags);
}
