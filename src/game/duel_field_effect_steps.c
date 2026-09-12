/* Field-wide duel effect step that marks qualifying opposing-side cards. */
#define D_8009B1D5_IS_VOLATILE
#define DUEL_FIELD_GRID_2D
#include "../types.h"
#include "../game/func_8002C604.h"
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

void func_80025F3C(void)
{
    DuelFieldEffectObject *object;
    DuelCardRecord *entry;
    int slot;
    int side;
    int field_side;

    if (DuelEffect_MarkInitialized() == 0) {
        object = (DuelFieldEffectObject *)func_8002C604(0x15);
        side = D_8009B1D5 ^ 1;
        D_8009B17C = (u8 *)object;
        D_8009B1F0[side] = (u8 *)object;
        object->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;
        object->x = 0xA0;
        object->y = 0x78;
        field_side = D_8009B1D5 ^ 1;
        object->field_1A = field_side;
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
                if ((*(u32 *)&entry->terrain_modifier & 0x90000000) ==
                    0x90000000) {
                    DuelFieldEffectObject *current;

                    current = (DuelFieldEffectObject *)entry->object;
                    current->callback = func_80025B28;
                    current->active = 1;
                }
            }
        }
    } else if (func_80042B40(1) == 0 &&
               ((DuelFieldEffectObject *)D_8009B17C)->count >= 2) {
        D_800E9FF0[D_8009B1D5 ^ 1].field_19 = 4;
        D_8009B220 = 0;
    }
}
