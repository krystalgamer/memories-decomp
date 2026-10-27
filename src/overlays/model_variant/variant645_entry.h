#ifndef MEMORIES_DECOMP_MODEL_VARIANT645_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT645_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the tube radius and length,
 * then the tube colour and the halo colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model645Command;

/* One 0x4C-byte element: a tube puff left at a model joint, or a halo
 * placed through its own coordinate unit. */
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
    VECTOR unknown_3C;
} Model645Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[2], unknown_16;
    u16 clut[2], unknown_1C[2];
    POLY_GT4 strip;
    POLY_FT4 sprite;
    POLY_G3 fan;
    /* The tube's two end rings, sixteen segments around. */
    SVECTOR tube[17][2];
    /* The halo's inner and outer band, 32 segments around. */
    SVECTOR band[33][2];
    /* The halo's spinning blades: inner and outer points, 16 around. */
    SVECTOR blades[17][2];
    /* Puff cursor, then the halo stage. */
    u8 flags[5];
    u8 puff_count, halo_count, unknown_4CF;
    /* The joint matrix each puff was left with. */
    MATRIX joints[64];
    Model645Element puffs[64];
    Model645Element halos[8];
} Model645State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model645ViewSnapshot;

/* A blade corner and its rotated copy. */
typedef struct {
    SVECTOR in, out;
} Model645Corner;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013FEDC[2];
extern Model645Command D_8013FF14[];

s32 func_8013B004(u8 *context, s32 command);

#endif
