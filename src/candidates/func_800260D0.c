/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_field_effect_steps.c.
 */
/* Two steps of the same field-wide duel effect machinery. Both latch their
 * first frame through DuelEffect_MarkInitialized, both drive their phase
 * through D_8009B220, and both walk the acting side's grid D_800907D8 by
 * D_8009B1D5 into the card records at D_801A7AD8. */
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
#include "../game/display_object_api.h"
#include "../game/sound.h"
#include "../game/duel_card_effects.h"
#include "../game/duel_field_effect_steps.h"

/* One step of a field-wide effect sweep, driven once per 16 frames by the
 * D_8009B1D0 countdown. The first entry resets the step index in
 * D_8009B20C[1]; every later entry advances it and spawns a type-8 effect
 * object over the next slot of the third grid row (slots 10..14 of the acting
 * side), offsetting the object's depth by the step so the objects stagger.
 * Steps past the fifth clear D_8009B220 and end the sweep. A card sitting in
 * the swept slot with a negative stat modifier has it cleared and gets the
 * alternate object state 5. */
void func_800260D0(void) {
    DuelCardRecord *record;
    DuelEffectObject *object;
    u8 *position;
    u8 *positions;
    s32 card;
    s32 next;
    s32 base_slot;
    s32 x;
    s32 timer;
    s32 step;
    register u8 *grid __asm__("$4");

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
           below, while func_80025F3C above uses the two-dimensional view the
           unit's DUEL_FIELD_GRID_2D declaration gives. The cast is the one
           place the two spellings meet. */
        grid = (u8 *)D_800907D8;
        base_slot = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                    DUEL_FIELD_SIDE_ZONE_COUNT;
        card = grid[(s16)next + base_slot];
        record = &D_801A7AD8[card];
        object = (DuelEffectObject *)func_8002C604(8);
        positions = (u8 *)D_80090800;
        step = D_8009B20C[1];
        object->field_1A = 3;
        position = (
            (step + DUEL_FIELD_SIDE_ZONE_COUNT) * sizeof(DuelFieldPosition) +
            D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES
        ) + positions;
        x = *(u16 *)(position + 0);
        object->y = 0;
        object->x = x;
        object->field_04 = *(u16 *)(position + 2);
        object->field_14 = object->field_14 + step * 0x3000;
        SD_SEPlayFull(0x14);

        if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            if (record->stat_modifier < 0) {
                record->stat_modifier = 0;
                object->field_1A = 5;
            }
        }
    } else {
        D_8009B220 = 0;
    }
}
