#ifndef MEMORIES_DECOMP_MODEL_VARIANT93_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT93_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_effect_requests.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../game/screen_projection.h"

/* One 0x58-byte command row: six fade colours, the two model parts, the
 * shapes of the dome, star, wave, flash and sparks, the phase durations,
 * the phase start times and the seven camera step times. */
typedef struct {
    u8 dome_red, dome_green, dome_blue;
    u8 bolt_red, bolt_green, bolt_blue;
    u8 star_red, star_green, star_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 wave_red, wave_green, wave_blue;
    u8 flash_red, flash_green, flash_blue;
    u8 part, bolt_part;
    s16 radius;
    s16 depth;
    s16 unknown_18;
    s16 star_short, star_long;
    s16 wave_length, wave_radius, wave_width, wave_height, wave_shift;
    s16 flash_size, spark_size, spark_spread;
    s16 dome_duration, bolt_duration, fade_in_duration, flash_duration;
    s16 fade_out_duration, wave_period;
    u16 delay;
    s16 bolt_start[3];
    s16 fade_start, flash_start, spark_start, wave_start;
    s16 camera_time[7];
} Model93Config;

/* The work area: the command row, the dome mesh, the three bolt end-point
 * pairs, the star, the wave strip, five groups of sixteen sparks, the
 * texture word, the elapsed time and the camera step. */
typedef struct {
    Model93Config *G32 config;
    SVECTOR dome[137];
    SVECTOR bolts[6];
    SVECTOR star[17];
    SVECTOR wave[18];
    union {
        SVECTOR group[5][16];
        SVECTOR flat[80];
    } sparks;
    u32 texture[1];
    s32 elapsed;
    u8 camera_step;
} Model93State;

extern GsIMAGE D_8013E438[];
extern Model93Config D_8013E454[];

s32 func_8013B004(u8 *context, s32 command);

#endif
