#ifndef MEMORIES_DECOMP_MODEL_VARIANT667_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT667_ENTRY_H

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

#include "../../game/model_copy_slot_position.h"

/* One 0xC-byte command row: the start delay, the quad and dome sizes, then
 * the ribbon colour and the sprite colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 color[6];
} Model667Command;

/* One 0x50-byte element: a ribbon, a sprite, a fan or a dome. */
typedef struct {
    u8 active;
    u8 color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u8 done, count, unknown_12[2];
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_40;
} Model667Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[3];
    u16 clut[3];
    u16 unknown_1E;
    POLY_G3 fan;
    POLY_G4 quad;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    /* Two edges of each ribbon's trail: [edge][point][ribbon]. */
    SVECTOR trail[2][64][2];
    SVECTOR corners[4];
    SVECTOR prev[2];
    SVECTOR heads[2][2];
    SVECTOR fan_rim[17];
    SVECTOR cap[8];
    SVECTOR ring[17];
    SVECTOR outer[17];
    u8 flags[5];
    u8 rows, cols, sprite_count, fan_count, dome_count;
    u8 cell_u[9], cell_v[9];
    /* Filled by Model_CopySlotPosition; the fourth halfword is unused. */
    s16 origin[4];
    Model667Element ribbons[2];
    Model667Element sprites[16];
    Model667Element fans[16];
    Model667Element domes[32];
} Model667State;

/* An eight-byte copy of the view metrics; the entry reads two angles. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model667ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE gModelVariant667_D2E14[3];
extern Model667Command gModelVariant667_D2E68[];

s32 ModelVariant667_HandleRequest(u8 *context, s32 command);

#endif
