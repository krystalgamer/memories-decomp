#include "../types.h"

/* The Build Deck screen, 0x800313E8 through 0x80033D00: the card transfer
   icon and card stat display, the card-list page and its slots, the deck
   count and transfers, the shared card-list graphics setup, the screen setup,
   its input, panes and per-frame update, and the sort-mode update. */

/* Zero-initialized storage this unit owns at 0x8009B2F8-0x8009B300, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 gBuildDeck_bConfirmMode;
struct BuildDeckTransitionState *gBuildDeck_pState;

#include "display_object_system.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "sound.h"
#include "display_object.h"
#include "build_deck.h"
#include "duel_card.h"
#include "card_constants.h"

/* The update callback BuildDeck_CreateCardTransferIcon (below)
   installs: a card's stat panel
   scales up over eight frames, then flies to its target at 0x18/0x1A, waits,
   and scales back down before releasing itself. field_6C carries the phase --
   0x40 while it grows, 0x20 while it shrinks, neither while it travels.

   Two views of one object, on purpose. DisplayObject names the scale word at
   0x44, the countdown at 0x60, the attribute and the phase byte;
   DisplayObjectVelocity names 0x30/0x32 and 0x36/0x38 as position and
   velocity, which is what this function computes there -- it seeds both
   halves with (target - position) << 8 / 12 and then hands the record to
   DisplayObject_StepPositionXY, whose parameter is that view. Taking the
   velocity names from the view that owns them is what
   display_object_helpers.h asks a caller to do rather than pushing one
   reading of 0x36/0x38 onto the shared record, where it has five. */
void BuildDeck_UpdateCardTransferIcon(DisplayObject *o) {
    DisplayObjectVelocity *motion = (DisplayObjectVelocity *)o;
    s32 v;
    s32 t;

    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)o) == 0) {
        o->field_44.word = 0;
        o->field_60 = 0;
        o->attribute = o->attribute & ~GsROTOFF;
        o->field_6C = o->field_6C | 0x40;
        SD_SEPlayFull(0xA);
    }

    v = o->field_6C;

    if ((v & 0x40) != 0) {
        t = *(u16 *)&o->field_60 + 1;
        o->field_60 = t;
        o->field_44.scale.y = (s16)t * 512;
        o->field_44.scale.x = (s16)t * 512;
        if (o->field_60 >= 8) {
            DisplayObject_ResetVelocity(motion);
            motion->velocity_x = ((*(s16 *)&o->field_18.target.x - motion->x) << 8) / 12;
            o->field_6C = o->field_6C & 0xBF;
            o->field_60 = 0xC;
            motion->velocity_y = ((*(s16 *)&o->field_18.target.y - motion->y) << 8) / 12;
        }
    } else if ((v & 0x20) != 0) {
        t = *(u16 *)&o->field_60 - 1;
        o->field_60 = t;
        o->field_44.scale.y = (s16)t * 512;
        o->field_44.scale.x = (s16)t * 512;
        if (o->field_60 == 0) {
            DisplayObject_ReleaseIfPresent(o);
        }
    } else {
        DisplayObject_StepPositionXY(motion);
        t = *(u16 *)&o->field_60 - 1;
        o->field_60 = t;
        if (o->field_60 <= 0) {
            o->xy.word = *(s32 *)&o->field_18.target.x;
            o->field_60 = 8;
            o->field_6C = o->field_6C | 0x20;
        }
    }
}

/* Regional values: the icon sprite's texture_v (0xC8 -> 0xC0) and clut_x,
 * its CLUT x (0x260 -> 0x2E0). The European build (src/game/european/)
 * defines its own. */
#ifndef DUEL_CARD_TYPE_ICON_FIELD_5D
#define DUEL_CARD_TYPE_ICON_FIELD_5D 0xC8
#endif
#ifndef DUEL_CARD_TYPE_ICON_CX
#define DUEL_CARD_TYPE_ICON_CX 0x260
#endif

#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define DISPLAY_OBJECT_UNSIGNED_HALFWORD(field) (*(u16 *)&(field))

/*
 * The object is the return value, not just a discarded temporary: retail
 * moves it to v0 before the final field stores and retains it through exit.
 * Restoring that return removes the artificial pointer-register binding.
 *
 * A scoped table-base capture keeps the full address ahead of the index
 * arithmetic under gcc_2_8_1_g0_split. The final independent flag updates
 * are written in this order to reproduce the target's scheduling; reversing
 * them changes six words. All 380 bytes match without instruction assembly.
 */
