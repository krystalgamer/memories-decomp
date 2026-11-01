#ifndef MEMORIES_DECOMP_MODEL_VARIANT768_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT768_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x2C-byte command row: the trail colour, the four model parts the
 * trails follow, the burst quad half height and depth, the burst drift, the
 * spark radius, the spark base half length, the spark count, the flash quad
 * half size, the dot count and speed, the inner and outer ring radii, the
 * ring height, the start delay, the end of the trail, the burst start and
 * the window in which the entry reports 1 before it ends. */
typedef struct {
    u8 r, g, b;
    u8 parts[4];
    u8 unknown_07;
    s16 burst_half_height;
    s16 burst_half_depth;
    s16 burst_drift;
    s16 spark_radius;
    s16 base_half;
    s16 spark_count;
    s16 quad_half;
    s16 dot_count;
    s16 dot_speed;
    s16 inner_radius;
    s16 outer_radius;
    s16 ring_height;
    s16 delay;
    s16 trail_end;
    s16 burst_start;
    s16 unknown_26;
    s16 report_start;
    s16 end;
} Model768Config;

/* The 0xBF8-byte work area: the command row, four 32-frame trails of part
 * positions (with a flat view for the start-up walk), the two burst columns,
 * the sparks, the burst centre, the dots and their velocities, the inner and
 * outer rings, the two flash rings, the projected dots, the textures, the
 * ambient level, the ring scale, the elapsed time, the phase, the started
 * flag, the trail frame, the burst count, the flash frame, the dot depths and
 * the fading colours. */
typedef struct {
    Model768Config *G32 config;
    union {
        struct {
            SVECTOR part0[32];
            SVECTOR part1[32];
            SVECTOR part2[32];
            SVECTOR part3[32];
        } s;
        SVECTOR flat[128];
    } trails;
    SVECTOR left[32];
    SVECTOR right[32];
    SVECTOR sparks[32];
    SVECTOR center;
    SVECTOR dots[32];
    SVECTOR dot_velocities[32];
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR ring_inner[16];
    SVECTOR ring_outer[16];
    DVECTOR screen[32];
    s32 texture[4];
    s32 ambient;
    s32 ring_scale;
    s16 elapsed;
    s16 phase;
    s16 started;
    s16 frame;
    s16 burst_count;
    s16 flash;
    u16 depths[32];
    u8 color[3];
    u8 burst_level;
    u8 quad_level;
    u8 color2[3];
} Model768State;

extern GsIMAGE gModelVariant768_D2474[4];
extern Model768Config gModelVariant768_D24E4[];

s32 ModelVariant768_HandleRequest(u8 *context, s32 command);

#endif
