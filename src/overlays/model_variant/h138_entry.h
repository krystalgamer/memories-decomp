#ifndef MEMORIES_DECOMP_MODEL_VARIANT138_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT138_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* The 0xDAC-byte work area: the command row, the 13x13 wave grid and its
 * colours, the ring's 9x17 colours, the ring's lower and upper edges, 34
 * orbiting sprites, 32 rising sprites, the orbiting sprites' frames, the
 * spin angle, three texture words, the completion flag and the elapsed
 * time. */
typedef struct {
    ModelVariantWaveConfig *config;
    SVECTOR grid[169];
    CVECTOR colours[169];
    CVECTOR ring[153];
    SVECTOR lower[17];
    SVECTOR upper[17];
    SVECTOR orbit[34];
    SVECTOR rising[32];
    u8 frames[34];
    s16 spin;
    s32 texture[3];
    u8 completed, unknown_DA5[3];
    s32 elapsed;
} Model138State;

extern char gModelVariant138_szWorkSizeFormat[];
extern GsIMAGE gModelVariant138_aImages[];
extern ModelVariantWaveConfig gModelVariant138_aConfigs[];

s32 ModelVariant138_HandleRequest(Model138State *context, s32 command);

#endif
