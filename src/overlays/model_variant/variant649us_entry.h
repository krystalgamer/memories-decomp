#ifndef MEMORIES_DECOMP_MODEL_VARIANT649US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT649US_ENTRY_H

#include "../../types.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x0C-byte command row: the start delay, the radius, the height, then
 * the primary and the secondary colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model649UsCommand;

/* One 0x4C-byte element placed through its own coordinate unit. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} Model649UsElement;

/* The work area: the command, the slot, the packets, the meshes and the
 * element arrays. The petals are textured quads strung between two joints,
 * the dome is an 8x8 textured grid over a hemisphere, the card is a
 * textured square flying from its anchor to the joints' midpoint, the
 * shards are textured triangles following it and the sparks are pairs of
 * thin triangles bursting from where the card lands. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 side;
    s32 other;
    s32 tint;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4];
    u16 unknown_2A;
    POLY_GT4 quad;
    POLY_FT4 card;
    POLY_GT3 shard;
    SVECTOR petal[4];
    SVECTOR dome[9][9];
    SVECTOR square[4];
    SVECTOR tri[3];
    SVECTOR spike[4];
    /* The camera latch in [0] and, in [4], the finished frame counter. */
    u8 flags[5];
    u8 petal_count, shard_count, spark_count;
    SVECTOR anchor;
    Model649UsElement petals[64];
    Model649UsElement dome_element;
    Model649UsElement card_element;
    Model649UsElement shards[32];
    Model649UsElement sparks[32];
} Model649UsState;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model649UsViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013DAAC[];
extern Model649UsCommand D_8013DB00[];

s32 func_8013B004(u8 *context, s32 command);

#endif
