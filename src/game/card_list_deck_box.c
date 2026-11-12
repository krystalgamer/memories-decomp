#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"
#include "color_constants.h"
#include "text_constants.h"
#include "duel_card.h"
#include "card_list_deck_box.h"
#include "card_list_rows.h"
#include "build_deck.h"
#include "duel_effect_text.h"
#include "duel_effect_states.h"
#include "text_staging.h"
#include "text_message_staging.h"

#define GS_SPRITE_COLOR_WORD(sprite) (*(u32 *)&(sprite)->r)

/* Regional values: the stat digits' texture page, CLUT and v, and how the
   type icons' CLUT x is formed from the glyph table's page bits. The
   European build (src/game/european/) defines its own. */
#ifndef CARD_LIST_DIGIT_TPAGE
#define CARD_LIST_DIGIT_TPAGE 0xB
#endif
#ifndef CARD_LIST_DIGIT_CX
#define CARD_LIST_DIGIT_CX 0x290
#endif
#ifndef CARD_LIST_DIGIT_CY
#define CARD_LIST_DIGIT_CY 0xFA
#endif
#ifndef CARD_LIST_DIGIT_V
#define CARD_LIST_DIGIT_V 0x70
#endif
#ifndef CARD_LIST_ICON_CX
#define CARD_LIST_ICON_CX(page) ((page) | 0x200)
#endif

/* Draws a deck box's six type-count digits and its seven card stat rows
   as 16x16 and 8x8 sprites into the ordering table. The glyph table
   gCardList_SortIconTable gives the u/v and page bits of each type icon; the selected
   icon (byte +0x69) is drawn bright. For each card entry of the box
   record (gFile_StagingBuffer, 24 bytes per box) that names a monster, the attack
   and defence from gDuel_adwCardStats are rendered as four decimal digits
   each (through Text_EncodeDecimalDigits), dimmed when the entry's high
   bit is set. */
