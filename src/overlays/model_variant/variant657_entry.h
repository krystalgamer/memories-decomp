#ifndef MEMORIES_DECOMP_MODEL_VARIANT657_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT657_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x10-byte command row: the start delay, the ring radius, the band
 * height, then the ring, the band/cap and the shard colours. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[9];
    u8 unknown_0F;
} Model657Command;

/* One 0x4C-byte effect part placed through a camera-relative coordinate
 * unit; the shards also keep an offset from their ring's centre. */
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
    VECTOR offset;
} Model657Part;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    SVECTOR ring[9][17][3];
    SVECTOR band[17][9][3];
    SVECTOR cap[9][5][3];
    SVECTOR corners[4];
    /* Flag 1 marks a started effect and flag 4 a finished one. */
    u8 flags[5];
    u8 ring_count, shard_count;
    u8 triggered[3], unknown_218E[2];
    Model657Part rings[3];
    Model657Part bands[3];
    Model657Part caps[3];
    Model657Part shards[8][3];
} Model657State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model657ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013FBC8[3];
extern Model657Command D_8013FC1C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
