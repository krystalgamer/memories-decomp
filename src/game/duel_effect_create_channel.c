#include "../types.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "display_object_helpers.h"
#include "duel_effect_create_channel.h"

extern u8 D_800EF6EA;

#define gDialog_bChoice (*(s8 *)0x8009B34D)

DuelEffectChannel *DuelEffect_CreateChannel(s32 value, s32 set_flags) {
    DuelEffectChannel *channel;

    /* A symbolic store changes the retail assembler-temporary address form. */
    gDialog_bChoice = -1;
    channel = TextBox_Create(
        D_800EF6EA,
        value & 0x7FFF,
        0x20,
        0x50,
        0x100,
        0x40
    );
    channel->field_59 = *(u8 *)&D_8009AF74[1] - 1;

    if (set_flags != 0) {
        channel->flags_34 |= 0x1008;
    } else if (value & 0x8000) {
        func_80039A14((u8 *)channel);
    }

    return channel;
}
