#include "../types.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "display_object_helpers.h"
#include "duel_effect_create_channel.h"
#include "dialog_layout.h"

/* Regional value: the channel flags it sets (0x1008 -> 0x1010). The European
 * build (src/game/european/) defines its own. */
#ifndef DUEL_EFFECT_CHANNEL_CREATE_FLAGS
#define DUEL_EFFECT_CHANNEL_CREATE_FLAGS 0x1008
#endif

#ifndef DIALOG_CHOICE_ADDRESS
#define DIALOG_CHOICE_ADDRESS 0x8009B34D
#endif

#ifndef DUEL_EFFECT_CHANNEL_HEIGHT
#define DUEL_EFFECT_CHANNEL_HEIGHT 0x40
#endif

#define gDialog_bChoice (*(s8 *)DIALOG_CHOICE_ADDRESS)

DuelEffectChannel *DuelEffect_CreateChannel(s32 value, s32 set_flags) {
    DuelEffectChannel *channel;

    /* A symbolic store changes the retail assembler-temporary address form. */
    gDialog_bChoice = -1;
    channel = TextBox_Create(
        D_800EF6EA,
        value & 0x7FFF,
        DIALOG_BOX_X,
        0x50,
        DIALOG_BOX_WIDTH,
        DUEL_EFFECT_CHANNEL_HEIGHT
    );
    channel->field_59 = *(u8 *)&D_8009AF74[1] - 1;

    if (set_flags != 0) {
        channel->flags_34 |= DUEL_EFFECT_CHANNEL_CREATE_FLAGS;
    } else if (value & 0x8000) {
        func_80039A14(channel);
    }

    return channel;
}
