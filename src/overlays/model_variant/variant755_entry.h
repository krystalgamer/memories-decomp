#ifndef MEMORIES_DECOMP_MODEL_VARIANT755_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT755_ENTRY_H

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

/* One 0x12-byte command row: the shard colour and the model part the sparks
 * leave from, the base shard size, the flight time, the spawn period, the
 * per-frame growth, and the start and the end of spawning. */
typedef struct {
    u8 r, g, b;
    u8 part;
    u16 size;
    u16 flight;
    u16 unknown_8;
    u16 period;
    u16 growth;
    u16 start;
    u16 spawn_end;
} Model755Config;

/* The work area: the command row, each shard's position, velocity and
 * target, four fragments per shard and their velocities, sixteen sparks and
 * their velocities, the shard sizes and frames, the spark frames, the
 * timers, the shard and spark counts, the phase, the shard states, the spark
 * flags, the ambient level, the textures, and a texture page and palette
 * that the entry sets but never reads. */
typedef struct {
    Model755Config *G32 config;
    SVECTOR positions[128];
    SVECTOR velocities[128];
    SVECTOR targets[128];
    SVECTOR fragments[128][4];
    SVECTOR fragment_velocities[128][4];
    SVECTOR sparks[16];
    SVECTOR spark_velocities[16];
    u16 sizes[128];
    u16 frames[128];
    u16 spark_frames[16];
    u32 elapsed;
    u32 tick;
    u16 count;
    u16 spark_count;
    u16 phase;
    u16 states[128];
    u16 spark_flags[16];
    s32 ambient;
    s32 texture[3];
    u16 unused_tpage, unused_clut;
} Model755State;

extern GsIMAGE D_8013C630[3];
extern Model755Config D_8013C684[];

#endif
