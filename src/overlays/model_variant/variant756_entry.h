#ifndef MEMORIES_DECOMP_MODEL_VARIANT756_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT756_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x34-byte command row: the burst and trail colours, the two trail
 * parts, the sizes, counts and speeds of every particle set, the spark
 * gravity, and the timing. */
typedef struct {
    u8 burst_r, burst_g, burst_b;
    u8 trail_r, trail_g, trail_b;
    u8 first_part, second_part;
    s16 star_size, column_height, speed, rotation_count, spark_count, column_width;
    s16 spark_size;
    u16 debris_count, debris_speed;
    s16 debris_size;
    u16 smoke_max;
    s16 smoke_rate;
    u16 smoke_speed;
    s16 smoke_size;
    u16 ember_count;
    s16 ember_size;
    u16 ember_speed;
    s16 gravity;
    s16 initial_delay, burst_delay, trail_duration, unknown_32;
} Model756Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model756Color;

/* The work area: the command row, the two trail edges, the column rotations,
 * the sparks, debris, smoke and embers, their frames, the colours, and the
 * timers. */
typedef struct {
    Model756Config *G32 config;
    SVECTOR trail_first[32];
    SVECTOR trail_second[32];
    SVECTOR rotations[32];
    SVECTOR positions[32];
    SVECTOR velocities[32];
    SVECTOR debris[16];
    SVECTOR debris_velocities[16];
    SVECTOR smoke[64];
    SVECTOR embers[64];
    SVECTOR ember_velocities[64];
    Model756Color trail_color, burst_color;
    u32 unknown_C0C;
    u32 burst_scale;
    u16 trail_count, debris_frame;
    u16 frames[64];
    u16 smoke_count, unknown_C9A;
    s32 texture[3];
    u16 burst_tpage, burst_clut;
    s32 level;
    s16 phase;
    u16 elapsed;
    u16 unknown_CB4;
} Model756State;

extern GsIMAGE D_8013D010[];
extern Model756Config D_8013D064[];

s32 func_8013B004(u8 *context, s32 command);

#endif
