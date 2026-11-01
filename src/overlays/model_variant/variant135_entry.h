#ifndef MEMORIES_DECOMP_MODEL_VARIANT135_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT135_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x2C-byte command row, picked by command % 100: the spark colour and
 * the beam brightness, the beam colour, the three slot parts, the spark and
 * beam half sizes, the spark spread, the beam cross height and spread, the
 * grid spacing, the target spread, the spark and beam spacing, the spark rise
 * and fade, the beam duration, the wave duration and its first stage, and the
 * beam and spark start times. */
typedef struct {
    u8 red, green, blue, brightness;
    u8 unknown_04[2];
    u8 wave_red, wave_green, wave_blue;
    u8 part, wave_part, wave_end_part;
    s16 spark_size, spread, beam_size, cross_height, cross_spread, grid, target_spread;
    s16 spacing, wave_spacing, rise, fade, beam, wave, wave_stage, wave_delay, delay;
} Model135Config;

/* The 0x70C-byte work area: the command row, 48 sparks and their targets,
 * the four beam crosses, 48 wave heads, the six slot anchors, a 3x3 grid and
 * 48 beam targets, the four grid quads, ten textures, the completion flag and
 * the elapsed time. */
typedef struct {
    Model135Config *G32 config;
    SVECTOR sparks[48];
    SVECTOR targets[48];
    SVECTOR cross[4];
    SVECTOR waves[48];
    SVECTOR anchors[6];
    SVECTOR grid[9];
    SVECTOR beams[48];
    SVECTOR *G32 quads[4][4];
    u32 texture[10];
    u8 completed, unknown_705[3];
    s32 elapsed;
} Model135State;

extern GsIMAGE gModelVariant135_D16A0[];
extern Model135Config gModelVariant135_D17B8[];

s32 ModelVariant135_HandleRequest(u8 *context, s32 command);

#endif
