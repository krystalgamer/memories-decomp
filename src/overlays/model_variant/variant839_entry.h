#ifndef MEMORIES_DECOMP_MODEL_VARIANT839_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT839_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x0C-byte command row: the model part the bubbles leave from, the base
 * bubble size, the sideways spread and forward speed, the upward speed, and
 * the start and the end of spawning. */
typedef struct {
    u8 part;
    u8 unknown_1;
    u16 size;
    u16 speed;
    u16 rise;
    u16 start;
    u16 spawn_end;
} Model839Config;

/* The work area: the command row, each bubble's position and velocity, four
 * fragments per bubble and their velocities, sixteen sparks and their
 * velocities, the bubble sizes and frames, the spark frames, the timers, the
 * bubble and spark counts, the phase, the bubble states, the spark flags,
 * the ambient level, the textures, and a texture page and palette that the
 * entry sets but never reads. */
typedef struct {
    Model839Config *G32 config;
    SVECTOR positions[128];
    SVECTOR velocities[128];
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
} Model839State;

extern GsIMAGE gModelVariant839_D14E4[3];
extern Model839Config gModelVariant839_D1538[];

s32 ModelVariant839_HandleRequest(u8 *context, s32 command);

#endif
