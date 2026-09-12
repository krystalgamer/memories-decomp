#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "../unmatched.h"
#include "duel_grid.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "duel_side_state.h"
#include "duel_card.h"
#include "duel_effect.h"
#include "display_object_api.h"
#include "sound.h"
#include "duel_card_effects.h"
#include "duel_field_effect_steps.h"

/* The first of the two field-wide effect steps. It latches its first frame
 * through DuelEffect_MarkInitialized, requests the type-0x15 effect object and
 * parks it at the screen centre, then on the following pass sweeps the acting
 * side's second grid row and hands every record carrying both high modifier
 * bits to func_80025B28. */
void func_80025F3C(void)
{
    DuelFieldEffectObject *object;
    DuelFieldEffectObject *current;
    DuelCardRecord *entry;
    int slot;
    int phase;
    int side;

    if (DuelEffect_MarkInitialized() == 0) {
        object = (DuelFieldEffectObject *)func_8002C604(0x15);
        D_8009B17C = (u8 *)object;
        D_8009B1F0[D_8009B1D5 ^ 1] = (u8 *)object;
        object->x = 0xA0;
        object->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
        /* The side is read once into its own name: the two stores that
         * straddle it keep retail's order only while this read sits between
         * them. */
        side = D_8009B1D5;
        object->y = 0x78;
        object->field_1A = side ^ 1;
        SD_SEPlayFull(0x23);
        return;
    }

    phase = D_8009B220;

    if ((phase & 0x40) == 0) {
        if (((DuelFieldEffectObject *)D_8009B17C)->count != 0) {
            D_8009B220 = phase | 0x40;

            for (
                slot = DUEL_FIELD_ROW_SIZE;
                slot < DUEL_FIELD_SIDE_ZONE_COUNT;
                slot++
            ) {
                entry = &D_801A7AD8[D_800907D8[D_8009B1D5][slot]];
                if ((*(u32 *)&entry->terrain_modifier & 0x90000000)
                    == 0x90000000) {
                    current = (DuelFieldEffectObject *)entry->object;
                    current->callback = func_80025B28;
                    current->active = 1;
                }
            }
        }
        return;
    }

    if (func_80042B40(1) == 0) {
        if (((DuelFieldEffectObject *)D_8009B17C)->count >= 2) {
            D_800E9FF0[D_8009B1D5 ^ 1].field_19 = 4;
            D_8009B220 = 0;
        }
    }
}
