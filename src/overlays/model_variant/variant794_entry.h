#ifndef MEMORIES_DECOMP_MODEL_VARIANT794_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT794_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, 32-point trail, origin, smoke puffs
 * and their velocities, impact rays and sparks; the timers, the per-bolt
 * flight counters, puff and spark frames, trail lengths, spark counts and
 * trail heads; the bolt count, the arrival flags, the phase, the ambient
 * level, the five textures and the per-bolt bolt, glow and ray colours. */
typedef struct {
    ModelVariantBoltStreamConfig *config;
    SVECTOR pos[16];
    SVECTOR step[16];
    SVECTOR angle[16];
    SVECTOR ring[16][16];
    SVECTOR target[16];
    SVECTOR trail[16][32];
    SVECTOR origin[16];
    SVECTOR puffs[16][8];
    SVECTOR puff_velocity[16][8];
    SVECTOR rays[16][16];
    SVECTOR sparks[16][8];
    u32 elapsed;
    u32 flight[16];
    u32 frames;
    u16 puff_frame[16][8];
    u16 spark_frame[16][8];
    u16 tail[16];
    u16 unknown_30EC;
    u16 spark_count[16];
    u16 trail_head[16];
    u16 head;
    u16 arrived[16];
    u16 phase;
    s32 ambient;
    s32 texture[5];
    u8 bolt_r[16];
    u8 bolt_g[16];
    u8 bolt_b[16];
    u8 glow_r[16];
    u8 glow_g[16];
    u8 glow_b[16];
    u8 ray_r[16];
    u8 ray_g[16];
    u8 ray_b[16];
} Model794State;

extern GsIMAGE gModelVariant794_aImages[];
extern ModelVariantBoltStreamConfig gModelVariant794_aConfigs[];

s32 ModelVariant794_HandleRequest(u8 *context, s32 command);

#endif
