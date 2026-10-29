#ifndef MEMORIES_DECOMP_MODEL_VARIANT815_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT815_ENTRY_H

#include "../../types.h"
#include "../../game/screen_projection.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x32-byte command row: the spark colour, the shard size and spread,
 * the glow width and height, the three ring radii and the ring height, the
 * spark size and speed, the fade step, the timing and the three camera
 * orbit phases. */
typedef struct {
    u8 r, g, b, pad_3;
    u16 shard_size;
    u16 spread;
    u16 glow_width;
    u16 glow_height;
    u16 top_radius;
    u16 middle_radius;
    u16 base_radius;
    u16 ring_height;
    u16 unknown_14[4];
    u16 spark_size;
    u16 spark_speed;
    u16 unknown_20;
    u8 fade, pad_23;
    u16 unknown_24;
    u16 start;
    u16 near_time;
    u16 far_time;
    s16 near_zoom;
    s16 far_zoom;
    s16 end_zoom;
} Model815Config;

/* An RGB colour. */
typedef struct {
    u8 r, g, b;
} Model815Rgb;

/* The work area: the command row, the effect position, the shards and the
 * three-ring cone, the embers and the sparks with their velocities, the
 * elapsed time, the frame, the glow scale, the shard ages, the phase, the
 * shard count, the ambient level, the textures, the glow colour and the
 * spark colour. */
typedef struct {
    Model815Config *G32 config;
    SVECTOR position;
    SVECTOR shards[48];
    SVECTOR ring[16][3];
    SVECTOR embers[64];
    SVECTOR ember_velocities[64];
    SVECTOR sparks[64];
    SVECTOR spark_velocities[64];
    u32 elapsed;
    u32 frame;
    u32 scale;
    u16 ages[48];
    u16 phase;
    u16 count;
    s32 level;
    s32 texture[5];
    /* The glow and spark colours, also tested as the word at the glow (whose
     * top byte is the spark red) and the spark green/blue halfword. */
    union {
        struct {
            Model815Rgb glow;
            Model815Rgb color;
        } c;
        struct {
            u32 glow;
            u16 color_gb;
        } w;
    } tint;
} Model815State;

extern GsIMAGE D_8013CFB0[5];
extern Model815Config D_8013D03C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
