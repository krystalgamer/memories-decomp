#ifndef MEMORIES_DECOMP_MODEL_VARIANT766_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT766_ENTRY_H

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

/* One 0x30-byte command row: the burst, flash and trail colours, the two
 * trail parts, the shape sizes and counts, the spark gravity, and the timing. */
typedef struct {
    u8 burst_r, burst_g, burst_b;
    u8 flash_r, flash_g, flash_b;
    u8 trail_r, trail_g, trail_b;
    u8 first_part, second_part, unknown_0B;
    s16 star_size, column_height, speed, rotation_count, spark_count, column_width;
    s16 spark_size, square_size, unknown_1C, unknown_1E;
    s32 flash_spin;
    s16 gravity, initial_delay, burst_delay, trail_duration, unknown_2C, unknown_2E;
} Model766Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model766Color;

/* The work area: the command row, the two trail edges, the column rotations,
 * the sparks, the flash square, the colours, and the timers. */
typedef struct {
    Model766Config *G32 config;
    SVECTOR trail_first[32];
    SVECTOR trail_second[32];
    SVECTOR rotations[32];
    SVECTOR positions[32];
    SVECTOR velocities[32];
    SVECTOR square[4];
    Model766Color trail_color, burst_color, flash_color;
    u32 burst_scale, flash_scale;
    u16 trail_count, unknown_53A;
    s32 texture[2];
    u16 burst_tpage, burst_clut;
    s32 level;
    s16 phase;
    u16 elapsed;
    u16 unknown_550;
} Model766State;

extern GsIMAGE D_8013C848[];
extern Model766Config D_8013C880[];

s32 func_8013B004(u8 *context, s32 command);

#endif
