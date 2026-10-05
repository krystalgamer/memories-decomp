#ifndef FRENCH_MODEL_VARIANT116_ENTRY_H
#define FRENCH_MODEL_VARIANT116_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 ring_r, ring_g, ring_b;
    u8 dot_r, dot_g, dot_b;
    u8 sprite_r, sprite_g, sprite_b, field_09;
    s16 ring_height, ring_width, ring_radius;
    s16 dot_radius, sprite_size, sprite_radius;
    s16 ring_count, dot_count, sprite_count;
    s16 ring_duration, dot_duration, sprite_duration;
    s16 ring_stagger, initial_delay;
} Model116Config;

typedef struct {
    Model116Config *G32 config;
    SVECTOR ring[16];
    SVECTOR dots[128];
    SVECTOR sprites[64];
    s32 texture[3];
    u16 frame_toggle;
    u8 field_692[2];
    s32 elapsed;
} Model116State;

extern GsIMAGE D_8013C16C[];
extern Model116Config D_8013C1C0[];
s32 func_8013B004(u8 *context, s32 command);

#endif
