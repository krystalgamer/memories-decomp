#ifndef MEMORIES_DECOMP_MODEL_VARIANT635_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT635_ENTRY_H

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

/* One 0x10-byte command row: the start delay, the dome radius and height,
 * then the dome, the burst and the ray colours. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[9];
    u8 unknown_0F;
} Model635Command;

/* One 0x50-byte effect part placed through its own coordinate unit. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* A frame counter for the domes and bursts, a shrink/grow toggle for
     * the comets. */
    u16 stage;
    /* Set once a leading dome has launched its comet. */
    u16 launched;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    /* The domes' drift per frame; the comets keep their launch point in
     * vy/vz and their sideways drift in vx. */
    VECTOR velocity;
} Model635Part;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_12;
    u16 clut[2];
    u8 unknown_18[4];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    GsGLINE line;
    /* Nine meridians of six rings of a dome. */
    SVECTOR dome[9][6];
    /* A 17x17 grid bent over a sphere that carries the comet sprite sheet. */
    SVECTOR orb[17][17];
    /* A cone of nine meridians and its copy spun about Y each frame. */
    SVECTOR cone[9][9];
    SVECTOR spun[9][9];
    /* Eight meridians of nine points for the flash rays. */
    SVECTOR rays[8][9];
    /* The launch cursor, then the finish flag at flags[4]. */
    u8 flags[5];
    u8 copies, dome_count, burst_count;
    /* The comets' current and previous positions. */
    VECTOR trail[2][3];
    Model635Part domes[5][3];
    Model635Part comets[3];
    Model635Part bursts[2][3];
    Model635Part flashes[3];
} Model635State;

/* An eight-byte copy of the view metrics, read but unused. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model635ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013EBE8[2];
extern Model635Command D_8013EC20[];

s32 func_8013B004(u8 *context, s32 command);

#endif
