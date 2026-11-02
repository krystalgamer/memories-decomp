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
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x48-byte command row: the ring colours and the model points the two
 * trails follow, the ring radii and depth, the star size, the debris and
 * ember sizes, counts and speeds, the timing of the trails and the bursts,
 * the shake and the flash flag. */
typedef struct {
    u8 r[2], g[2], b[2];
    u8 part[2];
    u16 inner_radius, outer_radius, ring_depth;
    u16 star_size;
    u16 debris_size;
    u16 debris_count;
    u16 debris_speed;
    u16 ember_count;
    u16 ember_size;
    u16 ember_speed;
    u16 unknown_1C[9];
    u16 delay;
    u16 second_start;
    u16 first_end;
    u16 second_end;
    u16 unknown_36[5];
    u16 shake_value, shake_arg;
    u16 flash;
    u16 unknown_46;
} Model847Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model847Color;

/* The work area: the command row, an unused block, the ring edges, for both
 * bursts the debris, embers and sparks and their velocities and the trail
 * ring of 48 points, the debris frames, the trail heads, the timers, the
 * ring scales, the ambient level, the textures, and the ring, flash and
 * trail colours. */
typedef struct {
    Model847Config *G32 config;
    SVECTOR unknown_4[64][2];
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR debris[8][2];
    SVECTOR debris_velocities[8][2];
    SVECTOR embers[64][2];
    SVECTOR ember_velocities[64][2];
    SVECTOR sparks[32][2];
    SVECTOR spark_velocities[32][2];
    SVECTOR trail[48][2];
    SVECTOR unknown_1504[16][2];
    u16 debris_frame[2];
    u16 head[2];
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
    CVECTOR glow[2];
} Model847State;

extern GsIMAGE gModelVariant847_D1E94[];
extern Model847Config gModelVariant847_D1F04[];

s32 ModelVariant847_HandleRequest(u8 *context, s32 command);

#endif
