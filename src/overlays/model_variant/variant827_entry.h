#ifndef MEMORIES_DECOMP_MODEL_VARIANT827_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT827_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_scene_runtime.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model827Color;

/* The work area: the command row, the ray rotations, the spark directions, the
 * debris and embers and their velocities, the elapsed time, the phase, the
 * spark frames, the debris frame, the spark count, the camera sequence, the
 * ambient level, the ray scale, the textures and the burst colour. */
typedef struct {
    ModelVariantBurstConfig *config;
    SVECTOR rays[64];
    SVECTOR sparks[16];
    SVECTOR debris[8];
    SVECTOR debris_velocities[8];
    SVECTOR embers[64];
    SVECTOR ember_velocities[64];
    u32 elapsed;
    u16 phase;
    u16 frames[16];
    u16 debris_frame;
    u16 spark_count;
    u16 camera;
    s32 level;
    u32 ray_scale;
    s32 texture[4];
    Model827Color color;
} Model827State;

extern GsIMAGE gModelVariant827_aImages[];
extern ModelVariantBurstConfig gModelVariant827_aConfigs[];

s32 ModelVariant827_HandleRequest(u8 *context, s32 command);

#endif
