#ifndef MEMORIES_DECOMP_MODEL_VARIANT757_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT757_ENTRY_H

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
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x14-byte command row: the spark colour, the fragment colour, the
 * model part the sparks leave from, the base shard size, the flight time,
 * the spawn period, the per-frame growth, and the start and the end of
 * spawning. */
typedef struct {
    u8 r, g, b;
    u8 fragment_r, fragment_g, fragment_b;
    u8 part;
    u8 pad;
    u16 size;
    u16 flight;
    u16 period;
    u16 growth;
    u16 start;
    u16 spawn_end;
} Model757Config;

/* The work area: the command row, each shard's position, velocity and
 * target, sixteen fragments per shard and their velocities, thirty-two
 * sparks and their velocities, the shard sizes, images and frames, the
 * spark frames, the timers, the shard and spark counts, the phase, the
 * shard states, the spark flags, the ambient level, the textures (three
 * shard images, one sheet packed in two modes for the sparks and the
 * fragments, and a sixth texture that the entry packs but never reads), and
 * a texture page and palette that the entry sets but never reads. */
typedef struct {
    Model757Config *G32 config;
    SVECTOR positions[52];
    SVECTOR velocities[52];
    SVECTOR targets[52];
    SVECTOR fragments[52][16];
    SVECTOR fragment_velocities[52][16];
    SVECTOR sparks[32];
    SVECTOR spark_velocities[32];
    u16 sizes[52];
    u16 images[52];
    u16 frames[52];
    u16 spark_frames[32];
    u32 elapsed;
    u32 tick;
    u16 count;
    u16 spark_count;
    u16 phase;
    u16 states[52];
    u16 spark_flags[32];
    s32 ambient;
    s32 texture[6];
    u16 unused_tpage, unused_clut;
} Model757State;

extern GsIMAGE D_8013C4F8[5];
extern Model757Config D_8013C584[];

#endif
