#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#include "duel_grid.h"
#include "duel_selection_layout.h"
#include "duel_get_card_viewer_request_id.h"
#include "input.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_card_can_act_this_turn.h"
#include "card_constants.h"

/* The duel screen's digit-sprite drawing, in address order. The card-frame
   drawer Duel_DrawCardFrame and its output-position callback
   Duel_DrawCardAtOutputPosition draw a card with its number digits, and the
   life-point and deck-count readout follows: Duel_DrawDecimalDigits sorts one row of
   digit sprites, Duel_UpdateLifePointDisplay steps the drawn life-point
   counter a frame towards the real total, and
   Duel_DrawLifePointsAndDeckCounts calls both for each side. Both halves
   draw from the same texture page and CLUT, so one regional value names it.

   The callback was recorded at gcc_2_8_1_g8, and Duel_DrawDecimalDigits and
   Duel_UpdateLifePointDisplay at gcc_2_8_1_g0_split and gcc_2_8_1_g8; all
   compile to identical objects at this unit's gcc_2_8_1_g8_split. */

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "text_encode_decimal_digits.h"
#include "duel_card.h"
#include "display_object_projection.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"
#include "duel_draw_card_frame.h"
#include "duel_draw_card_output_position.h"
#include "../psyq/libgs.h"
#include "duel_side_state.h"
#include "../ygo_types.h"
#include "duel_display.h"
#include "duel_grid.h"
#include "ordering_tables.h"
#include "duel_card_and_status_draw.h"

/* Regional values: the screen height the frame is drawn above (0xF0 ->
 * 0x100), and the cxcy of the CLUT that the card frame and the status digits
 * both draw with on texture page 0x1E, CLUT (0x100, 0xF1) -> (0x280, 0xE1).
 * The European build (src/game/european/) defines its own. */
#ifndef DUEL_CARD_FRAME_SCREEN_HEIGHT
#define DUEL_CARD_FRAME_SCREEN_HEIGHT 0xF0
#endif
#ifndef DUEL_SPRITE_CLUT_CXCY
#define DUEL_SPRITE_CLUT_CXCY 0xF10100
#endif

#define POLY_FT4_BYTES(packet) ((u8 *)(packet))
#define CARD_FRAME_SCRATCH_BYTES(scratch) ((u8 *)(scratch))

/* Draws one card's frame on the duel field: position, the hand/field
 * flags, the card-number digits (via Duel_CalcCardStats and
 * Text_EncodeDecimalDigits) and the face/back sprite, through
 * DisplayObject_SubmitPacket on the 0x1F8003xx scratchpad records.
 *
 * The two records it fills are SpritePrim, the GsSPRITE sprite_primitive.h
 * already spells for the two display-object renderers, which build theirs at
 * the same 0x1F800320. This one builds a second at 0x1F800000 for the card
 * art and reuses 0x1F800320 for the number digits, the spell-class frame and
 * the card face, resetting the fields it needs between submissions.
 *
 * The third scratch record, at 0x1F800344, is the POLY_FT4 the clip path
 * primes (len 9, code 0x2C) with the object's colour, and the record at
 * 0x1F8003E0 is CardFrameScratch above.
 *
 * All three reads of the colour word at 0x0C sit between scratchpad stores,
 * and the target keeps every one of them where the source puts it. That used
 * to need the read spelled through a cast, because a member read is a struct
 * reference and GCC 2.8.1 floats one across stores that are not. With the
 * scratchpad records typed, those stores are struct references too, the
 * reads stay put, and every offset this function touches is a member. */
