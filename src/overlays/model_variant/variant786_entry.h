#ifndef MEMORIES_DECOMP_MODEL_VARIANT786_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT786_ENTRY_H

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

/* One 0x28-byte command row: the ring sprite colour, the trail colour, the model
 * part the bolts leave from, the bolt size range, the flight length, the
 * bolt count, the launch and drift spreads, the ring spread, the ring sprite
 * size, the ring count and rise speed, the glow size, the fade step, the
 * ray spread, width and count and the start time. */
typedef struct {
    u8 r, g, b;
    u8 bolt_r, bolt_g, bolt_b;
    u8 part, unknown_07;
    s16 size;
    s16 steps;
    s16 count;
    s16 spread;
    s16 drift;
    s16 ring_spread;
    u16 ring_size;
    s16 ring_count;
    s16 ring_speed;
    u16 glow_size;
    u8 fade, unknown_1D;
    s16 ray_spread;
    u16 ray_width;
    s16 ray_count;
    s16 start;
    u16 unknown_26;
} Model786Config;

/* The work area: the command row; per bolt its position, flight step,
 * ring offsets, impact rays, 16-point trail, angle and size; the timer,
 * trail heads, the phase, the per-bolt states, the launch stage, the
 * active bolt count, the shown ring counts, trail lengths and ring frames;
 * the ambient level, the three textures and the per-bolt glow and bolt
 * colours. */
typedef struct {
    Model786Config *G32 config;
    SVECTOR pos[48];
    SVECTOR step[48];
    SVECTOR ring[48][4];
    SVECTOR rays[48][16];
    SVECTOR trail[48][16];
    SVECTOR angle[48];
    u16 size[48];
    u32 elapsed;
    s16 trail_head[48];
    u16 phase;
    u16 state[48];
    u16 stage;
    /* Retail tests ring_frame[i][i] (not [i][j]): halfword i * 5, byte
     * i * 10, from ring_frame. The only command row has 48 bolts, so i
     * reaches 47 and the read reaches bytes 470-471 from ring_frame, past its
     * 384 bytes into ambient, texture, glow and bolt_r. bytes is the
     * in-bounds backing view of this word-aligned tail (active onwards, 788
     * bytes) through which that diagonal is read. */
    union {
        struct {
            u16 active;
            u16 ring_shown[48];
            u16 tail[48];
            u16 ring_frame[48][4];
            s32 ambient;
            s32 texture[3];
            u8 glow[48];
            u8 bolt_r[48];
            u8 bolt_g[48];
            u8 bolt_b[48];
        } s;
        u8 bytes[788];
    } view;
} Model786State;

/* Byte offset of ring_frame[0][0] within Model786State.view.bytes. */
#define MODEL786_RING_FRAME_BYTE 194

extern GsIMAGE D_8013CDA8[];
extern Model786Config D_8013CDFC[];

s32 func_8013B004(u8 *context, s32 command);

#endif
