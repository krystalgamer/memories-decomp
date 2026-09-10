#include "../types.h"
#include "func_80016778.h"
#include "display_object_api.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_icon_setup.h"
#include "duel_deck_card.h"
#include "duel_grid.h"

#define DUEL_CARD_ICON_REPLAY_BASE_OFFSET 0x48000

extern u8 D_8015C424[];
extern DuelFieldPosition D_800908A0[];

/* Allocates a display object, positions it, wires up its per-frame callback,
   and selects a small icon variant for non-monster card types. */
DuelCardDisplayObject *func_80024C1C(s32 cardId, s32 x, s32 y) {
    DuelCardDisplayObject *obj;
    u32 desc;
    s32 type;
    s16 val;

    obj = func_800400AC(func_8004002C(), 0);

    obj->out_y = -0x18;
    obj->out_x = x;
    obj->field_34 = y;
    obj->field_67 = 0;
    obj->field_69 = 0;
    obj->attribute = obj->attribute | 0x1000000;

    desc = gDuel_adwCardStats[cardId - 1];
    obj->field_10 = (void *)func_80016778;
    obj->icon_state = 0;
    obj->field_5D = 0xC0;

    type = (s32)desc >> CARD_STAT_TYPE_SHIFT;
    type &= CARD_STAT_TYPE_MASK;
    obj->field_68 = (u8)type;
    obj->icon_variant = 0;

    if (type < CARD_TYPE_MAGIC) {
        goto end;
    }
    obj->icon_state = 0x38;

    switch (type) {
        case CARD_TYPE_EQUIP:
            obj->icon_variant = 1;
            goto end;
        case CARD_TYPE_MAGIC:
            val = 1;
            break;
        case CARD_TYPE_TRAP:
            val = 2;
            break;
        case CARD_TYPE_RITUAL:
            val = 3;
            break;
        default:
            goto end;
    }
    obj->icon_variant = val;

end:
    return obj;
}

void func_80024D34(s32 a, s32 b)
{
    u8 *slot;
    s32 idx;
    u8 *tb;
    DuelCardReplayRecordBlock *replay;
    DuelCardDisplayObject *obj;

    slot = Duel_SetupCardRecord(a, b);
    idx = a;
    if ((idx & 0x80) != 0) {
        idx = (idx & 0x7F) + DUEL_CARD_SIDE_RECORD_COUNT;
    }
    tb = D_8015C424;
    replay = (DuelCardReplayRecordBlock *)(
        tb + idx * sizeof(DuelCardRecord) + DUEL_CARD_ICON_REPLAY_BASE_OFFSET
    );
    obj = func_80024C1C(*(s16 *)replay->record.data, D_800908A0[idx].x,
                        D_800908A0[idx].y);
    *(DuelCardDisplayObject **)slot = obj;
    obj->card_index = idx;
}