void Duel_DrawCardFrame(DisplayObject *object, GsOT *ordering_table, s32 arg2, s32 arg3) {
    u8 sp18[8];
    u8 sp20[8];
    DuelCardRecord *card;
    s32 fl;
    s32 i;
    SpritePrim *k;
    CardFrameScratch *o;
    s32 n;
    s32 d;
    s32 g1;
    s32 g2;
    s32 t;
    SpritePrim *z;
    POLY_FT4 *y;

    if ((u32)(arg2 + 0x33) < 0x173) {
        if (arg3 >= -0x3B) {
            if (arg3 < DUEL_CARD_FRAME_SCREEN_HEIGHT) {
                o = (CardFrameScratch *)0x1F8003E0;
                k = (SpritePrim *)0x1F800320;
                y = (POLY_FT4 *)0x1F800344;
                z = (SpritePrim *)0x1F800000;
                card = &gDuel_aCardRecords[object->field_6A];
                o->pos.h.x = arg2;
                o->pos.h.y = arg3;
                fl = object->ot_depth | 0x10000;
                t = object->attribute;
                k->attribute = t;
                z->attribute = t;
                object->field_69 = 0;
                if (object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) {
                    fl = object->ot_depth | 0xF0000;
                    if (DisplayObject_ProjectRotationFacing(object,
                                      (s16)o->pos.h.x + 0x1A,
                                      (s16)o->pos.h.y + 0x1E,
                                      (struct ProjectionOut *)0x1F8003E0) < 0) {
                        return;
                    }
                    g1 = object->rgb;
                    setlen(y, 9);
                    *(s32 *)&y->r0 = g1;
                    y->code = 0x2C;
                    if ((object->field_20.word & 0xFFFFFF) == 0) {
                        k->attribute = k->attribute | 0x80;
                        z->attribute = k->attribute;
                    }
                }
                k->tpage = 0x1E;
                g2 = object->rgb;
                k->cxcy.word = DUEL_SPRITE_CLUT_CXCY;
                k->rgb = g2;
                n = object->field_67;
                if (n != 0) {
                    if (n < 0x29) {
                        k->extent.word = 0x10000C;
                        k->uv.b.hi = 0x70;
                        k->xy.h.x = o->pos.h.x + 0xF;
                        k->xy.h.y = o->pos.h.y + 0xF;
                        k->uv.b.lo = (n / 10) * 0xC;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->uv.b.lo = (n % 10) * 0xC;
                        k->xy.h.x = k->xy.h.x + 0xC;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                    }
                } else if (object->field_69 == 0) {
                    k->extent.word = 0x100020;
                    k->uv.word = 0x6000;
                    k->xy.h.x = o->pos.h.x + 0xA;
                    k->xy.h.y = o->pos.h.y + 0x28;
                    switch (object->field_68) {
                    case CARD_TYPE_EQUIP:
                        k->uv.b.lo = 0x20;
                    case CARD_TYPE_MAGIC:
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->cxcy.h.cy = k->cxcy.h.cy + 1;
                        break;
                    case CARD_TYPE_TRAP:
                        k->uv.b.lo = 0x40;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->cxcy.h.cy = k->cxcy.h.cy + 2;
                        break;
                    case CARD_TYPE_RITUAL:
                        k->uv.b.lo = 0x60;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->cxcy.h.cy = k->cxcy.h.cy + 3;
                        break;
                    default:
                        k->extent.word = 0x100008;
                        k->uv.word = 0x7078;
                        k->xy.h.x = o->pos.h.x + 5;
                        DisplayObject_SubmitPacket(
                            k, POLY_FT4_BYTES(y), ordering_table, fl,
                            CARD_FRAME_SCRATCH_BYTES(o));
                        k->uv.b.hi = 0x58;
                        k->extent.wh.h = 8;
                        k->xy.h.x = o->pos.h.x + 0xE;
                        d = Duel_CalcCardStats(card);
                        Text_EncodeDecimalDigits((s16)d, 4, sp18);
                        Text_EncodeDecimalDigits(d >> 0x10, 4, sp20);
                        i = 3;
                        do {
                            k->uv.b.lo = sp18[i] * 8;
                            DisplayObject_SubmitPacket(
                                k, POLY_FT4_BYTES(y), ordering_table, fl,
                                CARD_FRAME_SCRATCH_BYTES(o));
                            k->xy.h.y = k->xy.h.y + 8;
                            k->uv.b.lo = sp20[i] * 8;
                            DisplayObject_SubmitPacket(
                                k, POLY_FT4_BYTES(y), ordering_table, fl,
                                CARD_FRAME_SCRATCH_BYTES(o));
                            i--;
                            k->xy.h.x = k->xy.h.x + 8;
                            k->xy.h.y = k->xy.h.y - 8;
                        } while (i >= 0);
                        break;
                    }
                    z->tpage = 0xE;
                    z->rgb = object->rgb;
                    z->xy.h.x = o->pos.h.x + 6;
                    z->xy.h.y = o->pos.h.y + 6;
                    i = (s8)card->table_index;
                    z->cxcy.h.cx = 0x380;
                    z->cxcy.h.cy = i + 0xE0;
                    z->uv.b.lo = (i % 5) * 0x28;
                    z->extent.word = 0x200028;
                    z->uv.b.hi = (i / 5) << 5;
                    DisplayObject_SubmitPacket(
                        z, POLY_FT4_BYTES(y), ordering_table, fl,
                        CARD_FRAME_SCRATCH_BYTES(o));
                }
                k->extent.word = 0x3C0034;
                k->uv.word = 0x8000;
                k->xy.word = o->pos.word;
                if (object->field_67 != 0) {
                    k->uv.word = 0xC000;
                }
                if (object->field_69 != 0) {
                    k->uv.word = 0x8038;
                    k->cxcy.word = DUEL_SPRITE_CLUT_CXCY;
                }
                DisplayObject_SubmitPacket(
                    k, POLY_FT4_BYTES(y), ordering_table, fl,
                    CARD_FRAME_SCRATCH_BYTES(o));
            }
        }
    }
}

