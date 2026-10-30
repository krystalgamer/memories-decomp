#ifndef MEMORIES_DECOMP_MODEL_VARIANT788_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT788_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x42-byte command row: four colour triples (quad, inner ring, outer
 * ring and band pairs), the camera part, the ring radius and quad half size,
 * the line count and its shell radii and duration, the beam sizes, the band
 * spin, count and widths, the growth rate, the dot count, spread and
 * duration, the box half size, the fade steps and the phase times. */
typedef struct {
    u8 colors[4][3];
    u8 part, unknown_0D;
    u16 radius;
    u16 half_size;
    s16 line_count;
    s16 inner_radius;
    s16 outer_radius;
    s16 line_duration;
    u16 beam_width;
    u16 beam_half;
    u16 beam_length;
    s16 spin;
    s16 band_count;
    s16 unknown_24;
    s16 band_width;
    s16 band_top;
    s16 growth;
    s16 dot_duration;
    s16 dot_count;
    s16 dot_spread;
    u16 box_half;
    u8 fade_in, unknown_35;
    u8 fade_out, unknown_37;
    u16 beam_shrink;
    u16 unknown_3A;
    u16 start;
    u16 fade_end;
    u16 finish;
} Model788Config;

/* The work area: the command row, the line ends and their velocities, the
 * band rotations, the 16-point ring, the dots and their velocities, the
 * opposing slot position, the band sizes, the elapsed time and phase, the
 * line count, the beam sizes, the ambient level, the texture word and the
 * fading colours. */
typedef struct {
    Model788Config *G32 config;
    SVECTOR inner[64];
    SVECTOR outer[64];
    SVECTOR inner_velocity[64];
    SVECTOR outer_velocity[64];
    SVECTOR spins[16];
    SVECTOR ring[16];
    SVECTOR dots[128];
    SVECTOR dot_velocity[128];
    SVECTOR position;
    s16 band_height;
    s16 band_depth;
    s16 band_bottom;
    s16 band_top;
    u16 radius, unknown_1116;
    u32 elapsed;
    u16 phase;
    u16 count;
    s16 beam_length;
    s16 beam_width;
    s16 beam_half, unknown_1126;
    s32 ambient;
    s32 texture[1];
    u8 quad[3];
    u8 ring_colors[3][2];
    u8 band_colors[3][2];
    u8 flash;
    u8 box[3], unknown_1143;
} Model788State;

extern GsIMAGE D_8013D3E0[];
extern Model788Config D_8013D3FC[];

s32 func_8013B004(u8 *context, s32 command);

#endif
