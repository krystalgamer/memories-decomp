#ifndef MEMORIES_DECOMP_MODEL_VARIANT677_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT677_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the effect radius, an unused
 * height, then the spark colour and the ring/burst colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model677Command;

/* One 0x4C-byte element: a spark, a ring or a burst. */
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
} Model677Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4], unknown_22;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    SVECTOR corners[4];
    SVECTOR ring_corners[4];
    SVECTOR burst_corners[4];
    SVECTOR ring_inner[17];
    SVECTOR ring_outer[17];
    SVECTOR ring_middle[17];
    SVECTOR wave_corners[4];
    SVECTOR cone_inner[17];
    SVECTOR cone_outer[17];
    SVECTOR cone_middle[17];
    /* [0] spark row cursor, [4] the finished latch. */
    u8 flags[5];
    u8 column_count, row_count;
    /* Texture-cell origins indexed by a ring's u. */
    u8 cell_u[8];
    u8 cell_v[8];
    u8 unknown_447;
    VECTOR offsets[5];
    VECTOR spread[5];
    Model677Element sparks[12][2];
    Model677Element rings[12][2];
    Model677Element bursts[12][2];
} Model677State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model677ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char D_8013D74C[];
#endif
extern GsIMAGE D_8013D764[11];
extern Model677Command D_8013D898[];

s32 func_8013B004(u8 *context, s32 command);

#endif
