#ifndef MEMORIES_DECOMP_MODEL_VARIANT139_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT139_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x2E-byte command row, picked by command % 100: the tube, beam and
 * spark colours, the tube's inner and outer radii and height, the swirl
 * radius, the sway, the beam half height, the beam rise, the spark half size,
 * the burst radius, the fade-in, hold and fade-out durations, the spark
 * lifetime, the spin and swirl rates, the ring spacing, the spark spacing and
 * the start delay. */
typedef struct {
    u8 tube_color[3], beam_color[3], spark_color[3], unknown_9;
    s16 inner_radius, outer_radius, height, swirl_radius, sway, beam_size, rise, spark_size;
    s16 burst_radius, fade_in, hold, fade_out, spark_time, spin, swirl, spacing;
    s16 spark_spacing, delay;
} Model139Config;

/* The 0x97C-byte work area: the command row, a 9x17 tube mesh, two eight-point
 * swirl trails, the swirl centre, a 33-point burst, 96 spark positions, the
 * spin angle, the two swirl phases, three texture words, the completion flag
 * and the elapsed time. */
typedef struct {
    Model139Config *G32 config;
    SVECTOR mesh[153];
    /* Two eight-point trails; kept flat so each row start stays in bounds. */
    SVECTOR trail[16];
    SVECTOR centre;
    SVECTOR burst[33];
    SVECTOR sparks[96];
    s32 angle;
    s32 phase[2];
    u32 texture[3];
    u8 completed, unknown_975[3];
    s32 elapsed;
} Model139State;

extern GsIMAGE D_8013CACC[];
extern GsIMAGE D_8013CB04[];
extern Model139Config D_8013CB74[];

s32 func_8013B004(u8 *context, s32 command);

#endif