DisplayObject *BuildDeck_CreateCardTransferIcon(s32 index, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    s32 table_index = index;
    s32 x = arg1;
    s32 y = arg2;
    DisplayObjectConfigView *object;
    s32 field_18 = arg3;
    s32 bits;
    s32 kind;
    s32 *stats;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
    DisplayObject_ConfigureScreenSprite(
        DISPLAY_OBJECT_VIEW(object), x, y, 0x10, 0x10,
        0, DUEL_CARD_TYPE_ICON_FIELD_5D, 0xB, DUEL_CARD_TYPE_ICON_CX, 0xFC
    );
    do { stats = gDuel_adwCardStats; } while (0);
    table_index--;
    table_index <<= 2;
    table_index += (s32)stats;
    bits = *(s32 *)table_index;
    kind = (bits >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    switch (kind) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        DISPLAY_OBJECT_UNSIGNED_HALFWORD(object->clut_x) += 0x10;
        break;
    case CARD_TYPE_TRAP:
        DISPLAY_OBJECT_UNSIGNED_HALFWORD(object->clut_x) += 0x20;
        break;
    case CARD_TYPE_RITUAL:
        DISPLAY_OBJECT_UNSIGNED_HALFWORD(object->clut_x) += 0x30;
        break;
    }
    DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(object));
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 10);
    {
        DisplayObject *output = DISPLAY_OBJECT_VIEW(object);
        output->field_44.word = 0;
        output->field_18.target.x = field_18;
        output->field_18.target.y = arg4;
        output->update = (DisplayObjectCallback)BuildDeck_UpdateCardTransferIcon;
        output->flags &= 0xFFF7;
        output->attribute &= ~GsROTOFF;
        return output;
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"
#include "duel_card.h"

/* Regional value: the digit sprites' texture v (0x70 -> 0x40). The European
 * build (src/game/european/) defines its own. */
#ifndef DUEL_CARD_STAT_DIGIT_V
#define DUEL_CARD_STAT_DIGIT_V 0x70
#endif

void CardList_DrawDecimalDigits(GsSPRITE *sprite, GsOT *ot, const u8 *digits, s32 count)
{
    s32 i;

    sprite->v = DUEL_CARD_STAT_DIGIT_V;
    i = count - 1;
    if (i >= 0) {
        do {
            u8 value = digits[i];

            if (value < 0xA) {
                sprite->u = value * 8 - 0x80;
                GsSortFastSprite(sprite, ot, 0);
            }
            *(u16 *)&sprite->x += 8;
            i--;
        } while (i >= 0);
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "text_encode_decimal_digits.h"
#include "text_encode_decimal_no_padding.h"
#include "build_deck_transition_state.h"
#include "color_constants.h"
#include "main.h"
#include "card_list_sort_icons.h"

/* Initialized data this unit owns at 0x80090DD8: two 16-byte rows used by card-list sorting
 * and icon rendering. The packed bytes retain their separate glyph/page
 * and sort-key interpretations; the canonical name adds no record type.
 */
u8 gCardList_SortIconTable[] = {
    0x46, 0x11, 0x48, 0x22, 0x4A, 0x33, 0x4C, 0x44,
    0x4E, 0x55, 0x7A, 0x66, 0x44, 0x08, 0x00, 0x00,
    0x46, 0x11, 0x48, 0x22, 0x4A, 0x33, 0x4C, 0x44,
    0x4E, 0x55, 0x7A, 0x66, 0x7E, 0x79, 0x00, 0x00,
};

#define GS_SPRITE_COLOR_WORD(sprite) (*(s32 *)&(sprite)->r)

#ifdef VERSION_EUROPE
/* European only, right before BuildDeck_DrawSortIcons: draws count digit
   cells from the right (the last byte first), skipping bytes from 10 up,
   and steps the sprite 8 pixels right after each. This is the header-sprite
   counterpart of CardList_DrawDecimalDigits: the caller sets up the
   sprite's texture, and the cells sort at priority 1. */
void CardList_DrawHeaderDecimalDigits(GsSPRITE *record, GsOT *ordering_table, u8 *digits, s32 count)
{
    s32 i;
    s32 digit;

    for (i = count - 1; i >= 0; i--) {
        digit = digits[i];
        if ((u32)digit < 10) {
            record->u = digit * 8 - 128;
            GsSortFastSprite(record, ordering_table, 1);
        }
        record->x += 8;
    }
}
#endif

void BuildDeck_DrawSortIcons(
    GsSPRITE *record, GsOT *ordering_table, u8 *data, s32 selected)
{
    s32 i;
    u8 *cursor;

    record->cy = 251;
    i = 0;
    cursor = data + 1;
    do {
        GS_SPRITE_COLOR_WORD(record) = 0x202020;
        if ((cursor[0] & 15) == selected)
            GS_SPRITE_COLOR_WORD(record) = COLOR_RGB24_NEUTRAL_GREY;
        i++;
        record->u = ((data[0] & 15) << 3) - 128;
        record->v = data[0] & 240;
#ifdef VERSION_EUROPE
        record->cx = (cursor[0] & 240) + 0x280;
#else
        record->cx = (cursor[0] & 240) | 512;
#endif
        cursor += 2;
        GsSortFastSprite(record, ordering_table, 0);
        record->x += 18;
        data += 2;
    } while (i < 7);
}

/* The page drawer reads the colour word unsigned; the sort-icon drawer above
   used a signed view. */
#undef GS_SPRITE_COLOR_WORD
#define GS_SPRITE_COLOR_WORD(sprite) (*(u32 *)&(sprite)->r)

#ifdef VERSION_EUROPE
#define CARD_LIST_ROW_COUNT 9
#else
#define CARD_LIST_ROW_COUNT 8
#endif

/* Both sprites are GsSPRITE records in the scratchpad: `sprite` at 0x1F800020
 * draws the rows' digits and icons, `header` at 0x1F800060 the sort menu, and
 * the digit text is built at 0x1F800000. Europe also uses `header` for row
 * numbers and card ids. Every build walks the page as typed CardEntry rows;
 * GCC strength-reduces the European stat reads to an entry + 6 cursor, while
 * the US and Japanese builds keep a single entry + 8 cursor.
 *
 * The regional empty-loop boundaries preserve the prologue load schedule. */
void BuildDeck_DrawCardListPage(DisplayObject *obj, GsOT *ot)
{
    u8 *text;
    GsSPRITE *sprite = (GsSPRITE *)0x1F800020;
    GsSPRITE *header = (GsSPRITE *)0x1F800060;
#ifdef VERSION_EUROPE
    s32 kind;
    CardList *list;
    CardEntry *row;
#else
    CardList *list;
    CardEntry *row;
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

    x0 = (s16)obj->xy.h.x;
    y0 = (s16)obj->xy.h.y;
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
    row = &list->entries[list->first];
    header->y = y + 0x11;
#else
    row = &list->entries[list->first];
    text = (u8 *)0x1F800000;
#endif
    if (kind == 0) {
        header->x = x + 0x88;
#ifndef VERSION_EUROPE
        header->y = y + 0xF;
#endif
        BuildDeck_DrawSortIcons(header, ot, gCardList_SortIconTable, list->sort_mode);
    } else {
        header->x = x + 0x6A;
#ifndef VERSION_EUROPE
        header->y = y + 0xF;
#endif
        BuildDeck_DrawSortIcons(header, ot, &gCardList_SortIconTable[kind * 16],
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
        if (row->flags != 0) {
            GS_SPRITE_COLOR_WORD(sprite) = COLOR_RGB24_NEUTRAL_GREY;
#ifdef VERSION_EUROPE
            GS_SPRITE_COLOR_WORD(header) = COLOR_RGB24_NEUTRAL_GREY;
#endif
            id = (s16)row->id;
            if (row->flags & 0x80) {
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
                CardList_DrawHeaderDecimalDigits(header, ot, text, 2);
                header->cy = 0xC5;
#else
                CardList_DrawDecimalDigits(sprite, ot, text, 2);
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
            CardList_DrawHeaderDecimalDigits(header, ot, text, 3);
            sprite->x = header->x + 0x88;
#else
            CardList_DrawDecimalDigits(sprite, ot, text, 3);
            sprite->x += 0x88;
#endif
            if (row->type < 0x14) {
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
                CardList_DrawDecimalDigits(sprite, ot, text, 4);
                sprite->x -= 0x20;
                sprite->y -= 8;
                Text_EncodeDecimalDigits(row->attack, 4, text);
                CardList_DrawDecimalDigits(sprite, ot, text, 4);
#else
                /* u 0xD0, v 0x58: the ATK label. */
                *(u16 *)&sprite->u = 0x58D0;
                GsSortFastSprite(sprite, ot, 0);
                sprite->x += 8;
                Text_EncodeDecimalDigits(row->attack, 4, text);
                CardList_DrawDecimalDigits(sprite, ot, text, 4);
                /* u 0xD8, v 0x58: the DEF label, one line down. */
                *(u16 *)&sprite->u = 0x58D8;
                sprite->x -= 0x28;
                sprite->y += 8;
                GsSortFastSprite(sprite, ot, 0);
                sprite->x += 8;
                Text_EncodeDecimalDigits(row->defense, 4, text);
                CardList_DrawDecimalDigits(sprite, ot, text, 4);
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
                CardList_DrawDecimalDigits(sprite, ot, text, 3);
                n = gBuildDeck_pState->deck_card_quantities[id];
                /* Red once the deck holds the limit: three copies, or one of
                   ids 0x11-0x15. */
                if (n >= 3 || ((u32)(id - 0x11) < 5 && n != 0)) {
                    GS_SPRITE_COLOR_WORD(sprite) = 0x2020FF;
                }
                sprite->x = x + 0x122;
                Text_EncodeDecimalDigits(n, 2, text);
                CardList_DrawDecimalDigits(sprite, ot, text, 2);
                GS_SPRITE_COLOR_WORD(sprite) = COLOR_RGB24_NEUTRAL_GREY;
                sprite->y -= 8;
            }
        }
        row++;
        i++;
        ry += 0x16;
    } while (i < CARD_LIST_ROW_COUNT);
}

/* The Build Deck card slots and counts, five contiguous functions from
   CardList_CreateSlotTextBox (0x80031CD4) through
   BuildDeck_TakeCardFromChest: the per-slot card-list text box and the loop
   that builds every visible slot, then the count-box refresh and the two
   transfers that move one card between the chest and the deck. */
#include "text_box_blocking_build.h"
#include "card_constants.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "card_list_text_boxes.h"
#include "build_deck_transition_state.h"
#include "duel_card.h"
#include "text_staging.h"
#include "text_message_staging.h"

/* Builds the text box for one card slot of a trade or deck screen. The entry
   is picked out of the list at the scroll offset plus the slot, and the list
   kind at +0x2D47 selects both the box template and a fixed 0x160 shift. */

#ifndef CARD_LIST_SLOT_BOX_Y
#define CARD_LIST_SLOT_BOX_Y 0x2B
#endif

#ifndef CARD_LIST_SLOT_BOX_HEIGHT
#define CARD_LIST_SLOT_BOX_HEIGHT 0xB0
#endif

#ifndef CARD_LIST_SLOT_FOLLOWING_FLAG
#define CARD_LIST_SLOT_FOLLOWING_FLAG 0x40
#endif

void CardList_CreateSlotTextBox(CardList *list, s32 slot)
{
    DuelEffectChannel *box;
    s32 style;

    gDuel_wSelectedCardID = list->entries[list->first + slot].id;
    style = 0;
    if (list->entries[list->first + slot].flags != 0) {
        style = 6;
#if defined(VERSION_JAPAN) || defined(VERSION_EUROPE)
        /* The later regional builds give a magic or other non-monster card
           its own box style. */
        if (((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >>
              CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
            style = 7;
        }
#endif
    }
    box = TextBox_Create(list->kind + 1, style, 0x22,
                         CARD_LIST_SLOT_BOX_Y, 0x120,
                         CARD_LIST_SLOT_BOX_HEIGHT);
    box->pen_y = slot * 22;
    box->render_object->flags &=
        ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    if ((list->entries[list->first + slot].flags & 0x80) != 0) {
        box->color_index_54 = 4;
    }
    if (list->kind != 0) {
        *(u16 *)&box->rect_x += 0x160;
    }
    if (slot != 0) {
        box->flags_34 |= CARD_LIST_SLOT_FOLLOWING_FLAG;
    }
    TextBox_BuildBlocking(box);
}

void CardList_CreateSlotTextBoxes(CardList *list, s32 count)
{
    s32 i;

    for (i = 0; i < count; i++)
        CardList_CreateSlotTextBox(list, i);
}

/* Regional values: the count box's y (0x17 -> 0x19) and flags (0x100 ->
 * 0x1). The European build (src/game/european/) defines its own. */
#ifndef BUILD_DECK_COUNT_BOX_Y
#define BUILD_DECK_COUNT_BOX_Y 0x17
#endif
#ifndef BUILD_DECK_COUNT_BOX_FLAGS
#define BUILD_DECK_COUNT_BOX_FLAGS 0x100
#endif

/* The Build Deck screen's three contiguous count helpers. They refresh the
   count box, return a card to the chest, and remove a card from it. The unit
   is the complete gcc_2_8_1_g0_split run between the card-list text boxes and
   BuildDeck_RecountDeckCards.

   All three take the screen record as itself. An earlier note here said the state had to be cast at each use because a
   BuildDeckTransitionState * local cost an instruction; that is true of a
   local, and not of the parameter, which is the same incoming register
   either way. */

void BuildDeck_RefreshCountDisplay(BuildDeckTransitionState *record) {
    DuelEffectChannel *p;
    gText_StagedValues[0].build_deck.chest = record->chest_total;
    gText_StagedValues[0].build_deck.deck = record->deck_total;
    p = TextBox_CreateFlagged(3, 0xE, 0x16, BUILD_DECK_COUNT_BOX_Y, 0x280, 0x10, BUILD_DECK_COUNT_BOX_FLAGS);
    TextBox_BuildBlocking(p);
    p->render_object->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void BuildDeck_ReturnCardToChest(BuildDeckTransitionState *base, s32 index)
{
    u32 raw = base->chest_card_quantities[index];
    s32 count = raw & 255;

    if (count == 0) {
        CardEntry *entry = base->lists[0].entries;

        base->chest_total++;
        base->chest_card_quantities[index]++;
        do {
            s32 id = (s16)entry->id;

            entry++;
            if (id == index)
                break;
        } while (1);
        entry--;
        /* Keep the post-search adjustment separate from the flag store. */
        ((volatile CardEntry *)entry)->flags = 1;
        BuildDeck_SortCardList(&base->lists[0]);
    } else if (count != CARD_CHEST_QUANTITY_MAX) {
        s32 next = raw + 1;

        base->chest_card_quantities[index] = next;
        base->chest_total++;
    }
}

void BuildDeck_TakeCardFromChest(BuildDeckTransitionState *state, s32 id)
{
    s32 count = state->chest_card_quantities[id];

    if (count != 0) {
        count--;
        if (count == 0) {
            CardEntry *record = state->lists[0].entries;

            while (1) {
                if ((s16)record->id == id) {
                    break;
                }
                record++;
            }

            record->flags = 0;
            if (state->chest_card_quantities[id] != 0) {
                record->flags = 0x80;
            }
            BuildDeck_SortCardList(&state->lists[0]);
        }
        state->chest_card_quantities[id] = count;
        state->chest_total -= 1;
    }
}

#include "card_constants.h"
#include "build_deck_transition_state.h"

void BuildDeck_RecountDeckCards(BuildDeckTransitionState *state)
{
    s32 index;

    state->deck_card_quantities[0] = 0;
    {
        CardEntry *record;
        s32 record_index;

        for (index = CARD_ID_FIRST; index < CARD_ID_END; index++) {
            state->deck_card_quantities[index] = 0;
            record = state->lists[1].entries;
            for (record_index = 0; record_index < DECK_SIZE; record_index++) {
                /* Deck rows are compared as signed IDs (empty rows hold -1). */
                if (record->flags != 0 && (s16)record->id == index)
                    state->deck_card_quantities[index]++;
                record++;
            }
        }
    }
    {
        CardEntry *record;
        s32 leading;

        record = state->lists[1].entries;
        leading = 0;
        for (index = 0; index < DECK_SIZE; index++) {
            if (record->flags == 0)
                break;
            leading++;
            record++;
        }
        state->deck_total = leading;
    }
}

#include "../game/card_constants.h"
#include "../game/display_object_layout.h"
#include "../game/duel_card.h"
#include "../game/text_box_lifecycle.h"
#include "../game/text_box_runtime.h"
#include "../game/text_staging.h"

/* The Build Deck screen's card counts: the count box refresh, returning a
   copy to the chest, taking one out, recounting the deck, and adding a card
   to the deck list. All five work on the same screen record - the per-card
   counts at +0x5D97, the two totals at +0x5A9C and +0x5AA0, the chest list
   at +4 and the deck list at the second `lists` element - and each re-sorts
   through BuildDeck_SortCardList when it changes what a list shows.

   The four former sources were recorded at gcc_2_8_1_g0_split, gcc_2_8_1_g0
   and gcc_2_8_1_g8. Every member compiles to an identical object at
   gcc_2_8_1_g0_split. Bounded below by the card-list text boxes, which need
   gcc_2_8_1_g0, and above by CardList_LoadSharedGraphicsStage at gcc_2_8_1_cc_g8_as_g0_split. */

void BuildDeck_AddCard(s32 base, s32 card_id)
{
    s32 *record;
    CardEntry *entry;
    s32 index = 0;
    {
        s32 *base = gDuel_adwCardStats;
        s32 record_index = card_id - 1;

        record = &base[record_index];
    }
    entry = ((BuildDeckTransitionState *)base)->lists[1].entries;
    for (; index < DECK_SIZE; index++, entry++) {
        if (entry->flags == 0) {
            entry->flags = 1;
            entry->id = card_id;
            entry->type = (*record >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            entry->attack = (*record & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            entry->defense =
                ((*record >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                CARD_STAT_SCALE;
            BuildDeck_SortCardList(&((BuildDeckTransitionState *)base)->lists[1]);
            BuildDeck_RecountDeckCards((BuildDeckTransitionState *)base);
            return;
        }
    }
}

/* Shared card-list graphics setup and recent-acquisition maintenance.
   Trade and Build Deck use the blocking graphics loader; the editor also
   prunes recent IDs whose current chest quantity is zero. The transfer has
   four stages in NA/JP and five in Europe. */
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "card_constants.h"
#include "image_transfer_rects.h"
#ifdef VERSION_EUROPE
#include "card_preview.h"
#endif
#include "../ygo_types.h"

#ifdef VERSION_EUROPE
void CardList_LoadSharedGraphicsStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 w;
    s32 v;
    s32 t0;
    s32 t1;
    s32 c;
    s32 n;
    s32 u;
    s16 *g;
    s32 m;

    switch (mode) {
    case 0:
        m = 0xFFDDFFFF;
        *(s16 *)&p->field_30.h.counter = 0x300;
        *(s16 *)&p->field_30.h.field_32 = 0x100;
        *(u16 *)&p->w = 0x40;
        t0 = gFile_dwTransferState;
        *(u16 *)&p->h = 0x10;
        gFile_dwTransferState = t0 & m;
        gFile_dwTransferState = gFile_dwTransferState | 0x10000;
        p->transfer_mode = 2;
        v = gFile_TransferBufferBase;
        w = 64 * FILE_SECTOR_SIZE;
        *(s32 *)&p->phase_size = w;
        p->sector_destination = v;
        v += FILE_SECTOR_SIZE;
        p->alternate_sector_destination = v;
        break;

    case 1:
        *(s32 *)&p->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_TransferBufferBase;
        p->sector_destination = gFile_TransferBufferBase;
        p->transfer_mode = 1;
        break;

    case 2:
        g = (s16 *)gGraphics_aImageTransferRects;
        g[0] = 0x380;
        g[1] = 0x160;
        c = 0x40;
        g[2] = c;
        one = 0x10;
        g[3] = one;
        LoadImage2((RECT *)g,
                   (u32 *)(gFile_TransferBufferBase +
                           gText_bLanguageIndex * FILE_SECTOR_SIZE));

        m = 0xFFDDFFFF;
        t1 = gFile_dwTransferState;
        *(s16 *)&p->field_30.h.counter = 0x340;
        *(s16 *)&p->field_30.h.field_32 = 0;
        *(u16 *)&p->w = c;
        *(u16 *)&p->h = one;
        gFile_dwTransferState = t1 & m;
        u = gFile_dwTransferState;
        n = 0x10000;
        gFile_dwTransferState = u | n;
        p->transfer_mode = 2;
        v = gFile_TransferBufferBase;
        w = 8 * FILE_SECTOR_SIZE;
        *(s32 *)&p->phase_size = w;
        p->sector_destination = v;
        v += FILE_SECTOR_SIZE;
        p->alternate_sector_destination = v;
        break;

    case 3:
        *(s32 *)&p->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        p->alternate_sector_destination = gFile_TransferBufferBase;
        p->sector_destination = gFile_TransferBufferBase;
        p->transfer_mode = 1;
        break;

    case 4:
        g = (s16 *)gGraphics_aImageTransferRects;
        g[0] = 0x280;
        g[1] = 0xE0;
        g[2] = 0x100;
        g[3] = 0x10;
        LoadImage2((RECT *)g, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#else
void CardList_LoadSharedGraphicsStage(FileTransferDescriptor *p, s32 mode) {
    s32 one;
    s32 w;
    s32 v;
    s32 t0;
    s32 t1;
    s32 c;
    s32 n;
    s32 u;
    s16 *g;
    s32 m;
    s32 m2v;

    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    if (mode == 3) {
        goto m3;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    *(s16 *)&p->field_30.h.counter = 0x300;
    *(s16 *)&p->field_30.h.field_32 = 0x100;
    *(u16 *)&p->w = 0x40;
    t0 = gFile_dwTransferState;
    *(u16 *)&p->h = 0x10;
    gFile_dwTransferState = t0 & m;
    gFile_dwTransferState = gFile_dwTransferState | 0x10000;
    p->transfer_mode = 2;
    v = gFile_TransferBufferBase;
    w = 0x20000;
    *(s32 *)&p->phase_size = w;
    p->sector_destination = v;
    v += 0x800;
    p->alternate_sector_destination = v;
    return;

m1:
    m = 0xFFDDFFFF;
    *(s16 *)&p->field_30.h.counter = 0x340;
    *(u16 *)&p->w = 0x40;
    t1 = gFile_dwTransferState;
    *(u16 *)&p->h = 0x10;
    gFile_dwTransferState = t1 & m;
    u = gFile_dwTransferState;
    n = 0x10000;
    p->field_30.h.field_32 = 0;
    gFile_dwTransferState = u | n;
    p->transfer_mode = 2;
    v = gFile_TransferBufferBase;
    w = 0x4000;
    *(s32 *)&p->phase_size = w;
    p->sector_destination = v;
    v += 0x800;
    p->alternate_sector_destination = v;
    return;

m2:
    m2v = 0xFFDCFFFF;
    *(s32 *)&p->phase_size = 4 * FILE_SECTOR_SIZE;
    gFile_dwTransferState = gFile_dwTransferState & m2v;
    p->alternate_sector_destination = gFile_TransferBufferBase;
    p->sector_destination = gFile_TransferBufferBase;
    p->transfer_mode = 1;
    return;

m3:
    g = (s16 *)gGraphics_aImageTransferRects;
    c = 0x100;
    g[0] = c;
    g[1] = 0xF0;
    g[2] = c;
    g[3] = 0x10;
    LoadImage2((RECT *)g, (u32 *)gFile_TransferBufferBase);
}
#endif

#ifndef CARD_LIST_GRAPHICS_REQUEST_FILE_ID
#define CARD_LIST_GRAPHICS_REQUEST_FILE_ID 0x2189
#endif
#ifndef CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT
#define CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT 0x4C
#endif

void CardList_LoadSharedGraphics(void)
{
    File_RequestAsyncTransfer(
        0, 0, CARD_LIST_GRAPHICS_REQUEST_FILE_ID, CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT,
        CardList_LoadSharedGraphicsStage, 0, 0);
    File_WaitForTransfers();
}

/*
 * Reward-drop compaction. It matches all 136 bytes under split-address
 * lowering, which keeps the table's high half in a temporary before
 * publishing its address; without splitting, the first two words change.
 */
void Library_PruneRecentCardDrops(void)
{
    s16 *source;
    u8 *base;
    s32 i;
    u32 current;
    s16 *destination;

    source = gDuel_awRecentCardDrops;
    base = (u8 *)((u32)source - DUEL_RECENT_DROPS_CHEST_DISTANCE);
    i = DUEL_RECENT_CARD_DROP_COUNT - 1;
    current = (u32)(source + DUEL_RECENT_CARD_DROP_COUNT - 1);
    for (; i >= 0; i--, current -= sizeof(*source)) {
        if (*(s16 *)current != 0 && base[*(s16 *)current - 1] == 0)
            *(s16 *)current = 0;
    }
    destination = source;
    for (i = 0; i < DUEL_RECENT_CARD_DROP_COUNT; i++, source++) {
        if (*source) {
            if (source != destination) {
                *destination = *source;
                *source = 0;
            }
            destination++;
        }
    }
}

/*
 * BuildDeck_InitScreen sets up the Build Deck screen: it initialises the two
 * 0x6344-byte deck records (recent-drop ranks, the 40-card deck list with
 * its stats, and the 722-card chest list with its counts), sorts both lists,
 * then creates the screen's eleven display objects..
 *
 * Shapes measured on this body under strength reduction:
 * - the list headers, totals and rank row are reached as members of `state`
 *   and the chest sort receives &state->lists[0] last, so loop.c bases every
 *   record address on that state + 4 value, as retail does;
 * - the rank table is indexed by the card id directly; both list loops walk
 *   `entry` (and the deck and quantity cursors) beside the shared counter `j`;
 * - the deck stats are read as gDuel_adwCardStats[card - 1], which keeps the
 *   decrement ahead of the scale;
 * - `card` holds the deck card id and then the chest card id, and `n` is
 *   both the drop index and the chest total. Those shared lifetimes, the
 *   `on` local and the literal 0x80 decide the $a0-$t3 allocation.
 *
 * The display objects keep their raw slot offsets on the record.
 */
#include "duel_card.h"
#include "build_deck_transition_state.h"
#include "sound.h"
#include "card_list_sort_icons.h"

#ifndef BUILD_DECK_CURSOR_Y
#define BUILD_DECK_CURSOR_Y 0x29
#endif

#ifndef BUILD_DECK_LIST_Y
#define BUILD_DECK_LIST_Y 0x2A
#endif

void BuildDeck_InitScreen(BuildDeckTransitionState *base, void *deck, s32 other, s32 flags)
{
    BuildDeckTransitionState *state;
    u8 *object;
    u8 *label;
    CardList *chest_list;
    CardList *deck_list;
    CardEntry *entry;
    u8 *quantity;
    s16 *drops;
    u16 *cards;
    u8 *icons;
    s32 pane;
    s32 j;
    s32 on;
    s32 total;
    s32 n;
    s32 card;
#ifdef VERSION_EUROPE
    s32 quad_kind;
    s32 shade;
    DisplayObject *quad;
#endif

    CardList_LoadSharedGraphics();
    SD_BGMPlay(0x70E0);
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    state = base;
    Library_PruneRecentCardDrops();
    pane = 0;
    on = 1;
    icons = gCardList_SortIconTable;
    gBuildDeck_pState = state;
    ((u8 *)state)[0x6343] = flags;
    state[0].deck_cards = deck;
    state[1].deck_cards = (u16 *)other;
    state[0].pane_index = 0;
    state[1].pane_index = 0;
    do {
        if (state->deck_cards != 0) {
            drops = gDuel_awRecentCardDrops;
            state->card_sort_rank[0] = 0;
            for (j = 1; j < CARD_ID_END; j++) {
                state->card_sort_rank[j] = 0;
                for (n = 15; n >= 0; n--) {
                    if (drops[n] == j) {
                        state->card_sort_rank[j] = n + 1;
                    }
                }
            }
            deck_list = &state->lists[1];
            total = 0;
            state->lists[1].kind = on;
            state->lists[1].first_target = 0;
            state->lists[1].first = 0;
            state->lists[1].cursor = 0;
            state->lists[1].sort_choice = 0;
            state->lists[1].sort_mode =
                icons[state->lists[1].kind * 16 + 1] & 0xF;
            cards = state->deck_cards;
            entry = deck_list->entries;
            for (j = 0; j < DECK_SIZE; j++, entry++, cards++) {
                entry->flags = 0;
                entry->id = 0;
                card = *cards;
                if (card != 0) {
                    entry->id = card;
                    entry->flags = on;
                    entry->type = (gDuel_adwCardStats[card - 1] >> 26) & 0x1F;
                    entry->attack = (gDuel_adwCardStats[card - 1] & 0x1FF) * 10;
                    total++;
                    entry->defense = ((gDuel_adwCardStats[card - 1] >> 9) & 0x1FF) * 10;
                }
            }
            /* Keep the signed -1 materialization, not unsigned 65535. */
            *(s16 *)&deck_list->entries[DECK_SIZE].id = -1;
            state->deck_total = total;
            state->lists[1].row_count = DECK_SIZE;
            state->lists[1].sort_row_count = DECK_SIZE;
            BuildDeck_SortCardList(&state->lists[1]);
            BuildDeck_RecountDeckCards(state);
            chest_list = &state->lists[0];
            n = 0;
            chest_list->kind = 0;
            chest_list->first_target = 0;
            chest_list->first = 0;
            chest_list->row_count = CARD_COUNT;
            chest_list->cursor = 0;
            chest_list->sort_choice = 0;
            chest_list->sort_mode = icons[chest_list->kind * 16 + 1] & 0xF;
            quantity = (u8 *)state->deck_cards + 0x50;
            entry = chest_list->entries;
            for (j = 0; j < CARD_COUNT; j++, entry++, quantity++) {
                card = j + 1;
                entry->flags = 0;
                entry->id = card;
                entry->type = (gDuel_adwCardStats[j] >> 26) & 0x1F;
                entry->attack = (gDuel_adwCardStats[j] & 0x1FF) * 10;
                entry->defense = ((gDuel_adwCardStats[j] >> 9) & 0x1FF) * 10;
                state->chest_card_quantities[card] = *quantity;
                if (*quantity != 0) {
                    entry->flags = on;
                    n += *quantity;
                } else if (state->deck_card_quantities[card] != 0) {
                    entry->flags = 0x80;
                }
            }
            chest_list->entries[CARD_COUNT].id = 0;
            state->chest_total = n;
            state->lists[0].sort_row_count = CARD_COUNT;
            state->lists[0].row_count = CARD_COUNT;
            BuildDeck_SortCardList(&state->lists[0]);
        }
        pane++;
        state++;
    } while (pane < 2);

    state = gBuildDeck_pState;
#ifdef VERSION_EUROPE
    quad_kind = 4;
#endif
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 0, 0xC, 0x208);
#ifdef VERSION_EUROPE
    shade = 0x808080;
#endif
    label = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DISPLAY_OBJECT_VIEW(label)->field_67 = 0;
    DISPLAY_OBJECT_VIEW(label)->xy.word =
        DISPLAY_OBJECT_VIEW(object)->xy.word;
    DISPLAY_OBJECT_VIEW(label)->field_4C = (s32)BuildDeck_DrawCardListPage;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 0, 4, 1, 0xC, 0x208);
    label = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DISPLAY_OBJECT_VIEW(label)->field_67 = 1;
    DISPLAY_OBJECT_VIEW(label)->xy.word =
        DISPLAY_OBJECT_VIEW(object)->xy.word;
    DISPLAY_OBJECT_VIEW(label)->field_4C = (s32)BuildDeck_DrawCardListPage;
    state->state = 2;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x136, BUILD_DECK_CURSOR_Y, 0, 4, 0xC, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 8);
    *(u8 **)((u8 *)state + 0x2D3C) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x26A, BUILD_DECK_CURSOR_Y, 0, 4, 0xC, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 8);
    *(u8 **)((u8 *)state + 0x5A88) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0, BUILD_DECK_LIST_Y, 0, 4, 2, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    *(u8 **)((u8 *)state + 0x2D38) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 0x148, BUILD_DECK_LIST_Y, 0, 4, 3, 0xC, 0x218);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    *(u8 **)((u8 *)state + 0x5A84) = object;
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 9, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 0, 4, 0xA, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), 0xA);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), -4);
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 0, 4, 0xB, 0xC, 0x208);
    DISPLAY_OBJECT_VIEW(object)->flags |= 0x20;
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), -4);
#ifdef VERSION_EUROPE
    quad = DISPLAY_OBJECT_VIEW(DisplayObject_AcquireSlot(
        DisplayObject_FindFreeGeneralSlot(), quad_kind));
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -3);
    quad->position.word = 0xF80000;
    quad->xy.word = 0xF80140;
    quad->field_38.word = 0x1000000;
    quad->field_40.word = 0x1000140;
    quad->field_3C.word = shade;
    quad->field_44.word = shade;
    quad = DISPLAY_OBJECT_VIEW(DisplayObject_AcquireSlot(
        DisplayObject_FindFreeGeneralSlot(), quad_kind));
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -3);
    quad->position.word = 0;
    quad->xy.word = 0x140;
    quad->field_38.word = 0x80000;
    quad->field_40.word = 0x80140;
    quad->field_2C.word = shade;
    quad->field_34.word = shade;
#endif
    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, 0x140, 0, 3, 0, 3, 0xB, 0x2F8);
    DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(object), -4);
    BuildDeck_RefreshCountDisplay(state);
}

