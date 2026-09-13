#include "../types.h"
#include "text_encode_decimal_digits.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "duel_side_state.h"
#include "../ygo_types.h"
#include "duel_draw_status_numbers.h"
#include "card_constants.h"
#include "duel_display.h"
#include "duel_grid.h"
#include "ordering_tables.h"

/* The duel screen's life-point and deck-count readout: func_80016D2C sorts
   one row of digit sprites, Duel_UpdateLifePointDisplay steps the drawn
   life-point counter a frame towards the real total, and
   Duel_DrawLifePointsAndDeckCounts calls both for each side. The first two
   were recorded at gcc_2_8_1_g0_split and gcc_2_8_1_g8 and compile to
   identical objects at this unit's gcc_2_8_1_g8_split. */

void func_80016D2C(
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
            D_800E9D90[style->ot_index],
            style->field_14
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

/* Draws the paired digit-sprite readout for both players' D_800E9FF0 slots.
   D_8009B1D5 selects which side (0 or 1) currently renders in the "active"
   grey shade (0x808080) vs the dim shade (0x404040); the other side always
   gets the opposite shade. Each side draws two digit groups (4-digit then
   2-digit) offset from the display object arg0->field_50 points at. The values
   are displayed life points and DECK_SIZE minus the signed deck_draw_cursor
   of the corresponding D_800E9FF0 entry. */

#define SCRATCH ((DuelStatusDigitPacket *)0x1F800320)

void Duel_DrawLifePointsAndDeckCounts(DisplayObject *arg0)
{
    DisplayObject *pos;
    DuelStatusDigitPacket *scratch;
    u32 tmp10;

    Duel_UpdateLifePointDisplay(&D_800E9FF0[0]);
    Duel_UpdateLifePointDisplay(&D_800E9FF0[1]);
    pos = (DisplayObject *)arg0->field_50.word;

    scratch = SCRATCH;
    scratch->field_00 = 0x09000000;
    tmp10 = 0xF10100;
    scratch->field_10 = tmp10;
    scratch->field_0C = 0x1E;
    scratch->field_0E = 0x5800;
    scratch->field_08 = 0x80008;
    scratch->field_14 = DUEL_DISPLAY_COLOR_NORMAL;
    if (D_8009B1D5 == 0) {
        scratch->field_14 = DUEL_DISPLAY_COLOR_DIMMED;
    }

    scratch->field_04 = pos->field_30.h.field_30 - 3;
    scratch->field_06 = pos->field_30.h.field_32 - 0xD;
    func_80016D2C(
        pos,
        (GsSPRITE *)scratch,
        D_800E9FF0[1].displayed_life_points,
        4
    );

    scratch->field_04 = pos->field_30.h.field_30 + 0xE;
    scratch->field_06 = pos->field_30.h.field_32 - 5;
    func_80016D2C(
        pos,
        (GsSPRITE *)scratch,
        DECK_SIZE - D_800E9FF0[1].deck_draw_cursor,
        2
    );

    scratch->field_14 = DUEL_DISPLAY_COLOR_NORMAL;
    if (D_8009B1D5 != 0) {
        scratch->field_14 = DUEL_DISPLAY_COLOR_DIMMED;
    }

    scratch->field_04 = pos->field_30.h.field_30 - 3;
    scratch->field_06 = pos->field_30.h.field_32 + 0xD;
    func_80016D2C(
        pos,
        (GsSPRITE *)scratch,
        D_800E9FF0[0].displayed_life_points,
        4
    );

    scratch->field_04 = pos->field_30.h.field_30 + 0xE;
    scratch->field_06 = pos->field_30.h.field_32 + 5;
    func_80016D2C(
        pos,
        (GsSPRITE *)scratch,
        DECK_SIZE - D_800E9FF0[0].deck_draw_cursor,
        2
    );
}
