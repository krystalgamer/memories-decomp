#ifndef MEMORIES_DECOMP_MODEL_VARIANT650_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT650_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the radius, a value the entry
 * copies but never reads, the spinning quads' colour, and the colour of the
 * sphere and the fan. */
typedef struct {
    s16 delay;
    u16 radius, unknown_04;
    u8 red, green, blue, sphere_red, sphere_green, sphere_blue;
} Model650Command;

/* One 0x4C-byte record: a spinning quad, the sphere or the fan, moving from
 * position along growth over its lifetime. */
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
} Model650Record;

/* One spinning quad's arc: the amplitude and the divisor of its parabola. */
typedef struct {
    s32 amplitude;
    s32 divisor;
} Model650Arc;

/* The work area: the command, the slot, the packets, the quad corners, the
 * 17x9 sphere points, the fan ring, the flags, the record counts, the arcs,
 * and three record arrays: the spinning quads, the spheres and the fans. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, unknown_10;
    u16 tpage[3];
    u16 clut[4];
    POLY_FT4 flat;
    u8 unknown_48[0x34];
    POLY_G3 fan;
    SVECTOR square[4];
    SVECTOR grid[17][9];
    SVECTOR ring[17];
    u8 flags[5];
    u8 quad_count, sphere_count, fan_count;
    Model650Arc sideways[8];
    Model650Arc vertical[8];
    Model650Record quads[8];
    Model650Record spheres[4];
    Model650Record fans[64];
} Model650State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model650ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CB20[2];
extern Model650Command D_8013CB58[];

s32 func_8013B004(u8 *context, s32 command);

#endif