s32 BuildDeck_TestAndSetInitialized(BuildDeckTransitionState *state)
{
    u16 flags = state->state;
    if ((flags & 0x8000) == 0) {
        state->state = flags | 0x8000;
        return 0;
    }
    return 1;
}

/* The Build Deck card list, panes and per-frame update, fourteen contiguous
   functions from BuildDeck_CompareSortKeyDescending (0x80032B60) through
   BuildDeck_EnterWideConfirmMode: the descending and ascending unsigned-key
   comparators, both with ascending name-key ties, the sort and list input;
   the active-card lookup, deck and chest pane input and the pane transition;
   the open-slot test, exit confirmation and BuildDeck_Update with its step
   table; and the two confirm-mode setters. */
#include "duel_card.h"
#include "../psyq/qsort.h"
#include "card_list_text_boxes.h"
#include "rand_get_interval.h"
#include "card_constants.h"
#include "build_deck_transition_state.h"
#include "display_object.h"
#include "input.h"
#include "sound.h"
#include "card_list_sort_icons.h"
#include "duel_card_viewer.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "fade.h"
#include "fade_overlay_controls.h"
#include "text_box_lifecycle.h"
#include "text_box_update_channels.h"
#include "display_object_depth_bases.h"
#include "duel_transition_step_table.h"
#include "../psyq/rand.h"
#include "dialog_choice.h"
#include "build_deck_draw_card_list_page_data.h"
#include "main_mode_state.h"

