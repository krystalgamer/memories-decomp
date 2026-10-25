#ifndef MEMORIES_DECOMP_MODEL_VARIANT654_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT654_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the burst radius, a value the
 * entry copies but never reads, the burst and ring colour, and the sphere
 * colour. */
typedef struct {
    s16 delay;
    u16 radius, unknown_04;
    u8 red, green, blue, sphere_red, sphere_green, sphere_blue;
} Model654Command;

/* One burst quad, ring quad or sphere: the 0x4C-byte burst record of header
 * 647, flying from position along growth over its lifetime. */
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
} Model654Burst;

/* The work area: the command, the radius, the shared quad and line packets,
 * the quad corners and the 9x9 sphere points, the column flags, the grid
 * sizes, and the bursts, the ring quads and one sphere per column. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, unknown_10;
    u16 tpage, unknown_14[2];
    u16 clut, unknown_1A[3];
    POLY_FT4 flat;
    GsGLINE line;
    SVECTOR square[4];
    SVECTOR grid[9][9];
    u8 flags[5];
    u8 rows, columns, ring_count;
    Model654Burst bursts[8][12];
    Model654Burst rings[8][12];
    Model654Burst spheres[12];
} Model654State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model654ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D404;
extern Model654Command D_8013D420[];

s32 func_8013B004(u8 *context, s32 command);

#endif
