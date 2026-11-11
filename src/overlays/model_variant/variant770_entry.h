#ifndef MEMORIES_DECOMP_MODEL_VARIANT770_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT770_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x38-byte command row. The colours, sizes and counts are shared by the
 * two streams; each stream has its own trail parts, flash spin and timing. */
typedef struct {
    u8 burst_r, burst_g, burst_b;
    u8 flash_r, flash_g, flash_b;
    u8 trail_r, trail_g, trail_b;
    u8 first_part[2], second_part[2], unknown_0D;
    s16 star_size, column_height, speed, rotation_count, spark_count, column_width;
    s16 spark_size, square_size, unknown_1E;
    s32 flash_spin[2];
    s16 gravity;
    s16 trail_start[2], burst_delay0;
    s16 trail_end[2], burst_delay1;
    s16 unknown_36;
} Model770Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model770Color;

/* The work area: the command row and, per stream, the two trail edges, the
 * column rotations, the sparks, the colours, scales and trail length. */
typedef struct {
    Model770Config *config;
    SVECTOR trail_first[64][2];
    SVECTOR trail_second[64][2];
    SVECTOR rotations[64][2];
    SVECTOR positions[32][2];
    SVECTOR velocities[32][2];
    SVECTOR square[4];
    Model770Color trail_color[2], burst_color[2], flash_color[2];
    u32 burst_scale[2], flash_scale[2];
    u16 trail_count[2];
    s32 texture[2];
    u16 burst_tpage, burst_clut;
    s32 level;
    s16 phase;
    s16 stream_phase[2];
    u16 elapsed;
    u16 unknown_1068;
} Model770State;

extern GsIMAGE gModelVariant770_aImages[];
extern Model770Config gModelVariant770_aConfigs[];

s32 ModelVariant770_HandleRequest(u8 *context, s32 command);

#endif