s32 BuildDeck_CompareSortKeyDescending(
    CardEntry *arg0,
    CardEntry *arg1
)
{
    u32 a = arg0->key;
    u32 b = arg1->key;

    if (a == b) {
        if (
            gCard_asNameSortKey[((s16)arg0->id) - 1] <
            gCard_asNameSortKey[((s16)arg1->id) - 1]
        )
            return -1;
        return 1;
    }
    if (a < b)
        return 1;
    return -1;
}

#ifndef CARD_LIST_VISIBLE_ROWS
#define CARD_LIST_VISIBLE_ROWS 8
#endif

s32 BuildDeck_CompareSortKeyAscending(
    CardEntry *arg0,
    CardEntry *arg1
)
{
    u32 a = arg0->key;
    u32 b = arg1->key;

    if (a == b) {
        if (
            gCard_asNameSortKey[((s16)arg0->id) - 1] <
            gCard_asNameSortKey[((s16)arg1->id) - 1]
        )
            return -1;
        return 1;
    }
    if (b < a)
        return 1;
    return -1;
}

/* Card-list sort. Builds a 32-bit sort key into each sixteen-byte row of the
   list at p and hands the block to qsort with one of two comparators, chosen
   by the list's sort_mode.

   Levers that mattered here:
   - The rows are walked with a pointer, not indexed. Indexing p[i] lets
     combine_givs merge the key access at +0 with the id/flag accesses at +4
     into a single giv and loses eight increments; walking the pointer keeps
     retail's second giv at +4.
   - The stat reads are written inline at every use rather than through a
     local. A local for gDuel_adwCardStats[id - 1] keeps the value in its own
     register and shifts the whole loop's allocation by one.
   - The key expressions are inline for the same reason: with atk and def as
     locals gcc schedules both multiplies before either shift, where retail
     finishes the attack half before starting the defence half.
   - Halved stats are spelled value * (CARD_STAT_SCALE / 2) and the comparison
     multiplies back up. Written the other way round, as
     (value * CARD_STAT_SCALE) >> 1, gcc folds the shift into the constant and
     emits sll 16 where retail has sll 17.
   - The first loop increments the index before the row pointer and every
     other loop the row pointer first, which is the order retail schedules the
     two induction variables in each. */

