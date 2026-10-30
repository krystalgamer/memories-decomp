#ifndef MEMORIES_DECOMP_MODEL_VARIANT809_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT809_ENTRY_H

#include "../../types.h"
#include "../../game/screen_projection.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x1A-byte command row: the starting burst colour, the spark and burst
 * sizes, the spoke spread and length, the burst spawn period, the colour fade
 * step, and the delay, end and camera switch times. */
typedef struct {
    u8 red;
    u8 unknown_01;
    u8 green;
    u8 unknown_03;
    u8 blue;
    u8 unknown_05;
    u16 spark_size;
    u16 burst_size;
    u16 spread;
    u16 spoke_length;
    u16 period;
    u8 fade;
    u8 unknown_11;
    u16 unknown_12;
    u16 delay;
    u16 end;
    u16 camera_time;
} Model809Config;

/* The work area: the command row, the spark and burst positions, the spoke
 * tips of each burst, the spark ages, the elapsed time, the frame, the
 * ambient level, the spark count, the phase, the burst count, the textures
 * and the burst colours. */
typedef struct {
    Model809Config *G32 config;
    SVECTOR sparks[64];
    SVECTOR bursts[64];
    SVECTOR tips[64][16];
    u16 ages[64];
    u16 elapsed;
    u16 frame;
    s32 level;
    u16 spark_count;
    u16 phase;
    u16 burst_count;
    s32 texture[2];
    u8 red[64];
    u8 green[64];
    u8 blue[64];
} Model809State;

extern GsIMAGE D_8013C0D8[];
extern Model809Config D_8013C110[];

s32 func_8013B004(u8 *context, s32 command);

#endif
