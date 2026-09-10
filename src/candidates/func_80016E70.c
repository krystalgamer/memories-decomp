/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_draw_status_numbers.c.
 */
#include "../types.h"
#include "../game/text_encode_decimal_digits.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../game/duel_side_state.h"
#include "../ygo_types.h"
#include "../game/duel_draw_status_numbers.h"
#include "../game/card_constants.h"
#include "../game/duel_display.h"
#include "../game/duel_grid.h"
#include "../game/ordering_tables.h"

/* The duel screen's life-point and deck-count readout: func_80016D2C sorts
   one row of digit sprites, Duel_UpdateLifePointDisplay steps the drawn
   life-point counter a frame towards the real total, and
   Duel_DrawLifePointsAndDeckCounts calls both for each side. The first two
   were recorded at gcc_2_8_1_g0_split and gcc_2_8_1_g8 and compile to
   identical objects at this unit's gcc_2_8_1_g8_split. */

/* Draws the paired digit-sprite readout for both players' D_800E9FF0 slots.
   D_8009B1D5 selects which side (0 or 1) currently renders in the "active"
   grey shade (0x808080) vs the dim shade (0x404040); the other side always
   gets the opposite shade. Each side draws two digit groups (4-digit then
   2-digit) offset from the display object arg0->field_50 points at. The values
   are displayed LP from field12 and DECK_SIZE minus the signed draw cursor
   in field18 of the corresponding D_800E9FF0 entry. */

#define SCRATCH ((DuelStatusDigitPacket *)0x1F800320)

void Duel_DrawLifePointsAndDeckCounts(DisplayObject *arg0) {
    DisplayObject *pos;
    DuelStatusDigitPacket *scratch;
    register u32 tmp10 asm("v1");

    Duel_UpdateLifePointDisplay(&D_800E9FF0[0]);
    Duel_UpdateLifePointDisplay(&D_800E9FF0[1]);
    pos = (DisplayObject *)arg0->field_50.word;

    scratch = SCRATCH;
    tmp10 = 0xF10100;
    scratch->field_10 = tmp10;
    scratch->field_00 = 0x09000000;
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
        DECK_SIZE - D_800E9FF0[1].field_18,
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
        DECK_SIZE - D_800E9FF0[0].field_18,
        2
    );
}
