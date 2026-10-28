#ifndef MEMORIES_DECOMP_MODEL_VARIANT670_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT670_ENTRY_H

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
#include "../../game/model_copy_slot_position.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the radius, the height, the
 * shell colour and the ring colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, ring_red, ring_green, ring_blue;
} Model670Command;

/* One 0x4C-byte element: a textured shell or ring placed at position. */
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
} Model670Element;

/* The work area: the command, the slot, the packets, the meshes and the
 * two element arrays. The shell is a sprite square, an eight-point base in
 * two rings, an 8x4 grid of dome points and the 2x8x4 mesh a fifth of the
 * way from each base ring to the dome; the ring is a band between an inner
 * and an outer circle, with a square flash on top. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1E;
    POLY_FT4 sprite;
    POLY_GT3 fan;
    POLY_GT4 strip;
    SVECTOR square[4];
    /* [ring][point] */
    SVECTOR base[2][8];
    /* [meridian][row] */
    SVECTOR dome[8][4];
    SVECTOR inner[17];
    SVECTOR outer[17];
    SVECTOR corners[4];
    /* [ring][meridian][row] */
    SVECTOR mesh[2][8][4];
    /* Shell cursor and, in [4], the finished latch. */
    u8 flags[5];
    u8 count;
    s16 position[3];
    u8 unknown_580[4];
    Model670Element shells[32];
    Model670Element rings[32];
} Model670State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model670ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CD80[3];
extern Model670Command D_8013CDD4[];

s32 func_8013B004(u8 *context, s32 command);

#endif