void Duel_DrawCardAtOutputPosition(DisplayObjectPosition *object, s32 arg1)
{
    Duel_DrawCardFrame((DisplayObject *)object, (GsOT *)arg1, object->out_x, object->out_y);
}

void Duel_DrawDecimalDigits(
    DisplayObject *style,
    GsSPRITE *digit,
    int source,
    int count
) {
    u8 temp[8];
    int i;

    Text_EncodeDecimalDigits(source, count, temp);
    for (i = count - 1; i >= 0; i--) {
        digit->u = temp[i] << 3;
        GsSortFastSprite(
            digit,
            gGraphics_apOrderingTables[style->ot_index],
            style->ot_depth
        );
        digit->x += 8;
    }
}

/* Steps the drawn life-point counter one frame towards the real total. The
 * step grows with the distance left, which is what makes the readout race
 * for a large swing and crawl for a small one.
 */
void Duel_UpdateLifePointDisplay(DuelSideState *side)
{
    s32 difference =
        side->displayed_life_points - side->life_points.signed_value;
    /* This order and the builtin preserve the target a1/a2 register roles. */
    s32 step;
    s32 magnitude;

    if (difference == 0) {
        return;
    }
    magnitude = __builtin_abs(difference);
    step = 9;
    if (magnitude >= 300) {
        step = 19;
    }
    if (magnitude >= 1000) {
        step = 47;
    }
    if (magnitude >= 3000) {
        step = 97;
    }
    if (difference > 0) {
        difference -= step;
        if (difference < 0) {
            difference = 0;
        }
    } else {
        difference += step;
        if (difference > 0) {
            difference = 0;
        }
    }
    side->displayed_life_points =
        side->life_points.unsigned_value + difference;
}

/* Draws the paired digit-sprite readout for both players' gDuel_aSideStates slots.
   gDuel_bActiveSide selects which side (0 or 1) currently renders in the "active"
   grey shade (0x808080) vs the dim shade (0x404040); the other side always
   gets the opposite shade. Each side draws two digit groups (4-digit then
   2-digit) offset from the display object arg0->field_50 points at. The values
   are displayed life points and DECK_SIZE minus the signed deck_draw_cursor
   of the corresponding gDuel_aSideStates entry. */

#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))
#define SCRATCH ((DuelStatusDigitPacket *)0x1F800320)

