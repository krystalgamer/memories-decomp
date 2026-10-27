#ifndef MEMORIES_DECOMP_MODEL_VARIANT650_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT650_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the size of the sprites and
 * shells, then the sprite colour and the shell/fan colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model650Command;

/* One 0x4C-byte element: a sprite, shell or fan placed through its own
 * coordinate unit. */
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
    /* Only vz is used: the sprites' spin speed. */
    VECTOR spin;
} Model650Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_FT4 sprite;
    POLY_GT4 unused_strip;
    POLY_G3 fan;
    SVECTOR corners[4];
    /* Seventeen meridians of nine points on a sphere. */
    SVECTOR shell[17][9];
    SVECTOR circle[17];
    /* Shell trigger, then the restore request at flags[4]. */
    u8 flags[5];
    u8 sprite_count, shell_count, fan_count;
    /* Per-sprite numerator and divisor of the X and Y arcs. */
    s32 arc_x[8][2];
    s32 arc_y[8][2];
    Model650Element sprites[8];
    Model650Element shells[4];
    Model650Element fans[64];
} Model650State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model650ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CB20[2];
extern Model650Command D_8013CB58[];

s32 func_8013B004(u8 *context, s32 command);

#endif
