#define D_8009B1D5_IS_VOLATILE
#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "../unmatched.h"
#include "../game/duel_grid.h"
#include "../game/duel_effect_request.h"
#include "../game/duel_action_lock.h"
#include "../game/duel_side_state.h"
#include "../game/duel_card.h"
#include "../game/duel_effect.h"
#include "../game/sound.h"
#include "../game/duel_card_effects.h"
#include "../game/duel_field_effect_steps.h"

/* One step of a field-wide effect sweep, driven once per 16 frames by the
 * D_8009B1D0 countdown. The first entry resets the step index in
 * D_8009B20C[1]; every later entry advances it and spawns a type-8 effect
 * object over the next slot of the third grid row (slots 10..14 of the acting
 * side), offsetting the object's depth by the step so the objects stagger.
 * Steps past the fifth clear gDuel_wCardEffectFlags and end the sweep. A card sitting in
 * the swept slot with a negative stat modifier has it cleared and gets the
 * alternate object state 5. */
void DuelEffect_ApplyCursebreaker(void) {
    DuelCardRecord *record;
    DuelEffectObject *object;
    DuelFieldPosition *position;
    u8 *positions;
    s32 card;
    s32 next;
    s32 base_slot;
    s32 x;
    s32 timer;
    s32 step;
    s32 position_index;
    u8 *grid;

    if (DuelEffect_MarkInitialized() == 0) {
        D_8009B20C[1] = -1;
        D_8009B1D0 = 0;
    }

    timer = D_8009B1D0 - 1;
    D_8009B1D0 = timer;
    if ((s16)timer > 0) {
        return;
    }

    D_8009B1D0 = 0x10;
    next = *(u16 *)&D_8009B20C[1] + 1;
    D_8009B20C[1] = next;
    if ((s16)next < DUEL_FIELD_ROW_SIZE) {
        /* This step walks the grid flat, with the side folded into base_slot
           below, while sibling DuelEffect_ApplySwords uses the two-dimensional view
           selected by DUEL_FIELD_GRID_2D. The cast is the one
           place the two spellings meet. The single-pass loop preserves the
           retail grid and side register allocation under GCC 2.8.1. The
           selected record uses DuelFieldPosition, with u16 member views to
           retain retail's unsigned halfword loads. */
        do {
            grid = (u8 *)D_800907D8;
            base_slot = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                        DUEL_FIELD_SIDE_ZONE_COUNT;
        } while ((s16)next == 0 && (s16)next != 0);
        card = grid[(s16)next + base_slot];
        record = &D_801A7AD8[card];
        object = (DuelEffectObject *)DuelEffect_AllocateRequest(8);
        positions = (u8 *)D_80090800;
        step = D_8009B20C[1];
        /* Keep the row bias in the typed index for retail scheduling. */
        position_index = step + DUEL_FIELD_SIDE_ZONE_COUNT;
        object->field_1A = 3;
        position = (DuelFieldPosition *)(
            (u32)&((DuelFieldPosition *)0)[position_index] +
            D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES +
            positions
        );
        x = *(u16 *)&position->x;
        object->y = 0;
        object->x = x;
        object->field_04 = *(u16 *)&position->y;
        object->field_14 = object->field_14 + step * 0x3000;
        SD_SEPlayFull(0x14);

        if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            if (record->stat_modifier < 0) {
                record->stat_modifier = 0;
                object->field_1A = 5;
            }
        }
    } else {
        gDuel_wCardEffectFlags = 0;
    }
}