void Duel_DrawLifePointsAndDeckCounts(DisplayObject *arg0)
{
    DisplayObject *pos;
    DuelStatusDigitPacket *scratch;
    u32 tmp10;

    Duel_UpdateLifePointDisplay(&gDuel_aSideStates[0]);
    Duel_UpdateLifePointDisplay(&gDuel_aSideStates[1]);
    pos = (DisplayObject *)arg0->field_50.word;

    scratch = SCRATCH;
    scratch->field_00 = 0x09000000;
    tmp10 = DUEL_SPRITE_CLUT_CXCY;
    scratch->field_10 = tmp10;
    scratch->field_0C = 0x1E;
    scratch->field_0E = 0x5800;
    scratch->field_08 = 0x80008;
    scratch->field_14 = DUEL_DISPLAY_COLOR_NORMAL;
    if (gDuel_bActiveSide == 0) {
        scratch->field_14 = DUEL_DISPLAY_COLOR_DIMMED;
    }

    scratch->field_04 = pos->field_30.h.field_30 - 3;
    scratch->field_06 = pos->field_30.h.field_32 - 0xD;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        gDuel_aSideStates[1].displayed_life_points,
        4
    );

    scratch->field_04 = pos->field_30.h.field_30 + 0xE;
    scratch->field_06 = pos->field_30.h.field_32 - 5;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        DECK_SIZE - gDuel_aSideStates[1].deck_draw_cursor,
        2
    );

    scratch->field_14 = DUEL_DISPLAY_COLOR_NORMAL;
    if (gDuel_bActiveSide != 0) {
        scratch->field_14 = DUEL_DISPLAY_COLOR_DIMMED;
    }

    scratch->field_04 = pos->field_30.h.field_30 - 3;
    scratch->field_06 = pos->field_30.h.field_32 + 0xD;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        gDuel_aSideStates[0].displayed_life_points,
        4
    );

    scratch->field_04 = pos->field_30.h.field_30 + 0xE;
    scratch->field_06 = pos->field_30.h.field_32 + 5;
    Duel_DrawDecimalDigits(
        pos,
        GS_SPRITE_VIEW(scratch),
        DECK_SIZE - gDuel_aSideStates[0].deck_draw_cursor,
        2
    );
}

/* The two per-card checks DuelScene_UpdateFieldActions makes on the grid card
   under the cursor: whether it can still act this turn, and whether Triangle
   opens the card viewer for it. */

int DuelCard_CanActThisTurn(DuelCardRecord *object)
{
    u16 flags = object->flags;

    if (flags & DUEL_CARD_FLAG_OCCUPIED) {
        if (!(flags & DUEL_CARD_FLAG_USED_THIS_TURN)) {
            return 1;
        }
    }
    return 0;
}

s32 Duel_GetCardViewerRequestId(DuelCardRecord *card)
{
#ifdef VERSION_JAPAN
    /* The Japanese build asks only that the card be occupied and carry no
       display marker (flags 0x8000 set, 0x2000 clear), read as the word that
       ends at flags, as Duel_UpdateCardDisplayMarker does. The US build instead reads the
       cursor's grid cell and rejects a face-down card when that cell's value
       is DUEL_FIELD_SIDE_GRID_SLOT_COUNT or more. */
    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_TRIANGLE) &&
        (*(s32 *)&card->terrain_modifier & 0xA0000000) == 0x80000000)
        return card->card_id;
    return 0;
#else
    DuelSelectionRecord *position = gDuel_pActiveSelectionRecord;
    s32 valid;
    u32 value =
        gDuel_abFieldGridCardRecordIndices[position->row * DUEL_FIELD_ROW_SIZE + position->col];

    if (value < DUEL_FIELD_SIDE_GRID_SLOT_COUNT)
        valid = 1;
    else {
        s32 mask = card->flags & DUEL_CARD_FLAG_FACE_DOWN;
        valid = (u32)mask < 1;
    }

    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_TRIANGLE) &&
        (card->flags & DUEL_CARD_FLAG_OCCUPIED) && valid)
        return card->card_id;
    return 0;
#endif
}

/* Effective attack and defense for one card, packed into a single word:
 * defense in the high half, attack in the low half. Both modifiers apply to
 * both stats, and each half is clamped to 0..CARD_STAT_MAX independently.
 *
 * Callers take one half or the other -- `& 0xFFFF` or a (u16) cast for the
 * attack, `>> 16` for the defense -- so the packing is the whole point of the
 * return value and it must stay a full word.
 */
s32 Duel_CalcCardStats(DuelCardRecord *card)
{
    s32 attack = card->attack + card->stat_modifier + card->terrain_modifier;
    s32 defense;
    if (attack < 0) attack = 0;
    if (attack > CARD_STAT_MAX) attack = CARD_STAT_MAX;
    defense = card->defense + card->stat_modifier + card->terrain_modifier;
    if (defense < 0) defense = 0;
    if (defense > CARD_STAT_MAX) defense = CARD_STAT_MAX;
    return (defense << 16) | attack;
}
