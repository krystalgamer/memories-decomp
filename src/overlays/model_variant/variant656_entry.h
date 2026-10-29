#ifndef MEMORIES_DECOMP_MODEL_VARIANT656_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT656_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the size and height of the
 * effect, then the ring/shell colour and the sprite colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model656Command;

/* One 0x4C-byte element: a sprite, cone or dome placed through its own
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
    VECTOR growth;
} Model656Element;

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
    POLY_GT4 strip;
    u32 unknown_7C[5];
    SVECTOR corners[4];
    /* Nine rings of eight points widening along a cone. */
    SVECTOR cone[9][8];
    SVECTOR rim[9];
    SVECTOR inner[9];
    SVECTOR outer[9];
    /* Nine rows of a dome's inner and outer points. */
    SVECTOR dome_inner[9][8];
    SVECTOR dome_outer[9][8];
    /* Spawn cursor, then the finished latch at flags[4]. */
    u8 flags[5];
    u8 count, dome_count, unknown_84F;
    MATRIX joints[16];
    Model656Element sprites[16];
    Model656Element cones[16];
    Model656Element domes[16];
} Model656State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model656ViewSnapshot;

/* The work-size string; slot-1 images carry a nonzero pad byte after it. */
extern char D_8013D3C8[];
extern MATRIX D_800FE168;
extern GsIMAGE D_8013D3E0[2];
extern Model656Command D_8013D418[];

s32 func_8013B004(u8 *context, s32 command);

#endif
