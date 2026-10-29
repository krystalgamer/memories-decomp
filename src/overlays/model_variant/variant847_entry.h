#ifndef MEMORIES_DECOMP_MODEL_VARIANT847_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT847_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x48-byte command row: the colours of the two bursts, the two model
 * parts that leave trails, the ring radii and depth, the star size, the
 * debris size, count and speed, the ember count, size and speed, the delay,
 * the trail and hit times, the camera shake and the flash flag. */
typedef struct {
    u8 r[2], g[2], b[2];
    u8 parts[2];
    u16 inner_radius, outer_radius, ring_depth;
    u16 star_size;
    u16 debris_size;
    u16 debris_count;
    u16 debris_speed;
    u16 ember_count;
    u16 ember_size;
    u16 ember_speed;
    u8 unknown_1C[0x12];
    u16 delay;
    u16 second_trail_start;
    u16 first_hit;
    u16 second_hit;
    u16 unknown_36[5];
    u16 shake_value, shake_arg;
    u16 flash;
    u16 pad_46;
} Model847Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model847Color;

/* The work area, with one column per burst: the command row, the two ring
 * edges, the debris, embers and sparks with their velocities, the trail
 * points, the debris frames, the trail heads, the timers, the phase, the
 * ring scales, the ambient level, the textures, the burst texture page and
 * palette, and the burst, flash and trail colours. */
typedef struct {
    Model847Config *G32 config;
    SVECTOR unknown_4[128];
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR debris[8][2];
    SVECTOR debris_velocities[8][2];
    SVECTOR embers[64][2];
    SVECTOR ember_velocities[64][2];
    SVECTOR sparks[32][2];
    SVECTOR spark_velocities[32][2];
    SVECTOR trail[64][2];
    u16 debris_frame[2];
    u16 trail_head[2];
    u16 elapsed;
    u16 tick;
    u16 phase;
    u16 unknown_1612[2];
    u32 ring_scale[2];
    s32 level;
    s32 texture[4];
    u16 burst_tpage, burst_clut;
    Model847Color color[2];
    Model847Color flash;
    Model847Color trail_color[2];
} Model847State;

extern GsIMAGE D_8013CE94[];
extern Model847Config D_8013CF04[];

s32 func_8013B004(u8 *context, s32 command);

#endif
