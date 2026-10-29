#ifndef MEMORIES_DECOMP_MODEL_VARIANT817_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT817_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/graphics_constants.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x34-byte command row: the bolt colour and the model point it leaves
 * from, the bolt sprite size, the bolt path jitter, the strand spread and
 * half width, the burst and smoke sprite sizes and spreads, the bolt length,
 * the spawn period, the scale of the bolt tip, the glow and spark sizes, the
 * spark spreads, the two fade steps, the spark, spawn and end times and the
 * two camera switch times. */
typedef struct {
    u8 r, g, b;
    u8 part;
    u16 bolt_size;
    u16 jitter;
    u16 strand_spread;
    u16 strand_width;
    u16 burst_size;
    u16 burst_spread;
    u16 smoke_size;
    u16 smoke_spread;
    u16 count;
    u16 period;
    s32 tip_scale;
    u16 glow_size;
    u16 spark_size;
    u16 spark_reach;
    u16 spark_spread;
    u8 fade;
    u8 unknown_25;
    u16 unknown_26;
    u8 tint_fade;
    u8 unknown_29;
    u16 spark_end;
    u16 spawn_end;
    u16 end;
    u16 camera_start;
    u16 camera_switch;
} Model817Config;

/* The work area: the command row; for each of the three live bolts its
 * points, the strand offsets around every point, the burst sprites and
 * their velocities and the smoke sprites and their velocities; eighty-eight
 * five-segment sparks; the timers, the bolt head, the phase, the per-bolt
 * flash flags and lengths, the burst frames, the camera variant, the
 * ambient level, four textures, the per-point bolt colours, the glow tint,
 * the screen flash and the spark colours. */
typedef struct {
    Model817Config *G32 config;
    SVECTOR points[3][16];
    SVECTOR strands[3][16][16];
    SVECTOR burst[3][8];
    SVECTOR burst_velocity[3][8];
    SVECTOR smoke[3][16];
    SVECTOR smoke_velocity[3][16];
    SVECTOR sparks[88][6];
    u32 elapsed;
    u32 frames;
    u16 head;
    u16 phase;
    u16 flashed[16];
    u16 unknown_2EB0;
    u16 spark_count;
    u16 length[16];
    u16 burst_frame[3];
    u16 variant;
    s32 ambient;
    s32 texture[4];
    u8 red[3][16];
    u8 green[3][16];
    u8 blue[3][16];
    u8 tint_r, tint_g, tint_b;
    u8 flash;
    u8 spark_r[88];
    u8 spark_g[88];
    u8 spark_b[88];
} Model817State;

extern GsIMAGE D_8013E1AC[];
extern Model817Config D_8013E21C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
