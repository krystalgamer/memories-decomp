#ifndef MEMORIES_DECOMP_MODEL_VARIANT649_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT649_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the size of the shapes, then
 * the colour of the streaks, orb and shards and the colour of the dome. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model649Command;

/* One 0x4C-byte element placed through its own coordinate unit. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_3C;
} Model649Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    /* The opposing slot, which receives the tint and the camera orbit. */
    s32 target;
    s32 tint;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4], unknown_2A;
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_GT3 shard;
    SVECTOR corners[4];
    /* Nine rings of nine points on the dome. */
    SVECTOR dome[9][9];
    SVECTOR quad[4];
    SVECTOR triangle[3];
    /* Two shard triangles sharing their second and third points. */
    SVECTOR fan[4];
    /* Camera-orbit flag, then the finish counter at flags[4]. */
    u8 flags[5];
    u8 streak_count, shard_count, spark_count;
    SVECTOR position;
    Model649Element streaks[64];
    Model649Element canopy;
    Model649Element orb;
    Model649Element shards[32];
    Model649Element sparks[32];
} Model649State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model649ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013DAAC[3];
extern Model649Command D_8013DB00[];

s32 func_8013B004(u8 *context, s32 command);

#endif
