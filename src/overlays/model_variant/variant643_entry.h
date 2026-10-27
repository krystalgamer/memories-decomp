#ifndef MEMORIES_DECOMP_MODEL_VARIANT643_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT643_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0xC-byte command row: the start delay, the ring radius, the
 * cone height and two colours (rings and band, then the sprite grid). */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model643Command;

/* One 0x50-byte element: a cone ring, the flat band or a grid sprite. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    /* Next grid row to spawn; only the first row's copy is used. */
    u8 cursor, unknown_11[3];
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR target;
} Model643Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3], unknown_1E;
    POLY_GT4 strip;
    POLY_G4 band;
    POLY_FT4 quad;
    /* Seventeen inner and outer points round the cone. */
    SVECTOR cone[17][2];
    /* Seventeen inner and outer points round the flat band. */
    SVECTOR ring[17][2];
    SVECTOR corner[4];
    /* [0] latches the grid start, [4] the finished latch. */
    u8 flags[5];
    u8 cone_count, band_count, rows, columns;
    Model643Element cones[4];
    Model643Element bands[1];
    /* [row][column] */
    Model643Element grid[8][16];
} Model643State;

/* An eight-byte copy of the view metrics. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model643ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char D_8013D298[];
#endif
extern GsIMAGE D_8013D2B0[2];
extern Model643Command D_8013D2E8[];

s32 func_8013B004(u8 *context, s32 command);

#endif
