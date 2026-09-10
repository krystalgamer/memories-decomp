/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_800300C8.c.
 */
#include "../types.h"
#include "../game/display_object.h"
#include "../game/func_800300C8.h"
#include "../game/duel_effect.h"
#include "../game/frontend_debug_tables.h"
#include "../game/frontend_debug_state.h"

void func_800300C8(void)
{
    struct LayoutSource *source = (struct LayoutSource *)&D_800EB15C;
    s32 first = source->base;
    DisplayObject *target = D_8009B2E4;
    s32 index = D_8009B2F1;
    s32 second;
    s32 third;

    if (index >= 10) {
        register s32 raw_spacing asm("$2") =
            (s32)((u32)(u16)source->spacing << 16);
        register s32 half_spacing asm("$3") = raw_spacing >> 16;

        raw_spacing = (s32)((u32)raw_spacing >> 31);
        half_spacing += raw_spacing;
        half_spacing >>= 1;
        first -= 16;
        first += half_spacing;
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
