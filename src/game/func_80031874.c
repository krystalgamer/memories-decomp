#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "text_encode_decimal_digits.h"
#include "text_encode_decimal_no_padding.h"
#include "card_type_icon_table.h"
#include "build_deck_transition_state.h"
#include "color_constants.h"
#include "duel_card_stat_display.h"
#include "func_80031784.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "func_80031874.h"

/* Both sprites are GsSPRITE records in the scratchpad: `sprite` at 0x1F800020
 * draws the rows' digits and icons, `header` at 0x1F800060 the sort menu, and
 * the digit text is built at 0x1F800000. `row` points 8 bytes into the page's
 * first CardEntry: -4 is the card id, -2 and 0 its ATK and DEF, +2 its type
 * (below 0x14 for monsters) and +5 its flags.
 *
 * The empty loop ends the prologue's basic block before the list lookup. With
 * it, GCC's scheduler keeps the viewport subtraction for x where retail has
 * it, instead of sinking it to the branch. */
void func_80031874(DisplayObject *obj, GsOT *ot)
{
    u8 *text;
    GsSPRITE *sprite = (GsSPRITE *)0x1F800020;
    GsSPRITE *header = (GsSPRITE *)0x1F800060;
    CardList *list;
    u8 *row;
    s32 kind;
    s32 x0;
    s32 x;
    s32 vy;
    s32 y0;
    s32 y;
    s32 i;
    s32 ry;
    s32 id;
    s32 n;
    u32 attr;

    x0 = (s16)obj->field_30.h.field_30;
    y0 = (s16)obj->field_30.h.field_32;
    attr = obj->attribute;
    header->tpage = 0xB;
    sprite->tpage = 0xB;
    *(u32 *)&sprite->r = COLOR_RGB24_NEUTRAL_GREY;
    *(u32 *)&sprite->w = 0x80008;
    *(u32 *)&header->w = 0x100010;
    sprite->cx = 0x290;
    sprite->cy = 0xFA;
    vy = gGraphics_sViewportY;
    x = x0 - gGraphics_sViewportX;
    header->attribute = attr;
    sprite->attribute = attr;
    kind = obj->field_67;
    y = y0 - vy;
    do {
    } while (0);
    list = &gBuildDeck_pState->lists[kind];
    row = (u8 *)&list->entries[list->first];
    text = (u8 *)0x1F800000;
    if (kind == 0) {
        header->x = x + 0x88;
        header->y = y + 0xF;
        func_80031784(header, (s32)ot, D_80090DD8, list->sort_mode);
    } else {
        header->x = x + 0x6A;
        header->y = y + 0xF;
        func_80031784(header, (s32)ot, &D_80090DD8[kind * 16],
                      list->sort_mode);
    }
    i = 0;
    ry = 0x2B;
    row += 8;
    do {
        sprite->x = x + 4;
        sprite->y = ry;
        if (row[5] != 0) {
            *(u32 *)&sprite->r = COLOR_RGB24_NEUTRAL_GREY;
            id = *(s16 *)(row - 4);
            if (row[5] & 0x80) {
                *(u32 *)&sprite->r = COLOR_RGB24_DIM_GREY;
            }
            if (kind != 0) {
                sprite->x = x + 0x11;
                Text_EncodeDecimalNoPadding(list->first + i + 1, 2, text);
                func_800316F0((u8 *)sprite, (s32)ot, text, 2);
                sprite->x += 4;
            } else if (gBuildDeck_pState->card_sort_rank[id] != 0) {
                sprite->v = 0x68;
                sprite->w = 0x18;
                sprite->u = 0xE8;
                sprite->y += 8;
                GsSortFastSprite(sprite, ot, 0);
                sprite->w = 8;
                sprite->y -= 8;
            }
            Text_EncodeDecimalNoPadding(id, 3, text);
            func_800316F0((u8 *)sprite, (s32)ot, text, 3);
            sprite->x += 0x88;
            if (row[2] < 0x14) {
                /* u 0xD0, v 0x58: the ATK label. */
                *(u16 *)&sprite->u = 0x58D0;
                GsSortFastSprite(sprite, ot, 0);
                sprite->x += 8;
                Text_EncodeDecimalDigits(*(s16 *)(row - 2), 4, text);
                func_800316F0((u8 *)sprite, (s32)ot, text, 4);
                /* u 0xD8, v 0x58: the DEF label, one line down. */
                *(u16 *)&sprite->u = 0x58D8;
                sprite->x -= 0x28;
                sprite->y += 8;
                GsSortFastSprite(sprite, ot, 0);
                sprite->x += 8;
                Text_EncodeDecimalDigits(*(s16 *)row, 4, text);
                func_800316F0((u8 *)sprite, (s32)ot, text, 4);
                sprite->y -= 8;
            }
            if (kind == 0) {
                sprite->x = x + 0x107;
                sprite->y += 8;
                Text_EncodeDecimalDigits(
                    gBuildDeck_pState->chest_card_quantities[id], 3, text);
                func_800316F0((u8 *)sprite, (s32)ot, text, 3);
                n = gBuildDeck_pState->deck_card_quantities[id];
                /* Red once the deck holds the limit: three copies, or one of
                   ids 0x11-0x15. */
                if (n >= 3 || ((u32)(id - 0x11) < 5 && n != 0)) {
                    *(u32 *)&sprite->r = 0x2020FF;
                }
                sprite->x = x + 0x122;
                Text_EncodeDecimalDigits(n, 2, text);
                func_800316F0((u8 *)sprite, (s32)ot, text, 2);
                *(u32 *)&sprite->r = COLOR_RGB24_NEUTRAL_GREY;
                sprite->y -= 8;
            }
        }
        row += 0x10;
        i++;
        ry += 0x16;
    } while (i < 8);
}
