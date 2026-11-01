#ifndef MEMORIES_DECOMP_MODEL_VARIANT821_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT821_ENTRY_H

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

/* One 0x34-byte command row: the bolt colour, the model point each of the
 * three volleys leaves from, the chance of a ground strike, the bolt size,
 * the flight length, the ground spread, the strip width and radius, the
 * ring sprite size, the spark spread and size, the smoke puff size, rise
 * and spread, the debris size and spread, the flare spread, the fade step
 * and the spawn period, start and end. */
typedef struct {
    u8 r, g, b;
    u8 part[3];
    u16 chance;
    u16 size;
    u16 steps;
    u16 spread;
    u16 strip_width;
    u16 strip_radius;
    u16 ring_size;
    u16 spark_spread;
    u16 spark_width;
    u16 spark_height;
    u16 puff_size;
    u16 puff_rise;
    u16 puff_spread;
    u16 debris_size;
    u16 debris_spread;
    u16 unknown_24;
    u16 flare_spread;
    u8 fade, unknown_29;
    u16 unknown_2A;
    u16 unknown_2C;
    u16 period;
    u16 start;
    u16 end;
} Model821Config;

/* The work area: the command row; for each of three volleys of ten bolts
 * the position, step, target, angle, ring offsets, sixteen-point trail,
 * impact sparks, smoke puffs, debris and its velocity, flares and their
 * velocity; the timers, flight counters, phase, bolt states, volley heads,
 * trail lengths, spark counts, trail heads, spark, puff and flare frames,
 * the camera side, the ambient level, the textures and the bolt colours. */
typedef struct {
    Model821Config *G32 config;
    SVECTOR pos[3][10];
    SVECTOR step[3][10];
    SVECTOR target[3][10];
    SVECTOR angle[3][10];
    SVECTOR ring[3][10][8];
    SVECTOR trail[3][10][16];
    SVECTOR sparks[3][10][6];
    SVECTOR puffs[3][10][5];
    SVECTOR debris[3][10][8];
    SVECTOR debris_velocity[3][10][8];
    SVECTOR flares[3][10][2];
    SVECTOR flare_velocity[3][10][2];
    u32 elapsed;
    u32 frames;
    u16 flight[3][10];
    u16 phase;
    u16 state[3][10];
    u16 head[3];
    u16 tail[3][10];
    u16 spark_count[3][10];
    u16 trail_head[3][10];
    u16 spark_frame[3][10][6];
    u16 puff_frame[3][10];
    u16 flare_frame[3][10];
    u16 side;
    u16 unknown_3A72[61];
    s32 ambient;
    s32 texture[6];
    u8 r[3][10];
    u8 g[3][10];
    u8 b[3][10];
} Model821State;

extern GsIMAGE gModelVariant821_D3930[];
extern Model821Config gModelVariant821_D39BC[];

s32 ModelVariant821_HandleRequest(u8 *context, s32 command);

#endif
