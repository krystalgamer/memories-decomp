#ifndef MEMORIES_DECOMP_MODEL_VARIANT782_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT782_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_update_coordinate_unit_matrix.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x3C-byte command row: the flare size, the rise speed, the ring
 * radii and spreads, the growth of the dome, the debris and spark spreads,
 * the sprite sizes and the timing of every phase. */
typedef struct {
    s32 size;
    s16 rise;
    s16 ring_radius;
    s16 ring_depth;
    s16 unknown_0A;
    s32 grow;
    s32 grow_late;
    s16 cone_height;
    s16 unknown_16;
    s32 dome_grow;
    s16 base_radius;
    s16 debris_count;
    s16 debris_spread;
    s16 debris_speed;
    s16 debris_size;
    s16 debris_rise;
    s16 wave_height;
    s16 wave_step;
    s16 spark_rise;
    s16 spark_size;
    s16 spark_fall;
    s16 start;
    s16 dome_start;
    s16 camera;
    s16 wave_start;
    s16 end;
} Model782Config;

/* The work area: the command row, the effect origin, the four 16-point
 * rings, the debris positions and velocities, the sparks with their fall
 * speeds and frames, the timer, the phase, the debris frame, the wave
 * radius and height, the two dome scales, the ambient level, the three
 * textures and the four fade levels. */
typedef struct {
    Model782Config *G32 config;
    SVECTOR origin;
    SVECTOR outer[16];
    SVECTOR inner[16];
    SVECTOR cone[16];
    SVECTOR base[16];
    SVECTOR debris[16];
    SVECTOR debris_velocity[16];
    SVECTOR sparks[16];
    s16 spark_fall[16];
    s16 spark_frame[16];
    u32 elapsed;
    u16 phase;
    u16 debris_frame;
    s32 wave_radius;
    s32 wave_height;
    s32 dome_scale;
    s32 base_scale;
    s32 ambient;
    s32 texture[3];
    u8 flare_level;
    u8 dome_level;
    u8 wave_level;
    u8 spark_level;
} Model782State;

extern GsIMAGE gModelVariant782_D226C[];
extern Model782Config gModelVariant782_D22C0[];

s32 ModelVariant782_HandleRequest(u8 *context, s32 command);

#endif
