#ifndef MEMORIES_DECOMP_MODEL_VARIANT763_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT763_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x2C-byte command row: the colour, the radius of the sixteen-point
 * fan, the spread of the shards, the speed of the dots, the shard and dot
 * counts, the half lengths of the shard base and of the flash quad, the inner
 * and outer ring radii, the triangle height, width and roll, the ring offset,
 * the start delay, the start of the rings and the end of the effect. */
typedef struct {
    u8 r, g, b, unknown_03;
    s16 fan_radius;
    s16 shard_spread;
    s16 dot_speed;
    s16 shard_count;
    s16 dot_count;
    s16 base_half;
    s16 quad_half;
    s16 inner_radius;
    s16 outer_radius;
    s16 unknown_16;
    s32 triangle_height;
    s16 triangle_width;
    s16 unknown_1E;
    s32 triangle_roll;
    s16 ring_offset;
    s16 delay;
    s16 ring_start;
    s16 end;
} Model763Config;

/* The 0x70C-byte work area: the command row, the fan centre and rim, the
 * shard tips, the shard base, the flash quad, the dots and their velocities,
 * the inner and outer ring points, the triangle, the projected dots, the two
 * fading colours, the textures, the dot depths, the ambient level, the ring
 * pulse, the elapsed time, the flash frame and the phase. */
typedef struct {
    Model763Config *G32 config;
    SVECTOR center;
    SVECTOR rim[31];
    SVECTOR shards[32];
    SVECTOR base[2];
    SVECTOR quad[4];
    u8 unknown_234[0xE0];
    SVECTOR dots[32];
    SVECTOR dot_velocities[32];
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR triangle[3];
    DVECTOR screen[32];
    u8 color[3];
    u8 color2[3];
    u8 unknown_6B2[2];
    s32 unknown_6B4;
    s32 texture[2];
    u16 depths[32];
    s32 ambient;
    s16 pulse;
    s16 elapsed;
    s16 flash;
    s16 phase;
} Model763State;

extern GsIMAGE D_8013CBA8[2];
extern Model763Config D_8013CBE0[];

s32 func_8013B004(u8 *context, s32 command);

#endif
