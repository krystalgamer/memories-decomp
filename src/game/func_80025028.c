#include "../types.h"
#include "duel_grid.h"
#include "duel_card.h"

extern u8 D_8009B1D5;
extern s16 D_8009B22A;

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what makes the
   store below a single %gp_rel instruction whose load-delay slot needs the
   retail nop. The address itself comes from c_symbols.ld, which overrides this
   common symbol, so no storage is allocated here. */
u8 D_8009B1B8;

/* Searches the acting side's five monster slots for an occupied card whose id
   equals the argument. A hit records the id in D_8009B22A, copies the owning
   object's +0x6A slot index to D_8009B1B8 and returns the id; a miss leaves
   D_8009B22A cleared and returns 0. */
s32 func_80025028(s32 arg0)
{
    s32 i;
    s32 base;
    DuelCardRecord *record;
    u8 *object;

    base = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    D_8009B22A = 0;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        record = &D_801A7AD8[D_800907D8[i + base]];
        if (record->flags & DUEL_CARD_FLAG_OCCUPIED) {
            if ((s16) record->card_id == arg0) {
                object = record->object;
                D_8009B22A = arg0;
                D_8009B1B8 = object[0x6A];
                return arg0;
            }
        }
    }
    return 0;
}