void BuildDeck_SortCardList(CardList *list)
{
    CardEntry *row;
    s32 i;
    s32 n;

    n = list->sort_row_count;
    row = list->entries;
    switch (list->sort_mode) {
    case 1:
        if (list->kind != 0) {
            for (i = 0; i < n; i++, row++) {
                row->key = 0xFFFF;
                if (row->flags != 0) {
                    row->key = ((s16)row->id);
                }
            }
        } else {
            for (i = 0; i < n; row++, i++) {
                row->key = ((s16)row->id);
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 2:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = 0;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 3:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flags != 0) {
                if ((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE >=
                    ((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                        CARD_STAT_SCALE) {
                    row->key =
                        (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 17) |
                        ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                } else {
                    row->key =
                        ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) &
                           CARD_STAT_VALUE_MASK) *
                          (CARD_STAT_SCALE / 2))
                         << 17) |
                        (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                         << 3) |
                        1;
                }
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyDescending);
        break;
    case 4:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flags != 0) {
                row->key =
                    (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 17) |
                    ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyDescending);
        break;
    case 5:
        for (i = 0; i < n; row++, i++) {
            row->key = 0;
            if (row->flags != 0) {
                row->key =
                    ((((gDuel_adwCardStats[((s16)row->id) - 1] >> CARD_STAT_DEFENSE_SHIFT) & CARD_STAT_VALUE_MASK) *
                      (CARD_STAT_SCALE / 2))
                     << 17) |
                    (((gDuel_adwCardStats[((s16)row->id) - 1] & CARD_STAT_VALUE_MASK) * (CARD_STAT_SCALE / 2))
                     << 3) |
                    1;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyDescending);
        break;
    case 6:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = (gDuel_adwCardStats[((s16)row->id) - 1] >>
                            CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 8:
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = 0x100;
                if (gBuildDeck_pState->card_sort_rank[((s16)row->id)] != 0) {
                    row->key = gBuildDeck_pState->card_sort_rank[((s16)row->id)];
                }
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    case 9:
        n = list->row_count;
        for (i = 0; i < n; row++, i++) {
            row->key = -1;
            if (row->flags != 0) {
                row->key = Rand_GetInterval(0x1000);
            }
        }
        qsort(list, n, sizeof(CardEntry), BuildDeck_CompareSortKeyAscending);
        break;
    }
    CardList_CreateSlotTextBoxes(list, CARD_LIST_VISIBLE_ROWS);
}

/* The two explicit volatile .data input views retain absolute pad loads under
 * the uniform -G8 compiler/assembler profile used by the surrounding card
 * list sort code. */

#ifndef CARD_LIST_LAST_CURSOR
#define CARD_LIST_LAST_CURSOR 7
#endif
#ifndef CARD_LIST_SCROLL_SCALE
#define CARD_LIST_SCROLL_SCALE 152
#endif
#ifndef CARD_LIST_SCROLL_Y_BASE
#define CARD_LIST_SCROLL_Y_BASE 0x29
#endif
#ifndef CARD_LIST_CURSOR_Y_BASE
#define CARD_LIST_CURSOR_Y_BASE 0x2A
#endif

/* Card-list cursor and page input handler.

   Levers that mattered here:
   - The retry path is a backward goto, not a for(;;)/continue loop: with a
     real loop construct loop.c hoists the last cursor constant into a
     callee-saved register instead of rematerialising it at each use.
   - The shared sub-row commit tail is written once and reached by
     "goto finish" from the second branch, which reproduces the retail block
     order (the tail sits between the L1/R1 block and the L2/R2 block).
     Duplicating it in both branches leaves the two copies unmerged.
   - The visible-row subtraction is respelled at every use; a single local
     would collapse the reloads the target makes at each join point.
   - The gCardList_SortIconTable lookup keeps CardList_UpdateSortMode's statement-by-statement
     spelling so the base and slot pseudos land in the same registers.
*/

s32 BuildDeck_UpdateCardListInput(CardList *list)
{
    s32 row;
    s32 sel;
    s32 icon;
    s32 slot;
    u8 *entry;

    row = list->first | list->cursor;
    if (row != 0) {
        row = (list->first + list->cursor + 1) * CARD_LIST_SCROLL_SCALE /
              list->sort_row_count;
    }
    list->scroll_box->xy.h.y = row + CARD_LIST_SCROLL_Y_BASE;

top:
    if (list->first != list->first_target) {
        if (list->first_target < list->first) {
            *(u16 *)&list->first = *(u16 *)&list->first - 1;
        } else {
            *(u16 *)&list->first = *(u16 *)&list->first + 1;
        }
        CardList_CreateSlotTextBoxes(list, CARD_LIST_VISIBLE_ROWS);
        return 1;
    }

    if ((gInput_awPadHeld[0] & PAD_BUTTON_L1_R1_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_awPadHeld[0] & PAD_BUTTON_R1) != 0) {
            if (row == list->row_count - CARD_LIST_VISIBLE_ROWS &&
                list->cursor != CARD_LIST_LAST_CURSOR) {
                sel = CARD_LIST_LAST_CURSOR;
            }
            row += CARD_LIST_VISIBLE_ROWS;
            if (list->row_count - CARD_LIST_VISIBLE_ROWS < row) {
                row = list->row_count - CARD_LIST_VISIBLE_ROWS;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= CARD_LIST_VISIBLE_ROWS;
        }
        if (row < 0) {
            row = 0;
        }
        list->first_target = row;
        if (list->first != row) {
            SD_SEPlayFull(6);
            goto top;
        }
    finish:
        if (sel >= 0) {
            list->cursor = sel;
            list->cursor_box->xy.h.y =
                sel * 22 + CARD_LIST_CURSOR_Y_BASE;
            SD_SEPlayFull(6);
        }
        return 1;
    }

    if ((gInput_awPadRepeat[0] & PAD_BUTTON_TRIGGER_MASK) != 0) {
        sel = -1;
        row = list->first;
        if ((gInput_awPadRepeat[0] & PAD_BUTTON_R2) != 0) {
            if (row == list->row_count - CARD_LIST_VISIBLE_ROWS &&
                list->cursor != CARD_LIST_LAST_CURSOR) {
                sel = CARD_LIST_LAST_CURSOR;
            }
            row += 0x32;
            if (list->row_count - CARD_LIST_VISIBLE_ROWS < row) {
                row = list->row_count - CARD_LIST_VISIBLE_ROWS;
            }
        } else {
            if (row == 0 && list->cursor != 0) {
                sel = 0;
            }
            row -= 0x32;
            if (row < 0) {
                row = 0;
            }
        }
        if (list->first != row) {
            SD_SEPlayFull(6);
            list->first_target = row;
            list->first = row;
            CardList_CreateSlotTextBoxes(list, CARD_LIST_VISIBLE_ROWS);
            return 1;
        }
        goto finish;
    }

    if ((gInput_awPadRepeat[0] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
        row = list->first;
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_DOWN) != 0) {
            list->cursor += 1;
            if (list->cursor >= CARD_LIST_VISIBLE_ROWS) {
                row += 1;
                list->cursor = CARD_LIST_LAST_CURSOR;
            }
        } else {
            list->cursor -= 1;
            if (list->cursor < 0) {
                row -= 1;
                list->cursor = 0;
            }
        }
        list->cursor_box->xy.h.y =
            list->cursor * 22 + CARD_LIST_CURSOR_Y_BASE;
        if (list->row_count - CARD_LIST_VISIBLE_ROWS < row) {
            return 1;
        }
        if (row < 0) {
            return 1;
        }
        SD_SEPlayFull(6);
        list->first_target = row;
        if (list->first != row) {
            goto top;
        }
        return 1;
    }

    if ((gInput_awPadRepeat[0] & (PAD_BUTTON_SELECT | PAD_BUTTON_START)) !=
        0) {
        if ((gInput_awPadRepeat[0] & PAD_BUTTON_START) != 0) {
            list->sort_choice += 1;
            if (list->sort_choice >= 7) {
                list->sort_choice = 0;
            }
        } else {
            list->sort_choice -= 1;
            if (list->sort_choice < 0) {
                list->sort_choice = 6;
            }
        }
        SD_SEPlayFull(0x2F);
        icon = list->kind;
        slot = list->sort_choice;
        icon = icon << 4;
        slot = slot << 1;
        entry = gCardList_SortIconTable + slot;
        icon = icon + (s32)entry;
        list->sort_mode = *(u8 *)(icon + 1) & 0xF;
        BuildDeck_SortCardList(list);
        return 1;
    }

    return 0;
}

