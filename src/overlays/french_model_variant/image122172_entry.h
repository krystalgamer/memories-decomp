#ifndef FRENCH_MODEL_IMAGE122172_ENTRY_H
#define FRENCH_MODEL_IMAGE122172_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../game/model_slot_data.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 trail_r, trail_g, trail_b;
    u8 sprite_r, sprite_g, sprite_b;
    u8 dot_r, dot_g, dot_b;
    u8 parts[4];
    u8 trail_count;
    u8 history_length;
    u8 event_count;
    s16 half_size;
    s16 radius;
    s16 trail_duration;
    s16 flash_duration;
    s16 dot_duration;
    s16 trail_delay;
    s16 event_times[8];
} Model142Config;

typedef struct {
    Model142Config *config;
    SVECTOR history[96];
    SVECTOR dots[32];
    s32 texture[1];
    s32 field_408;
    s32 elapsed;
    u8 event;
} Model142State;

extern const VECTOR gFrenchModelVariant142_UnitScale;
extern GsIMAGE gFrenchModelVariant142_aImages[];
extern Model142Config gFrenchModelVariant142_aConfigs[];
s32 FrenchModelVariant142_HandleRequest(u8 *context, s32 command);

#endif
