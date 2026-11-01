#ifndef MEMORIES_DECOMP_MODEL_VARIANT666_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT666_ENTRY_H

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
#include "../../psyq/libgte.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the bloom radius, the
 * spark height and two colours (blooms and sparks, then rings). */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model666Command;

/* One 0x54-byte element: a bloom, a spark or a ring. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Bloom frame counter, spark cursor and spark ring latch. */
    u16 frame, cursor, spawned, unknown_16;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} Model666Element;

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
    POLY_FT3 fan;
    GsGLINE line;
    /* The bloom square. */
    SVECTOR flare[4];
    /* The spark root and nine points round a quarter circle. */
    SVECTOR root;
    SVECTOR rim[9];
    /* The ring square. */
    SVECTOR ring[4];
    /* Flag [4] latches once the finale has been signalled. */
    u8 flags[5];
    u8 count, unknown_112, sparks_per_bloom;
    SVECTOR origin;
    Model666Element blooms[3];
    Model666Element sparks[16][3];
    Model666Element rings[16][3];
} Model666State;

/* An eight-byte copy of the view metrics. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model666ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char gModelVariant666_D1A4C[];
#endif
extern GsIMAGE gModelVariant666_D1A64[3];
extern Model666Command gModelVariant666_D1AB8[];

s32 ModelVariant666_HandleRequest(u8 *context, s32 command);

#endif
