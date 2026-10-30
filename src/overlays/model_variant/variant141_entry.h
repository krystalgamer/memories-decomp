#ifndef MEMORIES_DECOMP_MODEL_VARIANT141_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT141_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x56-byte command row, picked by command % 100: the background fade,
 * tube, spark, burst, beam and flare colours, the slot parts the tube,
 * sparks and beam use, the wave depth, the ring, tube and spark sizes, the
 * spark spread, the burst size and rise, the beam length and width, the
 * flare size, the spiral radii, the ribbon width, the cylinder height and
 * radius, the fade, tube, spark, burst, beam, bolt, flare, flash and ribbon
 * durations, the ribbon length, the tube, spark and beam spacing and the
 * fade-in, fade-out, effect and ribbon start times. */
typedef struct {
    u8 fade_color[3], tube_color[3], spark_color[3], burst_color[3];
    u8 beam_color[3], flare_color[3];
    u8 tube_part, spark_part, beam_from, beam_to;
    s16 wave, ring_radius, tube_radius, spark_size, spark_spread;
    s16 burst_size, burst_rise, beam_length, beam_width, flare_size;
    s16 spiral_radius, spiral_wobble, ribbon_width, cylinder_height, cylinder_radius;
    s16 fade, unknown_36, tube_time, spark_time, burst_time, beam_time;
    s16 bolt_time, flare_time, flash_time, ribbon_time, ribbon_length;
    u8 tube_spacing, spark_spacing, beam_spacing, unknown_4d;
    s16 fade_in, fade_out, start, ribbon_start;
} Model141Config;

/* The 0xC7C-byte work area: the command row, the tube ring, the tube
 * targets and positions, the spark targets and positions, the beam
 * rotations, a 3x3 flare mesh, a 17-point spiral, a 34-point cylinder, the
 * flare's four quads (four corner pointers each), the background wave
 * offsets and steps, the wave scroll, eight texture words, the completion
 * flag, the elapsed time and the frame count. */
typedef struct {
    Model141Config *G32 config;
    SVECTOR ring[17];
    SVECTOR tube_targets[28];
    SVECTOR tube[28];
    SVECTOR spark_targets[72];
    SVECTOR sparks[72];
    SVECTOR beams[48];
    SVECTOR flare[9];
    SVECTOR spiral[17];
    SVECTOR cylinder[34];
    SVECTOR *G32 flare_quads[16];
    u8 wave[GRAPHICS_DEFAULT_HEIGHT];
    u8 wave_step[GRAPHICS_DEFAULT_HEIGHT];
    u8 wave_offset, scroll, unknown_c4e[2];
    u32 texture[8];
    u8 completed, unknown_c71[3];
    s32 elapsed;
    s32 frame;
} Model141State;

/* Stack scratch shared by the ring's middle point and the tube's projected
 * screen points. */
typedef union {
    SVECTOR middle;
    DVECTOR screen[34];
} Model141Scratch;

extern GsIMAGE D_8013EC00[];
extern Model141Config D_8013ECE0[];

s32 func_8013B004(u8 *context, s32 command);

#endif
