#ifndef MEMORIES_DECOMP_MODEL_VARIANT636_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT636_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the arc radius and the sprite
 * half-size, then the streak and the sprite colours. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model636Command;

/* One 0xC-byte flight path of a streak: the forward reach, the frame of
 * the apex, then two sideways and two vertical arc factors. */
typedef struct {
    u16 reach, apex;
    s16 rise_scale, rise_divisor;
    s16 side_scale, side_divisor;
} Model636Path;

/* One 0x50-byte effect part placed through its own coordinate unit. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Set once a streak has landed; the ring count of a dome. */
    u16 stage;
    u16 unknown_12;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    /* A streak's launch point; a sprite's drift over its lifetime. */
    VECTOR target;
} Model636Part;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 radius, height;
    u16 tpage, unknown_10[2];
    u16 clut, unknown_16[3];
    GsGLINE line;
    POLY_FT4 sprite;
    /* The last 17 positions of each streak, newest first. */
    SVECTOR trail[17][8];
    /* Eight meridians of nine points of a dome. */
    SVECTOR dome[8][9];
    SVECTOR corners[4];
    /* Three launch cursors, then the finish flag at flags[4]. */
    u8 flags[5];
    u8 rows, cols, unknown_6FF;
    Model636Path paths[8];
    Model636Part streaks[8];
    Model636Part domes[8];
    Model636Part sprites[16][8];
} Model636State;

/* An eight-byte copy of the view metrics; the sprites face the camera. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model636ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CF80;
extern Model636Command D_8013CF9C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
