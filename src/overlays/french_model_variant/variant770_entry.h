#ifndef FRENCH_MODEL_VARIANT770_ENTRY_H
#define FRENCH_MODEL_VARIANT770_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 trail_r, trail_g, trail_b;
    u8 sprite_r, sprite_g, sprite_b;
    u8 first_part, second_part;
    s16 trail_layers, sprite_count, dot_count, trail_layer_step;
    s16 sprite_spread, dot_radius, sprite_size;
    s16 initial_delay, trail_duration, burst_delay, sprite_delay;
} Model770Config;

typedef union {
    CVECTOR rgb;
    u32 word;
} Model770Color;

typedef struct {
    Model770Config *G32 config;
    SVECTOR trail_first[32];
    SVECTOR trail_second[32];
    SVECTOR quad[4];
    SVECTOR sprite_offsets[32];
    SVECTOR sprite_origins[32];
    u8 field_424[0x100];
    SVECTOR positions[64];
    SVECTOR velocities[64];
    SVECTOR inner_ring[16];
    SVECTOR outer_ring[16];
    DVECTOR screen[64];
    s32 trail_count;
    s32 level;
    u32 scale;
    u16 sprite_frames[32];
    u16 depths[64];
    u16 active_sprites, previous_sprites, elapsed, phase;
    u16 burst_tpage, burst_clut;
    s32 texture[1];
    Model770Color trail_color, burst_color;
} Model770State;

extern GsIMAGE D_8013C6F8[];
extern Model770Config D_8013C714[];
s32 func_8013B004(u8 *context, s32 command);

#endif
