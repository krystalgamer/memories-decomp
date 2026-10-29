#ifndef MEMORIES_DECOMP_MODEL_VARIANT844_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT844_ENTRY_H

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

/* One 0x2C-byte command row: the trail and star colours, the four slot parts
 * the two trails follow, the trail layers, the particle counts, speed and
 * sizes, the timing of both trails and both bursts, and the time at which
 * the ray fan appears. */
typedef struct {
    u8 trail_r, trail_g, trail_b;
    u8 star_r, star_g, star_b;
    u8 first_part, second_part, third_part, fourth_part;
    s16 layer_count, layer_step;
    s16 spark_max, flare_max;
    s16 speed;
    s16 spark_size;
    s16 flare_width, flare_height;
    s16 star_size;
    s16 first_start, first_end;
    s16 second_start, second_end;
    s16 burst_start, second_burst, end;
    u16 ray_start;
} Model844Config;

/* The work area: the command row, the four trail edges (two per trail, used
 * as rings of 64), the spark and flare directions, the random ray rotations,
 * the timers, the frame of every particle in both bursts, the ray length,
 * the star scales, the textures, the colours of both trails and of the stars
 * of both bursts, and the ray colour. */
typedef struct {
    Model844Config *G32 config;
    SVECTOR trail[4][64];
    SVECTOR sparks[16];
    SVECTOR flares[16];
    SVECTOR rays[32];
    s16 elapsed;
    s16 phase;
    s16 drawing[2];
    s16 head[2];
    s16 spark_frames[2][16];
    s16 flare_frames[2][16];
    s16 spark_counts[2];
    s16 flare_counts[2];
    u16 ray_scale;
    s32 level;
    s32 star_scale[2][16];
    u16 burst_tpage, burst_clut;
    s32 texture[3];
    CVECTOR colors[4][64];
    CVECTOR ray_color;
} Model844State;

extern GsIMAGE D_8013D5D8[];
extern Model844Config D_8013D62C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
