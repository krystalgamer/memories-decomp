#define DUEL_FIELD_GRID_2D
#define D_8009B260_IN_DATA
#include "../types.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "func_8002C604.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "duel_card.h"
#include "display_object_api.h"
#include "sound.h"
#include "duel_apply_card_object_flags.h"
#include "duel_field_effect_transition.h"

#define DUEL_FIELD_EFFECT_TIMER_STEP 8
#define DUEL_FIELD_EFFECT_MARK_THRESHOLD 40
#define DUEL_FIELD_EFFECT_TIMER_LIMIT 64

void func_80025B28(DuelFieldEffectObject *o)
{
    o->timer += DUEL_FIELD_EFFECT_TIMER_STEP;
    if (!(o->active & 0x80) &&
        o->timer >= DUEL_FIELD_EFFECT_MARK_THRESHOLD) {
        o->active |= 0x80;
        D_801A7AD8[o->index].flags &= ~DUEL_CARD_FLAG_DISPLAY_MARKER;
        o->mark = 0;
    }
    if (o->timer < DUEL_FIELD_EFFECT_TIMER_LIMIT) {
        o->timer = 0;
        o->active = 0;
        o->callback = 0;
        D_801A7AD8[o->index].flags &= ~0x3400;
        Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)o);
    }
}

/* Opens the duel-side effect prompt and, once acknowledged, hands every
   occupied slot of the current side over to the func_80025B28 animation. */
void func_80025BEC(void)
{
    DuelFieldEffectObject *object;
    DuelFieldEffectObject *target;
    DuelCardRecord *record;
    u16 flags;
    s32 i;

    if (DuelEffect_MarkInitialized() == 0) {
        object = (DuelFieldEffectObject *)func_8002C604(0x13);
        object->x = 0xA0;
        D_8009B17C = (u8 *)object;
        object->y = 0x68;
        SD_SEPlayFull(0x13);
        return;
    }
    flags = D_8009B220;
    if ((flags & 0x40) == 0 &&
        ((DuelFieldEffectObject *)D_8009B17C)->count != 0) {
        D_8009B220 = flags | 0x40;
        SD_SEPlayFull(0x1D);
        for (i = DUEL_FIELD_ROW_SIZE; i < DUEL_CARD_SIDE_RECORD_COUNT; i++) {
            record = &D_801A7AD8[D_800907D8[D_8009B1D5][i]];
            /* The retail code tests the two halfwords at +0x14 as one
               word; 0x90000000 selects bits 0x9000 of flags at +0x16. */
            if ((*(u32 *)&record->terrain_modifier & 0x90000000) ==
                0x90000000) {
                target = (DuelFieldEffectObject *)record->object;
                target->callback = func_80025B28;
                target->active = 1;
            }
        }
    }
    if ((D_8009B260 & 1) == 0 && func_80042B40(1) == 0) {
        D_8009B220 = 0;
    }
}
