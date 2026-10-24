#ifndef MEMORIES_DECOMP_MODEL_VARIANT828_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT828_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x3C-byte command row: the burst colour, the ray count and shape, the
 * spark count, size and speed, the glow size, the debris and ember sizes,
 * speeds and counts, the colour fade step, the delay, and the timing and
 * offsets of the camera presets. */
typedef struct {
    u8 r, g, b, pad_3;
    u16 ray_count;
    s16 ray_width, ray_base, ray_height;
    u16 spark_max;
    s16 spark_size;
    u16 spark_speed;
    u16 glow_size;
    s16 debris_size;
    u16 debris_speed;
    u16 debris_count;
    u16 ember_count;
    s16 ember_size;
    u16 ember_speed;
    s16 fade;
    s16 pad_22;
    u16 delay;
    u16 camera_times[2];
    s16 camera_args[9];
} Model828Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model828Color;

/* The work area: the command row, the ray rotations, the spark directions, the
 * debris and embers and their velocities, the elapsed time, the phase, the
 * spark frames, the debris frame, the spark count, the camera sequence, the
 * ambient level, the ray scale, the textures and the burst colour. */
typedef struct {
    Model828Config *G32 config;
    SVECTOR rays[64];
    SVECTOR sparks[16];
    SVECTOR debris[8];
    SVECTOR debris_velocities[8];
    SVECTOR embers[32];
    SVECTOR ember_velocities[32];
    u32 elapsed;
    u16 phase;
    u16 frames[16];
    u16 debris_frame;
    u16 spark_count;
    u16 camera;
    s32 level;
    u32 ray_scale;
    s32 texture[4];
    Model828Color color;
} Model828State;

extern GsIMAGE D_8013CA48[];
extern Model828Config D_8013CAB8[];

s32 func_8013B004(u8 *context, s32 command);

#endif
