#include "../types.h"
#include "duel_effect_update_object_layout.h"
#include "duel_effect.h"
#include "text_box_runtime.h"

void TextBox_SetPos(u8 *record, s32 x, s32 y)
{
    u8 *object;

    object = *(u8 **)(record + 40);
    *(s16 *)(record + 60) = x;
    *(s16 *)(record + 64) = y;
    if (object != (u8 *)0) {
        *(s16 *)(object + 48) = x;
        *(s16 *)(object + 50) = y;
    }
    object = *(u8 **)(record + 44);
    if (object != (u8 *)0) {
        if (*(s16 *)(object + 30) == 4)
            func_80039140((DuelEffectChannel *)record);
        else {
            *(s16 *)(object + 48) = x;
            *(s16 *)(object + 50) = y;
        }
    }
    object = *(u8 **)(record + 48);
    if (object != (u8 *)0) {
        if (*(s16 *)(object + 30) == 4)
            DuelEffect_UpdateObjectLayout((DuelEffectChannel *)record);
        else {
            *(s16 *)(object + 48) = *(u16 *)(record + 62) + x - 16;
            *(s16 *)(object + 50) = *(u16 *)(record + 66) + y - 16;
        }
    }
}

void func_80039A14(u8 *object)
{
    ((DuelEffectChannel *)object)->flags_34 |= TEXT_BOX_FLAG_BUILD_REQUESTED;
    do {
        TextBox_BuildStep((DuelEffectChannel *)object);
    } while (!(((DuelEffectChannel *)object)->flags_34 &
               TEXT_BOX_FLAG_DONE));
}

void func_80039A60(u8 *object)
{
    ((DuelEffectChannel *)object)->flags_34 |= 0xA00;
    do {
        TextBox_BuildStep((DuelEffectChannel *)object);
    } while (!(((DuelEffectChannel *)object)->flags_34 &
               TEXT_BOX_FLAG_DONE));
}