s32 BuildDeck_GetActiveCardID(CardList *list)
{
    s32 index = list->first + list->cursor;
    CardEntry *entry = &list->entries[index];
    if (entry->flags == 0) {
        return 0;
    }
    return (s16)entry->id;
}

/* Regional values: the two BuildDeck_CreateCardTransferIcon calls' x, y and last argument (deck
 * pane 0x234, 0x16, 0xA -> 0x235, 0x14, 0xC; chest pane y 0x18 -> 0x14 and
 * last argument 0xC -> 0xD). The European build (src/game/european/) defines
 * its own. */
#ifndef BUILD_DECK_DECK_PANE_X
#define BUILD_DECK_DECK_PANE_X 0x234
#endif
#ifndef BUILD_DECK_DECK_PANE_Y
#define BUILD_DECK_DECK_PANE_Y 0x16
#endif
#ifndef BUILD_DECK_DECK_PANE_ARG4
#define BUILD_DECK_DECK_PANE_ARG4 0xA
#endif
#ifndef BUILD_DECK_CHEST_PANE_Y
#define BUILD_DECK_CHEST_PANE_Y 0x18
#endif
#ifndef BUILD_DECK_CHEST_PANE_ARG4
#define BUILD_DECK_CHEST_PANE_ARG4 0xC
#endif

