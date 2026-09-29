#ifndef MEMORIES_DECOMP_DUEL_EFFECT_IMAGE_INPUTS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_IMAGE_INPUTS_H

#include "../../types.h"
#include "../../psyq/libgs.h"

/* Mode 20 overlaps the low halfword of the first variant image's pmode. */
typedef union {
    u16 modes[21];
    struct {
        u16 leading_modes[20];
        GsIMAGE images[5];
    } variant;
} DuelEffectImageInputs;

typedef char DuelEffectImageInputs_size_must_be_0xB4[
    sizeof(DuelEffectImageInputs) == 0xB4 ? 1 : -1
];
typedef char DuelEffectImageInputs_images_offset_must_be_0x28[
    (u32)&((DuelEffectImageInputs *)0)->variant.images == 0x28 ? 1 : -1
];

extern DuelEffectImageInputs D_8015A430;

#endif
