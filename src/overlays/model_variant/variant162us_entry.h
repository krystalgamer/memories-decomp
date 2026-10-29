#ifndef MEMORIES_DECOMP_MODEL_VARIANT162US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT162US_ENTRY_H

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
 * spark colours, the tube's inner and outer radii and height, the swirl and
 * beam ring radius, the spark scatter, the beam half height, the beam rise,
 * the spark half size, the burst radius, the fade-in, hold and fade-out
 * durations, the spark lifetime, the spin and swirl rates, the ring spacing,
 * the spark spacing and the start delay. */
typedef struct {
    u8 tube_color[3], beam_color[3], spark_color[3], unknown_9;
    s16 inner_radius, outer_radius, height, swirl_radius, scatter, beam_size, rise, spark_size;
    s16 burst_radius, fade_in, hold, fade_out, spark_time, spin, swirl, spacing;
    s16 spark_spacing, delay;
} Model162UsConfig;

/* The 0x978-byte work area: the command row, a 9x17 tube mesh, an
 * eight-point swirl trail, the swirl centre, a 33-point burst, 96 spark
 * positions, an eight-point beam ring, the spin angle, the swirl phase, three
 * texture words, the completion flag and the elapsed time. */
typedef struct {
    Model162UsConfig *G32 config;
    SVECTOR mesh[153];
    SVECTOR trail[1][8];
    SVECTOR centre;
    SVECTOR burst[33];
    SVECTOR sparks[96];
    SVECTOR ring[8];
    s32 angle;
    s32 phase[1];
    u32 texture[3];
    u8 completed, unknown_971[3];
    s32 elapsed;
} Model162UsState;

extern GsIMAGE D_8013CB18[];
extern Model162UsConfig D_8013CBC0[];

s32 func_8013B004(u8 *context, s32 command);

#endif
