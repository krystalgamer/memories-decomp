#ifndef MEMORIES_DECOMP_MODEL_VARIANT780_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT780_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x28-byte command row: the model part the glow sits on, the band
 * count and vertical step, the cylinder rise speed, the spark count, size,
 * rise speed and trail spacing, the glow size, the ray count and spread,
 * the fan size and spin, and the start, fade, burst, release and end
 * times. */
typedef struct {
    u8 part, unknown_01;
    s16 count;
    s16 rise_step;
    u16 rise_speed;
    s16 spark_count;
    s16 spark_size;
    u16 spark_rise;
    s16 spark_spacing;
    u16 beam_size;
    s16 ray_count;
    s16 ray_spread;
    u16 fan_size;
    s16 spin;
    s16 start;
    s16 fade_time;
    s16 burst_time;
    s16 release_time;
    s16 unknown_22;
    u32 end_time;
} Model780Config;

/* The work area: the command row; three 16-point rings at, outside and
 * inside the band radius and the half-radius cylinder ring; the spark
 * offsets and fan rays; the opposing slot's position; the cylinder rise,
 * the shown band or spark count, the timer, the spark counter, the phase,
 * the band radius and the ambient level; per band a scale and brightness;
 * the disc page and CLUT, the three textures, the disc brightness, the
 * per-spark brightness and the glow brightness. */
typedef struct {
    Model780Config *G32 config;
    SVECTOR ring[16];
    SVECTOR outer[16];
    SVECTOR inner[16];
    SVECTOR core[16];
    SVECTOR sparks[64];
    SVECTOR rays[32];
    SVECTOR centre;
    u16 rise;
    u16 active;
    u32 elapsed;
    u16 counter;
    u16 phase;
    u16 radius;
    s32 ambient;
    u32 scale[12];
    u16 burst_tpage;
    u16 burst_clut;
    s32 texture[3];
    u8 column[12];
    u8 burst;
    u8 spark[64];
    u8 beam;
} Model780State;

extern GsIMAGE gModelVariant780_D1DF0[];
extern Model780Config gModelVariant780_D1E44[];

s32 ModelVariant780_HandleRequest(u8 *context, s32 command);

#endif
