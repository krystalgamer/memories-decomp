#ifndef MEMORIES_DECOMP_MODEL_VARIANT818_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT818_ENTRY_H

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
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x44-byte command row: the head sprite colour, the bolt and glow
 * colours, the model point the bolt leaves from, the head size, the flight
 * length, the bolt count, the ring of flares and spinning strips around the
 * bolt, the smoke puffs, the impact glow and shock-fan radii and depth, the
 * sparks, the trail fade step, the shock-fan pulse steps and the camera
 * phase times. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 glow_r, glow_g, glow_b;
    u8 part;
    u16 size;
    u16 steps;
    u16 count;
    u16 ring_count;
    u16 strip_width;
    u16 strip_radius;
    u16 flare_size;
    u16 puff_spread;
    u16 puff_count;
    u16 puff_speed;
    u16 puff_size;
    u16 glow_size;
    u16 unknown_22;
    u16 fan_inner;
    u16 fan_outer;
    u16 fan_depth;
    u16 spark_spread;
    u16 spark_size;
    u16 spark_count;
    u8 fade, unknown_31;
    u16 unknown_32;
    u16 pulse_fast;
    u16 unknown_36;
    u16 pulse_slow;
    u16 unknown_3A;
    u16 start;
    u16 approach;
    u16 end;
    u16 unknown_42;
} Model818Config;

/* One point of the sixteen-segment shock fan. */
typedef struct {
    SVECTOR inner;
    SVECTOR outer;
} Model818FanPoint;

/* A fading colour, padded to a word. */
typedef struct {
    u8 r, g, b, pad;
} Model818Colour;

/* The work area; the effect has a single bolt, so every per-bolt array has
 * one row: the command row; the bolt position, step, strip angle, flare
 * ring offsets, target, sixteen-point trail and origin; the smoke puffs, the
 * launch debris, the shock fan, the sparks, the impact chips, shards and
 * dust with their velocities and the 64 spinning shock blades; the timer,
 * frame counters, trail lengths and heads, the bolt count, arrival flag,
 * phase, ambient level, shock-fan texture page and CLUT, the eleven
 * textures and the bolt, glow and launch-flash colours. */
typedef struct {
    Model818Config *G32 config;
    SVECTOR pos[1];
    SVECTOR step[1];
    SVECTOR angle[1];
    SVECTOR ring[1][16];
    SVECTOR target[1];
    SVECTOR trail[1][16];
    SVECTOR origin[1];
    SVECTOR puffs[1][8];
    SVECTOR puff_velocity[1][8];
    SVECTOR debris[1][32];
    SVECTOR debris_velocity[1][32];
    Model818FanPoint fan[16];
    SVECTOR sparks[1][64];
    SVECTOR chips[1][24];
    SVECTOR chip_velocity[1][24];
    SVECTOR shards[1][24];
    SVECTOR shard_velocity[1][24];
    SVECTOR dust[1][128];
    SVECTOR dust_velocity[1][128];
    SVECTOR spin[1][64];
    u16 elapsed;
    /* The pulse counter is read and stepped with the index i - count, which
     * for the single bolt is the row before it, so the flight and pulse
     * counters are also viewed as two rows. */
    union {
        struct {
            u16 flight[1];
            u16 pulse[1];
        } n;
        u16 rows[2][1];
    } u;
    u16 growth[1];
    u16 puff_frame[1][8];
    u16 spark_frame[1][64];
    u16 shard_frame[1];
    u16 tail[1];
    u16 unknown_1448;
    u16 spark_count[1];
    u16 trail_head[1];
    u16 head;
    u16 arrived[1];
    u16 phase;
    s32 ambient;
    u16 fan_tpage;
    u16 fan_clut;
    s32 texture[11];
    Model818Colour bolt[1];
    Model818Colour glow[1];
    Model818Colour flash[1];
} Model818State;

extern GsIMAGE gModelVariant818_D48A0[];
extern Model818Config gModelVariant818_D49D4[];

s32 ModelVariant818_HandleRequest(u8 *context, s32 command);

#endif
