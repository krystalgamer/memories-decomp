#ifndef MEMORIES_DECOMP_MODEL_VARIANT671_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT671_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/camera_view.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the effect radius, the
 * cone height and two colours (petal blooms, then rings). */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model671Command;

/* One 0x4C-byte element: a petal bloom or a ring. */
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
    VECTOR target;
} Model671Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_FT4 quad;
    POLY_GT3 fan;
    POLY_GT4 strip;
    /* The bloom square. */
    SVECTOR flare[4];
    /* Three cone points for each of eight petals in eight turns. */
    SVECTOR grid[3][8][8];
    /* Seventeen points round an inner and an outer circle. */
    SVECTOR rim[2][17];
    /* The ring square. */
    SVECTOR ring[4];
    /* Spawn cursor and, in [4], the halfway latch. */
    u8 flags[5];
    u8 count;
    SVECTOR origin;
    Model671Element blooms[32];
    Model671Element rings[32];
} Model671State;

/* An eight-byte copy of the view metrics. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model671ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char D_8013C674[];
#endif
extern GsIMAGE D_8013C68C[3];
extern Model671Command D_8013C6E0[];

s32 func_8013B004(u8 *context, s32 command);

#endif
