#ifndef MEMORIES_DECOMP_MODEL_VARIANT846_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT846_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, 32-point trail, origin, smoke puffs
 * and their velocities, impact rays, 64 drifting impact flecks and their
 * velocities, and sparks; the timer, the per-bolt flight counters, puff and
 * spark frames, trail lengths, spark counts and trail heads; the bolt count,
 * the arrival flags, the phase, the ambient level, the six textures and the
 * per-bolt bolt, glow and ray colours. The work area is 0x1126 bytes. */
typedef struct {
    ModelVariantTwinBoltConfig *config;
    SVECTOR pos[2];
    SVECTOR step[2];
    SVECTOR angle[2];
    SVECTOR ring[2][32];
    SVECTOR target[2];
    SVECTOR trail[2][32];
    SVECTOR origin[2];
    SVECTOR puffs[2][16];
    SVECTOR puff_velocity[2][16];
    SVECTOR rays[2][16];
    SVECTOR flecks[2][64];
    SVECTOR fleck_velocity[2][64];
    SVECTOR sparks[2][16];
    u32 elapsed;
    u32 flight[2];
    u16 puff_frame[2][16];
    u16 spark_frame[2][16];
    u16 tail[2];
    u16 unknown_10E4;
    u16 spark_count[2];
    u16 trail_head[2];
    u16 head;
    u16 arrived[2];
    u16 phase;
    s32 ambient;
    s32 texture[6];
    u8 bolt_r[2];
    u8 bolt_g[2];
    u8 bolt_b[2];
    u8 glow_r[2];
    u8 glow_g[2];
    u8 glow_b[2];
    u8 ray_r[2];
    u8 ray_g[2];
    u8 ray_b[2];
} Model846State;

extern GsIMAGE gModelVariant846_aImages[];
extern ModelVariantTwinBoltConfig gModelVariant846_aConfigs[];

s32 ModelVariant846_HandleRequest(u8 *context, s32 command);

#endif
