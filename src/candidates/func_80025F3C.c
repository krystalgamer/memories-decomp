/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/duel_field_effect_steps.c.
 */
/* Applies Swords of Revealing Light to the opposing side. The handler creates
 * its persistent effect object, stages the occupied field objects, then
 * installs the three-turn attack lock once the presentation completes. */
#define D_8009B1D5_IS_VOLATILE
#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "../game/func_8002C604.h"
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
#include "../game/duel_swords_effect.h"

void DuelEffect_ApplySwords(void)
{
    DuelFieldEffectObject *object;
    DuelCardRecord *entry;
    int slot;
    register int side __asm__("$3");
    if (DuelEffect_MarkInitialized() == 0) {
        object = (DuelFieldEffectObject *)func_8002C604(0x15);
        side = D_8009B1D5 ^ 1;
        D_8009B17C = (u8 *)object;
        gDuel_apSwordsEffectObjects[side] = object;
        object->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
        object->x = 0xA0;
        object->y = 0x78;
        side = D_8009B1D5 ^ 1;
        object->field_1A = side;
        SD_SEPlayFull(0x23);
    } else if (!(D_8009B220 & 0x40)) {
        if (((DuelFieldEffectObject *)D_8009B17C)->count != 0) {
            D_8009B220 |= 0x40;
            for (
                slot = DUEL_FIELD_ROW_SIZE;
                slot < DUEL_FIELD_SIDE_ZONE_COUNT;
                slot++
            ) {
                entry = &D_801A7AD8[D_800907D8[D_8009B1D5][slot]];
                if ((*(u32 *)&entry->terrain_modifier & 0x90000000) == 0x90000000) {
                    register DuelFieldEffectObject *current __asm__("$2");
                    current = (DuelFieldEffectObject *)entry->object;
                    current->callback = DuelEffect_UpdateRevealCard;
                    current->active = 1;
                }
            }
        }
    } else if (func_80042B40(1) == 0 &&
               ((DuelFieldEffectObject *)D_8009B17C)->count >= 2) {
        D_800E9FF0[D_8009B1D5 ^ 1].swords_turns_remaining =
            DUEL_SWORDS_INITIAL_COUNTER;
        D_8009B220 = 0;
    }
}
