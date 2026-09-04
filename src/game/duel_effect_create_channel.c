#include "../types.h"
#include "duel_effect.h"

extern u8 D_800EF6EA;
extern u16 D_8009AF76;
extern DuelEffectChannel *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void func_80039A14(DuelEffectChannel *);

#define D_8009B34D (*(s8 *)0x8009B34D)

DuelEffectChannel *DuelEffect_CreateChannel(s32 value, s32 set_flags) {
    DuelEffectChannel *channel;

    /* A symbolic store changes the retail assembler-temporary address form. */
    D_8009B34D = -1;
    channel = TextBox_Create(
        D_800EF6EA,
        value & 0x7FFF,
        0x20,
        0x50,
        0x100,
        0x40
    );
    channel->field_59 = (u8)D_8009AF76 - 1;

    if (set_flags != 0) {
        channel->flags_34 |= 0x1008;
    } else if (value & 0x8000) {
        func_80039A14(channel);
    }

    return channel;
}
