#ifndef FRENCH_MODEL_VARIANT121_ENTRY_H
#define FRENCH_MODEL_VARIANT121_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 red, green, blue, part, count, field_05;
    s16 sprite_size, spread, ring_radius, ring_depth;
    s16 travel_duration, fade_duration, particle_stagger;
    s16 initial_delay, ring_duration;
} Model121Config;

typedef union {
    CVECTOR colors[26];
    struct {
        CVECTOR initialized_colors[24];
        u32 texture[2];
    } storage;
} Model121Palette;

typedef struct {
    Model121Config *config;
    SVECTOR positions[64];
    SVECTOR destinations[64];
    SVECTOR ring[26];
    /* The final triangle reads its second RGB from the first texture handle. */
    Model121Palette palette;
    u8 completed;
    u8 field_53D[3];
    s32 elapsed;
} Model121State;

extern GsIMAGE gFrenchModelVariant121_aImages[];
extern Model121Config gFrenchModelVariant121_aConfigs[];
s32 FrenchModelVariant121_HandleRequest(u8 *context, s32 command);

#endif
