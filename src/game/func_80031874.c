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

#define GS_SPRITE_COLOR_WORD(sprite) (*(u32 *)&(sprite)->r)

#ifdef VERSION_EUROPE
#define CARD_LIST_ROW_FLAGS(row) ((row)->flags)
#define CARD_LIST_ROW_ID(row) ((row)->id)
#define CARD_LIST_ROW_TYPE(row) ((row)->type)
#define CARD_LIST_ROW_COUNT 9
#else
#define CARD_LIST_ROW_FLAGS(row) ((row)[5])
#define CARD_LIST_ROW_ID(row) (*(s16 *)((row) - 4))
#define CARD_LIST_ROW_TYPE(row) ((row)[2])
#define CARD_LIST_ROW_COUNT 8
#endif

/* Both sprites are GsSPRITE records in the scratchpad: `sprite` at 0x1F800020
 * draws the rows' digits and icons, `header` at 0x1F800060 the sort menu, and
 * the digit text is built at 0x1F800000. Europe also uses `header` for row
 * numbers and card ids. Its typed row iteration lets GCC strength-reduce the
 * stat cursor to entry + 6; the other builds retain their entry + 8 cursor.
 *
 * The regional empty-loop boundaries preserve the prologue load schedule. */
void func_80031874(DisplayObject *obj, GsOT *ot)
{
    u8 *text;
    GsSPRITE *sprite = (GsSPRITE *)0x1F800020;
    GsSPRITE *header = (GsSPRITE *)0x1F800060;
#ifdef VERSION_EUROPE
    s32 kind;
    CardList *list;
    CardListRenderEntry *row;
#else
    CardList *list;
    u8 *row;
    s32 kind;
#endif
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
#ifdef VERSION_EUROPE
    sprite->tpage = 0xA;
    header->tpage = 0xB;
#else
    header->tpage = 0xB;
    sprite->tpage = 0xB;
#endif
    GS_SPRITE_COLOR_WORD(sprite) = COLOR_RGB24_NEUTRAL_GREY;
    *(u32 *)&sprite->w = 0x80008;
    *(u32 *)&header->w = 0x100010;
#ifdef VERSION_EUROPE
    sprite->cx = 0x280;
    sprite->cy = 0xC0;
    text = (u8 *)0x1F800000;
#else
    sprite->cx = 0x290;
    sprite->cy = 0xFA;
#endif
    vy = gGraphics_sViewportY;
#ifndef VERSION_EUROPE
    x = x0 - gGraphics_sViewportX;
#endif
    header->attribute = attr;
    sprite->attribute = attr;
#ifdef VERSION_EUROPE
    do {
    } while (0);
#endif
    kind = obj->field_67;
    y = y0 - vy;
#ifndef VERSION_EUROPE
    do {
    } while (0);
#endif
    list = &gBuildDeck_pState->lists[kind];
#ifdef VERSION_EUROPE
    x = x0 - gGraphics_sViewportX;
    row = (CardListRenderEntry *)&list->entries[list->first];
    header->y = y + 0x11;
#else
    row = (u8 *)&list->entries[list->first];
    text = (u8 *)0x1F800000;
#endif
    if (kind == 0) {
        header->x = x + 0x88;
#ifndef VERSION_EUROPE
        header->y = y + 0xF;
#endif
        func_80031784(header, ot, D_80090DD8, list->sort_mode);
    } else {
        header->x = x + 0x6A;
#ifndef VERSION_EUROPE
        header->y = y + 0xF;
#endif
        func_80031784(header, ot, &D_80090DD8[kind * 16],
                      list->sort_mode);
    }
#ifdef VERSION_EUROPE
    *(u32 *)&header->w = 0x100008;
    *(u32 *)&header->cx = 0xC50280;
    header->v = 0;
    header->tpage = 0xA;
#endif
    i = 0;
#ifdef VERSION_EUROPE
    ry = 0x2C;
#else
    ry = 0x2B;
    row += 8;
#endif
    do {
#ifdef VERSION_EUROPE
        header->x = x + 4;
#endif
        sprite->x = x + 4;
#ifdef VERSION_EUROPE
        header->y = ry;
#endif
        sprite->y = ry;
        if (CARD_LIST_ROW_FLAGS(row) != 0) {
            GS_SPRITE_COLOR_WORD(sprite) = COLOR_RGB24_NEUTRAL_GREY;
#ifdef VERSION_EUROPE
            GS_SPRITE_COLOR_WORD(header) = COLOR_RGB24_NEUTRAL_GREY;
#endif
            id = CARD_LIST_ROW_ID(row);
            if (CARD_LIST_ROW_FLAGS(row) & 0x80) {
                GS_SPRITE_COLOR_WORD(sprite) = COLOR_RGB24_DIM_GREY;
#ifdef VERSION_EUROPE
                GS_SPRITE_COLOR_WORD(header) = COLOR_RGB24_DIM_GREY;
#endif
            }
            if (kind != 0) {
#ifdef VERSION_EUROPE
                header->cy = 0xC2;
                header->x = x + 0x11;
#endif
                sprite->x = x + 0x11;
                Text_EncodeDecimalNoPadding(list->first + i + 1, 2, text);
#ifdef VERSION_EUROPE
                func_800317F0(header, ot, text, 2);
                header->cy = 0xC5;
#else
                func_800316F0(sprite, ot, text, 2);
#endif
                sprite->x += 4;
#ifdef VERSION_EUROPE
                header->x += 4;
            } else {
                header->cy = 0xC5;
                if (gBuildDeck_pState->card_sort_rank[id] != 0) {
#else
            } else if (gBuildDeck_pState->card_sort_rank[id] != 0) {
#endif
                sprite->v = 0x68;
                sprite->w = 0x18;
                sprite->u = 0xE8;
#ifdef VERSION_EUROPE
                sprite->tpage = 0xB;
                sprite->cx = 0x310;
                sprite->cy = 0xFA;
                sprite->y += 10;
#else
                sprite->y += 8;
#endif
                GsSortFastSprite(sprite, ot, 0);
#ifdef VERSION_EUROPE
                sprite->cx = 0x280;
                sprite->cy = 0xC0;
                sprite->tpage = 0xA;
#endif
                sprite->w = 8;
#ifdef VERSION_EUROPE
                header->cy = 0xC1;
                sprite->y -= 10;
                header->y -= 4;
                }
#else
                sprite->y -= 8;
#endif
            }
            Text_EncodeDecimalNoPadding(id, 3, text);
#ifdef VERSION_EUROPE
            func_800317F0(header, ot, text, 3);
            sprite->x = header->x + 0x88;
#else
            func_800316F0(sprite, ot, text, 3);
            sprite->x += 0x88;
#endif
            if (CARD_LIST_ROW_TYPE(row) < 0x14) {
#ifdef VERSION_EUROPE
                sprite->cy = 0xC7;
                *(u16 *)&sprite->u = 0x40F8;
                GsSortFastSprite(sprite, ot, 0);
                *(u16 *)&sprite->u = 0x48F8;
                sprite->y += 8;
                GsSortFastSprite(sprite, ot, 0);
                sprite->cy = 0xC0;
                sprite->x += 8;
                Text_EncodeDecimalDigits(row->defense, 4, text);
                func_800316F0(sprite, ot, text, 4);
                sprite->x -= 0x20;
                sprite->y -= 8;
                Text_EncodeDecimalDigits(row->attack, 4, text);
                func_800316F0(sprite, ot, text, 4);
#else
                /* u 0xD0, v 0x58: the ATK label. */
                *(u16 *)&sprite->u = 0x58D0;
                GsSortFastSprite(sprite, ot, 0);
                sprite->x += 8;
                Text_EncodeDecimalDigits(*(s16 *)(row - 2), 4, text);
                func_800316F0(sprite, ot, text, 4);
                /* u 0xD8, v 0x58: the DEF label, one line down. */
                *(u16 *)&sprite->u = 0x58D8;
                sprite->x -= 0x28;
                sprite->y += 8;
                GsSortFastSprite(sprite, ot, 0);
                sprite->x += 8;
                Text_EncodeDecimalDigits(*(s16 *)row, 4, text);
                func_800316F0(sprite, ot, text, 4);
                sprite->y -= 8;
#endif
            }
            if (kind == 0) {
                sprite->x = x + 0x107;
#ifdef VERSION_EUROPE
                sprite->cy = 0xC0;
#endif
                sprite->y += 8;
                Text_EncodeDecimalDigits(
                    gBuildDeck_pState->chest_card_quantities[id], 3, text);
                func_800316F0(sprite, ot, text, 3);
                n = gBuildDeck_pState->deck_card_quantities[id];
                /* Red once the deck holds the limit: three copies, or one of
                   ids 0x11-0x15. */
                if (n >= 3 || ((u32)(id - 0x11) < 5 && n != 0)) {
                    GS_SPRITE_COLOR_WORD(sprite) = 0x2020FF;
                }
                sprite->x = x + 0x122;
                Text_EncodeDecimalDigits(n, 2, text);
                func_800316F0(sprite, ot, text, 2);
                GS_SPRITE_COLOR_WORD(sprite) = COLOR_RGB24_NEUTRAL_GREY;
                sprite->y -= 8;
            }
        }
#ifdef VERSION_EUROPE
        row++;
#else
        row += sizeof(CardEntry);
#endif
        i++;
        ry += 0x16;
    } while (i < CARD_LIST_ROW_COUNT);
}