void CardList_RenderDeckBoxStats(DisplayObject *obj, GsOT *ot) {
    u8 atk[8];
    u8 def[8];
    GsSPRITE *sp;
    u8 *tbl;
    CardListRowSet *rec;
    u16 *entries;
    s32 pri;
    s32 i;
    s32 j;
    s32 id;
    u8 d;

    tbl = gCardList_SortIconTable;
    sp = (GsSPRITE *)0x1F800320;
    rec = &((CardListRowSet *)gFile_StagingBuffer)[obj->field_6A];
    pri = *(s16 *)&obj->ot_depth;
    sp->attribute = obj->attribute;
    sp->tpage = 0xB;
    GS_SPRITE_COLOR_WORD(sp) = COLOR_RGB24_NEUTRAL_GREY;
    *(u32 *)&sp->w = 0x100010;
    sp->x = obj->xy.h.x + 0x22;
    sp->y = obj->xy.h.y - 0x18;
    sp->cy = 0xFB;
    i = 0;
    do {
        GS_SPRITE_COLOR_WORD(sp) = 0x202020;
        if (i == obj->field_69) {
            GS_SPRITE_COLOR_WORD(sp) = COLOR_RGB24_NEUTRAL_GREY;
        }
        sp->u = (tbl[0] & 0xF) * 8 - 0x80;
        sp->v = tbl[0] & 0xF0;
        sp->cx = CARD_LIST_ICON_CX(tbl[1] & 0xF0);
        GsSortFastSprite(sp, ot, pri);
        sp->x += 0x14;
        i++;
        tbl += 2;
    } while (i < 6);

    if (rec->enabled != 0) {
        sp->attribute = obj->attribute;
        sp->x = obj->xy.h.x + 0x24;
        *(u32 *)&sp->w = 0x80008;
        entries = rec->card_entries;
        sp->y = obj->xy.h.y;
        sp->tpage = CARD_LIST_DIGIT_TPAGE;
        sp->cx = CARD_LIST_DIGIT_CX;
        sp->cy = CARD_LIST_DIGIT_CY;
        sp->u = 0x80;
        sp->v = CARD_LIST_DIGIT_V;
        i = 0;
        do {
            id = *entries & 0xFFF;
            if (id != 0) {
                if (((gDuel_adwCardStats[id - 1] >> CARD_STAT_TYPE_SHIFT) &
                     CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                    sp->x = obj->xy.h.x + 0x24;
                    GS_SPRITE_COLOR_WORD(sp) = COLOR_RGB24_NEUTRAL_GREY;
                    if (*entries & 0x8000) {
                        GS_SPRITE_COLOR_WORD(sp) = COLOR_RGB24_DIM_GREY;
                    }
                    Text_EncodeDecimalDigits(
                        (gDuel_adwCardStats[id - 1] & CARD_STAT_VALUE_MASK) *
                            CARD_STAT_SCALE,
                        4, atk);
                    Text_EncodeDecimalDigits(
                        ((gDuel_adwCardStats[id - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                         CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE,
                        4, def);
                    for (j = 3; j >= 0; j--) {
                        d = atk[j];
                        if (d < TEXT_DECIMAL_RADIX) {
                            sp->u = d * 8 - 0x80;
                            GsSortFastSprite(sp, ot, pri);
                        }
                        if (def[j] < TEXT_DECIMAL_RADIX) {
                            sp->y += 8;
                            sp->u = def[j] * 8 - 0x80;
                            GsSortFastSprite(sp, ot, pri);
                            sp->y -= 8;
                        }
                        sp->x += 8;
                    }
                }
            }
            i++;
            sp->y += 0x16;
            entries++;
        } while (i < CARD_LIST_ROW_ENTRY_COUNT);
    }
}

#ifndef CARD_LIST_FOLLOWING_FLAG
#define CARD_LIST_FOLLOWING_FLAG 0x40
#endif

/* Builds the regional card-list text boxes for one row set: each two-halfword
 * input row supplies a card ID and count, staged into gText_StagedValues before its
 * box is created. A monster card uses the base style and anything from the
 * magic type up the next one; an empty count marks the box and the row's
 * entry, and every box after the first gets the regional following-row flag.
 *
 * The staging read goes through the per-row count pointer, while the later
 * empty test indexes the row directly; the European body uses an incremented
 * staging cursor so both stores share one split address. */
void CardList_BuildRowSetTextBoxes(u16 *e, s32 idx, s32 flag, s32 ignored)
{
    CardListRowSet *s;
    DuelEffectChannel *box;
    s32 i;
    s32 style;
    s32 base;
    s32 id;
    u16 *count;
#ifdef VERSION_EUROPE
    u8 *color_slots;
    s32 *stage;
    s32 empty_color;
#endif

    s = &((CardListRowSet *)gFile_StagingBuffer)[idx];
    base = 0x18;
    s->enabled = flag;
    ((DisplayObject *)s->object)->field_68 = flag;
    if (flag != 0) {
        base = 0x1A;
    }
    i = 0;
#ifdef VERSION_EUROPE
    color_slots = gText_abColorSlots;
    empty_color = 4;
#endif
    for (i = 0; i < CARD_LIST_ROW_ENTRY_COUNT; i++) {
#ifdef VERSION_EUROPE
        color_slots[4] = 0;
#endif
        count = e + 1;
        s->card_entries[i] = e[0];
        style = e[0];
        if (style != 0) {
            id = style;
            style = base;
            if (((gDuel_adwCardStats[id - 1] >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
                style = base + 1;
            }
#ifdef VERSION_EUROPE
            stage = &gText_StagedValues[0].card.card_id;
            *stage++ = id;
            gDuel_wSelectedCardID = id;
            *stage = *count;
#else
            gText_StagedValues[0].card.card_id = id;
            gText_StagedValues[0].card.count = *count;
            gDuel_wSelectedCardID = id;
#endif
        }
        box = TextBox_Create(idx + 1, style, s->x, s->y, 0x120, 0xB0);
        box->pen_y = i * 0x16;
        if (e[1] == 0) {
#ifdef VERSION_EUROPE
            box->color_index_54 = empty_color;
#else
            box->color_index_54 = 4;
#endif
            s->card_entries[i] |= 0x8000;
#ifdef VERSION_EUROPE
            color_slots[4] = empty_color;
#endif
        }
        if (i != 0) {
            box->flags_34 |= CARD_LIST_FOLLOWING_FLAG;
        }
        TextBox_BuildBlocking(box);
        e += 2;
    }
}

#include "main.h"
#include "display_object.h"
#include "display_object_system.h"
#include "card_list_rows.h"

/* Keep symbolic usage evidence while preserving the literal-address codegen.
   A regional build supplies its own addresses and row bias. */
#ifndef gGraphics_sViewportY_abs
#define gGraphics_sViewportY_abs (*(s16 *)0x8009B148)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009B146)
#endif
#ifndef CARD_LIST_ROW_X_BIAS
#define CARD_LIST_ROW_X_BIAS 2
#endif

void CardList_CreateDeckBoxStatRows(s32 x0, s32 y0, s32 x1, s32 y1)
{
    CardListRowSet *record = (CardListRowSet *)gFile_StagingBuffer;
    DisplayObject *object;
    s32 i = 0;

    record[0].x = x0 + CARD_LIST_ROW_X_BIAS;
    record[0].y = y0;
    record[1].x = x1 + CARD_LIST_ROW_X_BIAS;
    record[1].y = y1;

#ifdef VERSION_EUROPE
    /* The European build clears the first row set's entries first. */
    for (i = CARD_LIST_ROW_ENTRY_COUNT - 1; i >= 0; i--) {
        record->card_entries[i] = 0;
    }
    i = 0;
#endif

    do {
        record->enabled = 0;
        object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
        object->field_68 = 0;
        object->field_69 = 0;
        object->xy.h.x = record->x;
        object->xy.h.y = record->y;
        object->field_6A = i++;
        DisplayObject_SelectOrderingTable1(object);
        object->field_4C = (s32)CardList_RenderDeckBoxStats;
        record->object = (DuelCardDisplayObject *)object;
        record++;
    } while (i < 2);

    gGraphics_sViewportY_abs = 0;
    gGraphics_sViewportX_abs = 0;
}
