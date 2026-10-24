#ifndef MEMORIES_DECOMP_MODEL_VARIANT627_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT627_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xE-byte command row: the start delay, the inner and outer radius and
 * the height of the cone, the colour, and the ring lifetime. */
typedef struct {
    s16 delay;
    u16 inner, outer, height;
    u8 red, green, blue, unknown_0B;
    u16 duration;
} Model627Command;

/* One ring or one fan of lines: the 0x4C-byte burst record of header 647,
 * with the cone growth in growth.vy (outer edge) and growth.vz (inner edge). */
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
} Model627Ring;

/* The work area: the command, the shared packets and vertex rings, and up to
 * ten rings and ten fans of lines. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 inner, outer, height;
    u8 unknown_10[0xC];
    POLY_G4 quad;
    GsGLINE line;
    SVECTOR rim[17];
    SVECTOR edge[17];
    SVECTOR spokes[32][2];
    u8 flags[5];
    u8 ring_count, line_count, unknown_36B;
    Model627Ring rings[10];
    Model627Ring lines[10];
} Model627State;

extern MATRIX D_800FE168;
extern Model627Command D_8013C384[];

s32 func_8013B004(u8 *context, s32 command);

#endif
