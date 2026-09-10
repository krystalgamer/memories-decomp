#include "../types.h"
#include "display_object.h"
#include "duel_effect_update_object_layout.h"
#include "duel_effect.h"
#include "text_box_runtime.h"

/* Moves a text box and the objects it owns. The record is a
 * DuelEffectChannel, but its callers reach it by byte arithmetic on
 * D_800EB0F8 and pass a u8 *, so the parameter keeps that spelling and each
 * access casts it. A typed local would be tidier and costs the match: the
 * extra copy makes the prologue save s1 before s0. */
void TextBox_SetPos(u8 *record, s32 x, s32 y)
{
    DisplayObject *object;

    object = ((DuelEffectChannel *)record)->field_28;
    ((DuelEffectChannel *)record)->field_3C = x;
    ((DuelEffectChannel *)record)->field_40 = y;
    if (object != 0) {
        object->field_30.h.field_30 = x;
        object->field_30.h.field_32 = y;
    }
    object = ((DuelEffectChannel *)record)->field_2C;
    if (object != 0) {
        if (object->field_1E == 4)
            func_80039140((DuelEffectChannel *)record);
        else {
            object->field_30.h.field_30 = x;
            object->field_30.h.field_32 = y;
        }
    }
    object = ((DuelEffectChannel *)record)->field_30;
    if (object != 0) {
        if (object->field_1E == 4)
            DuelEffect_UpdateObjectLayout((DuelEffectChannel *)record);
        else {
            object->field_30.h.field_30 = ((DuelEffectChannel *)record)->field_3E + x - 16;
            object->field_30.h.field_32 = ((DuelEffectChannel *)record)->field_42 + y - 16;
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