/* The cancel and confirm buttons. The Japanese release swaps Cross and Circle,
 * so a regional build names its own. */
#ifndef BUILD_DECK_PANE_CANCEL_BUTTON
#define BUILD_DECK_PANE_CANCEL_BUTTON PAD_BUTTON_CANCEL
#endif
#ifndef BUILD_DECK_PANE_CONFIRM_MASK
#define BUILD_DECK_PANE_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif

void BuildDeck_UpdateDeckPaneInput(BuildDeckTransitionState *state) {
    CardList *e;
    s32 r;

    e = &state->lists[state->pane_index];

    BuildDeck_TestAndSetInitialized(state);

    if (BuildDeck_UpdateCardListInput(e) != 0) {
        return;
    }

    if ((gInput_awPadPressed[0] & PAD_BUTTON_TRIANGLE) != 0) {
        r = BuildDeck_GetActiveCardID(e);
        if (r != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = r;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        }
        return;
    }

    if (gInput_awPadHeld[0] == PAD_DIRECTION_LEFT) {
        state->state = 1;
        state->viewport_target_x = 0;
        state->next_state = 2;
        return;
    }

    if ((gInput_awPadPressed[0] & BUILD_DECK_PANE_CANCEL_BUTTON) != 0) {
        state->state = 4;
        state->next_state = 3;
        return;
    }

    if ((gInput_awPadRepeat[0] & BUILD_DECK_PANE_CONFIRM_MASK) != 0) {
        r = BuildDeck_GetActiveCardID(e);
        if (r != 0) {
            SD_SEPlayFull(7);
            e->entries[e->first + e->cursor].flags = 0;
            BuildDeck_SortCardList(&state->lists[1]);
            BuildDeck_RecountDeckCards(state);
            BuildDeck_ReturnCardToChest(state, r);
            BuildDeck_RefreshCountDisplay(state);
            BuildDeck_CreateCardTransferIcon(r, BUILD_DECK_DECK_PANE_X, BUILD_DECK_DECK_PANE_Y, 0x162, BUILD_DECK_DECK_PANE_ARG4);
            return;
        }
        SD_SEPlayFull(9);
    }
}

void BuildDeck_UpdateChestPaneInput(BuildDeckTransitionState *state)
{
    CardList *e;
    s32 r;
    u32 c;

    e = &state->lists[state->pane_index];
    BuildDeck_TestAndSetInitialized(state);
    if (BuildDeck_UpdateCardListInput(e) != 0) {
        return;
    }

    if ((gInput_awPadPressed[0] & PAD_BUTTON_TRIANGLE) != 0) {
        r = BuildDeck_GetActiveCardID(e);
        if (r != 0) {
            gDuel_bCardViewerYOffset = 0x14;
            gDuel_wViewerCardID = r;
            gDuel_bEffectState = DUEL_EFFECT_STATE_CARD_VIEWER;
        }
        return;
    }

    if (gInput_awPadHeld[0] == PAD_DIRECTION_RIGHT) {
        state->viewport_target_x = 0x140;
        state->state = 1;
        state->next_state = 3;
        return;
    }

    if ((gInput_awPadPressed[0] & BUILD_DECK_PANE_CANCEL_BUTTON) != 0) {
        state->state = 4;
        state->next_state = 2;
        return;
    }

    if ((gInput_awPadRepeat[0] & BUILD_DECK_PANE_CONFIRM_MASK) == 0) {
        return;
    }

    r = BuildDeck_GetActiveCardID(e);
    c = 1;
    if ((u32)(r - EXODIA_FIRST_CARD_ID) < EXODIA_PIECE_COUNT) {
        c = state->deck_card_quantities[r] < c;
    }

    if (r != 0 && c != 0) {
        if (state->deck_total < DECK_SIZE &&
            state->chest_card_quantities[r] != 0 &&
            state->deck_card_quantities[r] < DECK_CARD_COPY_LIMIT) {
            SD_SEPlayFull(7);
            BuildDeck_AddCard((s32)state, r);
            BuildDeck_TakeCardFromChest(state, r);
            BuildDeck_RefreshCountDisplay(state);
            BuildDeck_CreateCardTransferIcon(r, 3, BUILD_DECK_CHEST_PANE_Y, 0x11C, BUILD_DECK_CHEST_PANE_ARG4);
            return;
        }
    }

    SD_SEPlayFull(9);
}

#define BUILD_DECK_PANE_TRANSITION_TICKS 16

void BuildDeck_UpdatePaneTransition(BuildDeckTransitionState *state)
{
    s32 ticks;

    if (BuildDeck_TestAndSetInitialized(state) == 0) {
        s32 diff = state->viewport_target_x - (s16)gGraphics_sViewportX;

        state->viewport_step_x = diff / BUILD_DECK_PANE_TRANSITION_TICKS;
        state->transition_ticks = BUILD_DECK_PANE_TRANSITION_TICKS;
        SD_SEPlayFull(30);
    }

    gGraphics_sViewportX += (u16)state->viewport_step_x;
    ticks = state->transition_ticks - 1;
    state->transition_ticks = ticks;
    if (ticks == 0) {
        u16 position = (u16)state->viewport_target_x;

        state->pane_index = 0;
        gGraphics_sViewportX = position;
        if ((s32)position << 16) {
            state->pane_index = 1;
        }
        state->state = state->next_state;
    }
}

/* Initialized data this unit owns at 0x80090DF8 and 0x80090E0C, previously a generated blob
   (#2602).

   gBuildDeck_apfnStepHandlers is the build-deck pane transition step table.
   BuildDeck_Update dispatches gBuildDeck_apfnStepHandlers[state & 0x3F] with the
   shared BuildDeckTransitionState. The mask permits sixty-four entries and
   there are five; that is recorded rather than corrected, because nothing
   shows the game produces a larger index and widening the array would change
   the bytes.

   gText_DecimalDivisorView's first seven words are the divisor ladder
   Text_EncodeDecimalDigits indexes by digit count and then walks with
   d /= TEXT_DECIMAL_RADIX: 1, 1, 10, 100, 1000, 10000, 100000.

   The glyph nibble patterns and colour masks follow it. Only Japan's text
   renderer reads them (DuelEffect entries), but every image carries them. */
void (*gBuildDeck_apfnStepHandlers[])(BuildDeckTransitionState *) = {
    BuildDeck_UpdatePaneTransition,
    BuildDeck_UpdatePaneTransition,
    BuildDeck_UpdateChestPaneInput,
    BuildDeck_UpdateDeckPaneInput,
    BuildDeck_UpdateExitConfirmation,
};

s32 gText_DecimalDivisorView[] = {
    /* The divisor ladder, indexed by digit count. */
    0x00000001,
    0x00000001,
    0x0000000A,
    0x00000064,
    0x000003E8,
    0x00002710,
    0x000186A0,
};

/* The European build's divisor table ends with the ladder; these two do not
 * follow it there. */
#ifndef BUILD_DECK_DIVISOR_LADDER_ONLY
/* Japan's text renderer expands each glyph nibble through these sixteen
 * patterns and masks the result with the channel's colour. */
u16 gText_awGlyphNibblePatterns[16] = {
    0x0000,
    0xF000,
    0xEF00,
    0xFF00,
    0x0EF0,
    0xFEF0,
    0xEFF0,
    0xFFF0,
    0x00EF,
    0xF0EF,
    0xEFEF,
    0xFFEF,
    0x0EFF,
    0xFEFF,
    0xEFFF,
    0xFFFF,
};

u16 gText_awGlyphColorMasks[8] = {
    0xFFFF,
    0xBBBB,
    0x7777,
    0x9999,
    0xDDDD,
    0x5555,
    0x3333,
    0x0000,
};
#endif

