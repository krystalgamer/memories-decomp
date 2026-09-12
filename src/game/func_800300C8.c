#include "../types.h"
#include "display_object.h"
#include "func_800300C8.h"
#include "duel_effect.h"
#include "frontend_debug_tables.h"
#include "frontend_debug_state.h"

void func_800300C8(void)
{
    struct LayoutSource *source = (struct LayoutSource *)&D_800EB15C;
    s32 first = source->base;
    DisplayObject *target = D_8009B2E4;
    s32 index = D_8009B2F1;
    s32 second;
    s32 third;

    if (index >= 10) {
        first += source->spacing / 2 - 16;
    }
    target->field_38.h.field_38 = first;
    target->position.h.field_28 = first;

    second = first + source->spacing / 2 - 16;
    target->field_40.h.field_40 = second;
    target->field_30.h.field_30 = second;

    {
        s32 quotient = index / 10;
        s8 remainder = (s8)(index - quotient * 10);
        third = source->row + remainder * 16;
    }
    target->field_30.h.field_32 = third;
    target->position.h.field_2A = third;
    third += 16;
    target->field_40.h.field_42 = third;
    target->field_38.h.field_3A = third;
}
