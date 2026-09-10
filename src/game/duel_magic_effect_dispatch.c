#include "../types.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "../psyq/stdio.h"

#include "card_constants.h"
#include "duel_action_lock.h"
#include "duel_effect_request.h"
#include "duel_card.h"
#include "duel_card_object_cleanup.h"
#include "duel_effect_tables.h"

#include "duel_magic_effect_format.h"
#include "duel_magic_effect_dispatch.h"

void func_80026A3C(void)
{
    int i;

    if (!DuelEffect_MarkInitialized()) {
        DuelEffectRequest *object = func_8002C68C(0x17);
        int side = D_8009B1D5 ^ 1;

        object->field_04 = D_80090800[side][0].y;
        printf(D_8009AF40, (short)object->field_04);
    } else {
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            int position =
                i + D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
            DuelCardRecord *entry =
                &D_801A7AD8[D_800907D8[position]];

            if (entry->flags & DUEL_CARD_FLAG_OCCUPIED)
                func_80024954(entry);
        }
        D_8009B220 = 0;
    }
}

int func_80026B34(void)
{
    u16 flags = D_8009B220;

    if (flags & DUEL_CARD_EFFECT_FLAG_ACTIVE) {
        u8 *indices = gDuelEffect_abGroupByEffectId;
        DuelEffectHandler *callbacks;
        int index = indices[D_8009B1A8] * DUEL_CARD_EFFECT_HANDLERS_PER_GROUP;

        if (flags & DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER)
            index++;
        callbacks = gDuelEffect_apfnGroupHandler;
        callbacks[index]();
    }
    return D_8009B220;
}

void func_80026BA4(int value, int flag)
{
    int index;

    if (((unsigned)(value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID) <
         DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        ((unsigned)(value - DUEL_EFFECT_SECOND_BLOCK_CARD_ID) <
         DUEL_EFFECT_CARD_BLOCK_SIZE) ||
        value == DUEL_DARK_MAGIC_RITUAL_CARD_ID) {
        if (value < DUEL_EFFECT_SECOND_BLOCK_CARD_ID) {
            index = value - DUEL_EFFECT_FIRST_BLOCK_CARD_ID;
        } else {
            index = value -
                (DUEL_EFFECT_SECOND_BLOCK_CARD_ID -
                 DUEL_EFFECT_CARD_BLOCK_SIZE);
            if (value == DUEL_DARK_MAGIC_RITUAL_CARD_ID) {
                index = DUEL_DARK_MAGIC_RITUAL_EFFECT_INDEX;
            }
        }
        D_8009B1A8 = index;
        D_8009B1D2 = value;
        D_8009B220 = DUEL_CARD_EFFECT_FLAG_ACTIVE;
        if (flag) {
            D_8009B220 =
                DUEL_CARD_EFFECT_FLAG_ACTIVE |
                DUEL_CARD_EFFECT_FLAG_SECOND_HANDLER;
        }
    }
}
