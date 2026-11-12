#ifndef MEMORIES_DECOMP_MODEL_VARIANT824_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT824_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"
#include "model_variant.h"

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, sixteen-point trail, origin, smoke puffs and
 * their velocities, impact rays and sparks; the inner and outer shock-ring
 * points; the timer, flight counters, frame count, puff and spark frames,
 * trail lengths, spark counts and trail heads; the bolt count, arrival
 * flags, phase, per-bolt shock-ring growth, ambient level, the shock-ring
 * texture page and CLUT, the textures and the per-bolt colours. */
typedef struct {
    ModelVariantBoltStreamConfig *config;
    SVECTOR pos[28];
    SVECTOR step[28];
    SVECTOR angle[28];
    SVECTOR ring[28][8];
    SVECTOR target[28];
    SVECTOR trail[28][16];
    SVECTOR origin[28];
    SVECTOR puffs[28][8];
    SVECTOR puff_velocity[28][8];
    SVECTOR rays[28][8];
    SVECTOR fan_inner[16];
    SVECTOR fan_outer[16];
    SVECTOR sparks[28][8];
    u32 elapsed;
    u32 flight[28];
    u32 frames;
    u16 puff_frame[28][8];
    u16 spark_frame[28][8];
    u16 tail[28];
    u16 unknown_3A94;
    u16 spark_count[28];
    u16 trail_head[28];
    u16 head;
    u16 arrived[28];
    u16 phase;
    u16 growth[28];
    s32 ambient;
    u16 fan_tpage;
    u16 fan_clut;
    s32 texture[5];
    u8 bolt_r[28];
    u8 bolt_g[28];
    u8 bolt_b[28];
    u8 glow_r[28];
    u8 glow_g[28];
    u8 glow_b[28];
    u8 ray_r[28];
    u8 ray_g[28];
    u8 ray_b[28];
} Model824State;

extern GsIMAGE gModelVariant824_aImages[];
extern ModelVariantBoltStreamConfig gModelVariant824_aConfigs[];

s32 ModelVariant824_HandleRequest(u8 *context, s32 command);

#endif
