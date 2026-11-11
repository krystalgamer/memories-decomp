#ifndef MEMORIES_DECOMP_MODEL_VARIANT795_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT795_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_camera_effects.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* The work area: the command row; per bolt its position, step, strip
 * angle, ring offsets, target, 24-point trail, origin, smoke puffs and
 * their velocities and impact rays; the sixteen inner and outer fan
 * points; the sparks; the fan growth, the timer, the flight counters, the
 * frame count, puff and spark frames, trail lengths, spark counts and
 * trail heads; the bolt count, the arrival flags, the phase, the ambient
 * level, the textures and the per-bolt bolt, glow and ray colours. */
typedef struct {
    ModelVariantBoltFanConfig *config;
    SVECTOR pos[8];
    SVECTOR step[8];
    SVECTOR angle[8];
    SVECTOR ring[8][18];
    SVECTOR target[8];
    SVECTOR trail[8][24];
    SVECTOR origin[8];
    SVECTOR puffs[8][8];
    SVECTOR puff_velocity[8][8];
    SVECTOR rays[8][32];
    SVECTOR fan[16][2];
    SVECTOR sparks[8][16];
    u32 growth;
    u32 elapsed;
    u32 flight[8];
    u32 frames;
    u16 puff_frame[8][8];
    u16 spark_frame[8][8];
    u16 tail[8];
    u16 unknown_1E00;
    u16 spark_count[8];
    u16 trail_head[8];
    u16 head;
    u16 arrived[8];
    u16 phase;
    s32 ambient;
    s32 texture[5];
    u8 bolt_r[8];
    u8 bolt_g[8];
    u8 bolt_b[8];
    u8 glow_r[8];
    u8 glow_g[8];
    u8 glow_b[8];
    u8 ray_r[8];
    u8 ray_g[8];
    u8 ray_b[8];
} Model795State;

extern GsIMAGE gModelVariant795_aImages[];
extern ModelVariantBoltFanConfig gModelVariant795_aConfigs[];

s32 ModelVariant795_HandleRequest(u8 *context, s32 command);

#endif
