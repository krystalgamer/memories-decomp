#ifndef MEMORIES_DECOMP_MODEL_VARIANT680_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT680_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the radius, the height, the
 * cone colour and the flash colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, flash_red, flash_green, flash_blue;
} Model680Command;

/* One 0x4C-byte effect record. */
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
} Model680Element;

/* One 0x20-byte glow record of the column grid. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, unknown_05;
    u16 elapsed, duration, unknown_0A;
    GsCOORDUNIT *G32 coord;
    VECTOR position;
} Model680Glow;

/* The work area: the command, the slot, the packets, the cone grid (five
 * meridians of five point pairs), the flash square, the inner and outer
 * star outlines (point pairs), the spark, burst and glow squares, the
 * flags, the counts, the spawn order, the flipbook cells, the flash offsets
 * per cone, the burst offsets, the cone, flash and spark records and the
 * [column][row] glow grid. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[4];
    u16 clut[4];
    u16 unknown_22;
    POLY_FT4 flat;
    POLY_GT4 shaded;
    SVECTOR grid[5][5][2];
    SVECTOR square[4];
    /* [inner/outer][point][pair] */
    SVECTOR band[2][6][2];
    SVECTOR flash[4];
    SVECTOR spark[4];
    SVECTOR glow[4];
    /* Cone cursor, glow column cursor, the glow latch and, in [4], the
     * signal latch. */
    u8 flags[5];
    u8 count;
    u8 rows, columns;
    u8 order[4];
    u8 cell_u[8];
    u8 cell_v[8];
    VECTOR ring[5][32];
    VECTOR burst[5];
    Model680Element cones[32];
    Model680Element flashes[32];
    Model680Element sparks[32];
    Model680Glow glows[8][6];
} Model680State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model680ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n"; the image keeps it in data because the
 * byte after its terminator is a nonzero alignment pad. */
extern char D_8013DAAC[];
#endif
extern GsIMAGE D_8013DAC4[11];
extern Model680Command D_8013DBF8[];

s32 func_8013B004(u8 *context, s32 command);

#endif
