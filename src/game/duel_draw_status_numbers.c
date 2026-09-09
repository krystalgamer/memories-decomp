#include "../types.h"
#include "../ygo_types.h"
#include "duel_draw_status_numbers.h"
#include "duel_update_life_point_display.h"
#include "func_80016D2C.h"
#include "duel_side_state.h"
#include "card_constants.h"
#include "duel_display.h"
#include "duel_grid.h"

/* Draws the paired digit-sprite readout for both players' D_800E9FF0 slots.
   D_8009B1D5 selects which side (0 or 1) currently renders in the "active"
   grey shade (0x808080) vs the dim shade (0x404040); the other side always
   gets the opposite shade. Each side draws two digit groups (4-digit then
   2-digit) offset from the shared position struct at arg0->unk50. The values
   are displayed LP from field12 and DECK_SIZE minus the signed draw cursor
   in field18 of the corresponding D_800E9FF0 entry. */

#define SCRATCH ((DuelStatusDigitPacket *)0x1F800320)

void Duel_DrawLifePointsAndDeckCounts(DuelStatusWidget *arg0) {
    DuelStatusPosition *pos;
    DuelStatusDigitPacket *scratch;
    register u32 tmp10 asm("v1");

    Duel_UpdateLifePointDisplay(&D_800E9FF0[0]);
    Duel_UpdateLifePointDisplay(&D_800E9FF0[1]);
    pos = arg0->field_50;

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

    scratch->field_04 = pos->field_30 - 3;
    scratch->field_06 = pos->field_32 - 0xD;
    func_80016D2C(
        pos,
        (GsSPRITE *)scratch,
        D_800E9FF0[1].displayed_life_points,
        4
    );

    scratch->field_04 = pos->field_30 + 0xE;
    scratch->field_06 = pos->field_32 - 5;
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

    scratch->field_04 = pos->field_30 - 3;
    scratch->field_06 = pos->field_32 + 0xD;
    func_80016D2C(
        pos,
        (GsSPRITE *)scratch,
        D_800E9FF0[0].displayed_life_points,
        4
    );

    scratch->field_04 = pos->field_30 + 0xE;
    scratch->field_06 = pos->field_32 + 5;
    func_80016D2C(
        pos,
        (GsSPRITE *)scratch,
        DECK_SIZE - D_800E9FF0[0].field_18,
        2
    );
}
