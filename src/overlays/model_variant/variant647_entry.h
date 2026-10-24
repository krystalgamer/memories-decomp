#ifndef MEMORIES_DECOMP_MODEL_VARIANT647_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT647_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the ring radius and height,
 * and the colour of the bursts. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, unknown_09[3];
} Model647Command;

/* One burst: a cone of rings that grows from the camera unit while it
 * fades, drawn as quads and triangles from the shared ring vertices. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_4A[2];
    s32 unknown_4C;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_80;
} Model647Burst;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT3 triangle;
    POLY_GT4 quad;
    SVECTOR inner[17][4];
    SVECTOR outer[17][4];
    SVECTOR top[17][8];
    SVECTOR bottom[17][8];
    u8 flags[5];
    u8 count, unknown_D42[2];
    Model647Burst bursts[2];
    u8 unknown_DDC[0x428];
} Model647State;

/* An eight-byte copy of the view metrics that the entry never reads. */
typedef struct {
    u8 bytes[8];
} Model647ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013BF5C;
extern Model647Command D_8013BF78[];

s32 func_8013B004(u8 *context, s32 command);

#endif
