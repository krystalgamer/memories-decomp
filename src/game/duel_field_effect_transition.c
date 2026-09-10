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
#include "duel_effect.h"
#include "sound.h"
#include "duel_apply_card_object_flags.h"
#include "duel_field_effect_transition.h"
#include "view_state.h"
#include "func_80022D94.h"
#include "func_80025D30.h"
#include "../unmatched.h"

#define DUEL_FIELD_EFFECT_TIMER_STEP 8
#define DUEL_FIELD_EFFECT_MARK_THRESHOLD 40
#define DUEL_FIELD_EFFECT_TIMER_LIMIT 64

extern u8 D_800907D8_flat[] asm("D_800907D8");

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

/* Companion field-wide stat-penalty sweep. It advances one occupied slot of
 * the acting side's second row per countdown, spawns the effect at that card,
 * and waits on the same request-completion state as the transition above. */
void func_80025D30(void) {
    DuelCardRecord *record;
    DuelEffectObject *object;
    u8 *card;
    s32 timer;
    s32 base_slot;

    if (DuelEffect_MarkInitialized() == 0) {
        D_8009B20C[1] = 0;
        D_8009B1D0 = 0;
    }

    if ((D_8009B220 & 0x40) != 0) {
        if ((D_8009B260 & 1) == 0) {
            D_8009B220 = 0;
        }
        return;
    }

    timer = *(u16 *)&D_8009B20C[1] - 1;
    D_8009B20C[1] = timer;
    if ((s16)timer > 0) {
        return;
    }
    D_8009B20C[1] = 0x10;

    base_slot = D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                DUEL_FIELD_ROW_SIZE;
    record = &D_801A7AD8[D_800907D8_flat[(s16)D_8009B1D0 + base_slot]];
    if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
        card = (u8 *)record->object;
        object = (DuelEffectObject *)func_8002C604(0xD);
        object->x = *(u16 *)(card + 0x30);
        object->y = *(u16 *)(card + 0x32);
        object->field_04 = *(u16 *)(card + 0x34);
        object->field_14 = object->field_14 + ((s16)D_8009B1D0 << 14);
        if (D_8009B1D2 == DUEL_SPELLBINDING_CIRCLE_CARD_ID) {
            object->field_1A = 2;
            record->stat_modifier =
                record->stat_modifier - DUEL_STAT_PENALTY_PER_LEVEL;
            object->field_12 = -DUEL_STAT_PENALTY_PER_LEVEL;
        } else {
            object->field_1A = 1;
            record->stat_modifier =
                record->stat_modifier - 2 * DUEL_STAT_PENALTY_PER_LEVEL;
            object->field_12 = -2 * DUEL_STAT_PENALTY_PER_LEVEL;
        }
        SD_SEPlayFull(0x21);
    }

    D_8009B1D0 = D_8009B1D0 + 1;
    if ((s16)D_8009B1D0 >= DUEL_FIELD_ROW_SIZE) {
        D_8009B220 = D_8009B220 | 0x40;
    }
}
