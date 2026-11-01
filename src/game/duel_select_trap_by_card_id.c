#include "../types.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "duel_card.h"
#include "display_object.h"
#include "duel_select_trap_by_card_id.h"
#include "duel_field_card_render_data.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what makes the
   store below a single %gp_rel instruction whose load-delay slot needs the
   retail nop. The address itself comes from c_symbols.ld, which overrides this
   common symbol, so no storage is allocated here. */
u8 gDuel_bSelectedTrapSlot;

/* Searches the acting side's five mapped field slots for an occupied trap
   whose id equals the argument. A hit records the id in gDuel_wSelectedTrapCardId, copies
   the owning object's field_6A slot index to gDuel_bSelectedTrapSlot and returns the id; a
   miss leaves gDuel_wSelectedTrapCardId cleared and returns 0. */
s32 Duel_SelectTrapByCardId(s32 arg0)
{
    s32 i;
    s32 base;
    DuelCardRecord *record;
    DisplayObject *object;

    base = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    gDuel_wSelectedTrapCardId = 0;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        record = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[i + base]];
        if (record->flags & DUEL_CARD_FLAG_OCCUPIED) {
            if ((s16) record->card_id == arg0) {
                object = (DisplayObject *)record->object;
                gDuel_wSelectedTrapCardId = arg0;
                gDuel_bSelectedTrapSlot = object->field_6A;
                return arg0;
            }
        }
    }
    return 0;
}
