#ifndef MEMORIES_DECOMP_MODEL_VARIANT644_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT644_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the size of the effect and
 * its colour; the last three bytes are unused by this entry. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model644Command;

/* One 0x50-byte element: a glowing cage, a spinning star or a burst. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u32 unknown_10;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_40;
} Model644Element;

/* Two points of one line segment. */
typedef struct {
    SVECTOR from, to;
} Model644Segment;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_GT4 strip;
    POLY_GT3 fan;
    GsGLINE line;
    /* Nine rings of four inner then four outer band points. */
    SVECTOR band[9][8];
    SVECTOR rim[9];
    Model644Segment spokes[8];
    Model644Segment cage[8][9];
    /* Spawn timer, burst cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 count[3];
    Model644Element glows[2];
    Model644Element stars[2];
    Model644Element bursts[32];
} Model644State;

/* An eight-byte copy of the view metrics; the entry never reads it. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model644ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D218;
extern Model644Command D_8013D234[];

s32 func_8013B004(u8 *context, s32 command);

#endif
