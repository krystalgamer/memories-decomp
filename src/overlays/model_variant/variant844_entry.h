#ifndef MEMORIES_DECOMP_MODEL_VARIANT844_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT844_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x2C-byte command row: the trail and burst colours, the four model
 * parts the two trails follow, the trail segment count and spacing, the
 * spark and flare counts, the ring radius, the spark and flare sprite
 * sizes, the burst size, the two trail windows, the two burst start
 * times, the end time and the beam start time. */
typedef struct {
    u8 trail_r, trail_g, trail_b;
    u8 burst_r, burst_g, burst_b;
    u8 parts[4];
    s16 segments;
    s16 segment_step;
    s16 spark_count;
    s16 flare_count;
    s16 radius;
    s16 spark_size;
    s16 flare_width;
    s16 flare_height;
    s16 burst_size;
    s16 trail_start, trail_end;
    s16 trail2_start, trail2_end;
    s16 burst_start;
    s16 burst2_start;
    s16 end;
    u16 beam_start;
} Model844Config;

/* The work area: the two pairs of trail rings sampled from the model
 * parts, the ring of spark and flare offsets, the beam spins, the timer,
 * the phase, the trail switches and ring indices, the spark and flare
 * frames and counts for both bursts, the beam height, the ambient level,
 * the burst scales, the beam texture page and palette, the textures, the
 * fading trail and burst colours, and the fading beam colour. */
typedef struct {
    Model844Config *G32 config;
    SVECTOR trail_a[64];
    SVECTOR trail_b[64];
    SVECTOR trail_c[64];
    SVECTOR trail_d[64];
    SVECTOR sparks[16];
    SVECTOR flares[16];
    SVECTOR spins[32];
    s16 elapsed;
    s16 phase;
    s16 trail_on[2];
    s16 trail_index[2];
    s16 spark_frames[2][16];
    s16 flare_frames[2][16];
    s16 spark_count[2];
    s16 flare_count[2];
    u16 beam_height;
    s32 level;
    s32 burst_scale[2][16];
    u16 tpage, clut;
    s32 texture[3];
    u8 trail_colors[2][64][4];
    u8 burst_colors[2][64][4];
    union {
        u8 rgb[4];
        u32 word;
    } beam_color;
} Model844State;

extern GsIMAGE D_8013D5D8[3];
extern Model844Config D_8013D62C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