/* Regional values: the confirmation boxes' y and flags (wide 0x78, 0x1028 ->
 * 0x84, 0x1050; narrow 0x60, 0x20 -> 0x6C, 0x40), the fade level (0xA0 ->
 * 0xC0) and the completion mask (0x2008 -> 0x2010). The European build
 * (src/game/european/) defines its own. */
#ifndef BUILD_DECK_WIDE_BOX_Y
#define BUILD_DECK_WIDE_BOX_Y 0x78
#endif
#ifndef BUILD_DECK_WIDE_BOX_FLAGS
#define BUILD_DECK_WIDE_BOX_FLAGS 0x1028
#endif
#ifndef BUILD_DECK_NARROW_BOX_Y
#define BUILD_DECK_NARROW_BOX_Y 0x60
#endif
#ifndef BUILD_DECK_NARROW_BOX_FLAGS
#define BUILD_DECK_NARROW_BOX_FLAGS 0x20
#endif
#ifndef BUILD_DECK_CONFIRM_FADE_LEVEL
#define BUILD_DECK_CONFIRM_FADE_LEVEL 0xA0
#endif
#ifndef BUILD_DECK_CONFIRM_COMPLETION_MASK
#define BUILD_DECK_CONFIRM_COMPLETION_MASK TEXT_BOX_COMPLETION_MASK
#endif

#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)

/* Reports whether the deck has a free slot: the first of DECK_SIZE entries in
 * gBuildDeck_pState->lists[1] whose flags byte is zero. The exit step below
 * is its only caller. It was its own gcc_2_8_1_g8 unit and is byte-identical
 * at gcc_2_8_1_g8_split. */
int BuildDeck_HasOpenDeckSlot(void)
{
    int index;
    CardEntry *entry = gBuildDeck_pState->lists[1].entries;
    for (index = 0; index < DECK_SIZE; index++, entry++) {
        if (entry->flags == 0) {
            return 1;
        }
    }
    return 0;
}

/* Handles leaving the deck editor. When the editor's own check passes, the
 * confirm sound plays and, if the deck has an open slot
 * (BuildDeck_HasOpenDeckSlot), bit 14 of the state word at +0x633E is set, a
 * confirmation box is created (the wide
 * one in the 640-wide mode selected by bit 7 of gBuildDeck_bConfirmMode, the narrow one
 * otherwise, which is then waited on until its prompt_object pointer is filled),
 * Fade_SetTargetLevel(0xA0, 2) runs and gFade_bKeepOverlayDepth is set from gDisplayObject_awOrderingTableDepthBases[1] - 8.
 * With bit 14 set the effect channel at gText_aChannels is polled: once its flags
 * match TEXT_BOX_FLAG_DONE under TEXT_BOX_COMPLETION_MASK, the box is
 * destroyed and either bit 14
 * is cleared (narrow mode with a choice made) or the state word is reloaded
 * from +0x6340 and Fade_SetTargetLevel(0xFF, 2) runs. Without bit 14 the CARD_COUNT
 * trunk bytes at +0x5D98 are copied after the DECK_SIZE halfwords of the
 * object at +0. Those halfwords are rebuilt from the 0x10-byte entries at
 * +0x2D54 whose byte 9 is set, Library_PruneRecentCardDrops runs and the state word is
 * cleared. */
void BuildDeck_UpdateExitConfirmation(BuildDeckTransitionState *record)
{
    BuildDeckTransitionState *workspace = record;
    DuelEffectChannel *box;
    u8 *src;
    u8 *dst;
    u16 *slot;
    CardEntry *entry;
    s32 mode;
    s32 i;

    if (BuildDeck_TestAndSetInitialized(workspace) == 0) {
        SD_SEPlayFull(8);
        if (BuildDeck_HasOpenDeckSlot() != 0) {
            /* The mode byte is read before the flag store, as retail
               schedules it. */
            mode = gBuildDeck_bConfirmMode & BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
            workspace->state |= 0x4000;
            if (mode) {
                ((DuelEffectChannel *)TextBox_CreateFlagged(
                    0, 8, 0x28, BUILD_DECK_WIDE_BOX_Y, 0xF0, 0x10, BUILD_DECK_WIDE_BOX_FLAGS
                ))->depth_offset = 0xA;
            } else {
                box = TextBox_CreateFlagged(0, 9, 0x30, BUILD_DECK_NARROW_BOX_Y, 0xE0, 0x30, BUILD_DECK_NARROW_BOX_FLAGS);
                box->depth_offset = 0xA;
                do {
                    TextBox_UpdateChannels();
                } while (box->prompt_object == 0);
            }
            Fade_SetTargetLevel(BUILD_DECK_CONFIRM_FADE_LEVEL, 2);
            gFade_bKeepOverlayDepth = *(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] - 8;
        }
    }
    /* Computed before the branch: it is only used on the copy path, so it
       crosses no call, and reorg lifts it into the branch delay slot. */
    src = &workspace->chest_card_quantities[CARD_ID_FIRST];
    if (workspace->state & 0x4000) {
        TextBox_UpdateChannels();
        /* The same variable as the confirmation box, which keeps the
           channel in $s0 across the destroy call. */
        box = gText_aChannels;
        if ((*(u32 *)&box->flags_34 & BUILD_DECK_CONFIRM_COMPLETION_MASK) ==
            TEXT_BOX_FLAG_DONE) {
            TextBox_Destroy(box);
            if (!(gBuildDeck_bConfirmMode & BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG) &&
                gDialog_bChoice != 0) {
                workspace->state &= 0xBFFF;
            } else {
                workspace->state = workspace->next_state;
                Fade_SetTargetLevel(0xFF, 2);
            }
        }
    } else {
        /* One counter for both loops, initialised before the pointers each
           time; that is what ranks the counter and the pointers into
           retail's argument registers. */
        i = 0;
        dst = (u8 *)(workspace->deck_cards + DECK_SIZE);
        for (; i < CARD_COUNT; i++) {
            *dst++ = *src++;
        }
        slot = workspace->deck_cards;
        i = 0;
        entry = workspace->lists[1].entries;
        for (; i < DECK_SIZE; i++) {
            *slot = 0;
            if (entry->flags != 0) {
                *slot = entry->id;
            }
            slot++;
            entry++;
        }
        Library_PruneRecentCardDrops();
        workspace->state = 0;
    }
}

/* Per-frame Build Deck transition driver. It pulses both pane colours, then
 * dispatches the current step through gBuildDeck_apfnStepHandlers; BuildDeck_UpdateExitConfirmation above is the
 * table's exit step. */
s32 BuildDeck_Update(void)
{
    s32 intensity;
    s32 color;
    BuildDeckTransitionState *base;
    DisplayObject *first;
    DisplayObject *second;

    rand();

    intensity = gMain_dwFrameCounter & 0x3F;
    if (intensity >= 0x20) {
        intensity = 0x3F - intensity;
    }

    base = gBuildDeck_pState;
    color = intensity * 2 + 0x40;
    first = base->lists[0].cursor_box;
    second = base->lists[1].cursor_box;

    DISPLAY_OBJECT_COLOR_BYTES(second)[2] = color;
    DISPLAY_OBJECT_COLOR_BYTES(second)[1] = color;
    DISPLAY_OBJECT_COLOR_BYTES(second)[0] = color;
    DISPLAY_OBJECT_COLOR_BYTES(first)[2] = color;
    DISPLAY_OBJECT_COLOR_BYTES(first)[1] = color;
    DISPLAY_OBJECT_COLOR_BYTES(first)[0] = color;

    if (DuelEffect_UpdateState() == 0) {
        gBuildDeck_apfnStepHandlers[gBuildDeck_pState->state & 0x3F](gBuildDeck_pState);
    }

    return gBuildDeck_pState->state;
}

void BuildDeck_EnterNarrowConfirmMode(void)
{
    gBuildDeck_bConfirmMode = 0;
    gMain_bActiveModeState = 7;
}

void BuildDeck_EnterWideConfirmMode(void)
{
    gBuildDeck_bConfirmMode = BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG;
    gMain_bActiveModeState = 7;
}

#include "card_list_sort_icons.h"

void CardList_UpdateSortMode(CardList *a0) {
    int a1;
    int v1;
    u8 *p;
    u8 v0;
    a1 = a0->kind;
    v1 = a0->sort_choice;
    a1 = a1 << 4;
    v1 = v1 << 1;
    p = gCardList_SortIconTable + v1;
    v0 = p[a1 + 1];
    v0 = v0 & 0xf;
    a0->sort_mode = v0;
}
