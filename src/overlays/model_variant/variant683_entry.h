#ifndef MEMORIES_DECOMP_MODEL_VARIANT683_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT683_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_state_setters.h"
#include "../../game/camera_view.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libgs.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One 0x10-byte command row: the start delay, the radius, the height, the
 * orb colour, the screen-fade colour and the burst colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 orb[3];
    u8 fade[3];
    u8 burst[3];
    u8 unknown_0F;
} Model683Command;

/* One 0x4C-byte effect record; `spin.vx` counts drawn frames. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR spin;
} Model683Element;

/* The work area: the command, the slot, the packets, the orb square, the
 * dome grid ([meridian][parallel]), the inner, outer and middle ring
 * outlines, the burst and ray squares, the flags, the counts, the other
 * slot's position, the ray offsets, the orbs, the screen fade and the
 * bursts. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s32 direction;
    s16 delay;
    u16 radius, height;
    u16 tpage[6];
    u16 clut[6];
    u16 unknown_2A;
    POLY_FT4 flat;
    POLY_GT4 shaded;
    POLY_F4 fill;
    SVECTOR square[4];
    SVECTOR grid[17][9];
    SVECTOR inner[17];
    SVECTOR outer[17];
    SVECTOR middle[17];
    SVECTOR burst[4];
    SVECTOR ray[4];
    /* Orb cursor and, in [1], the signal latch. */
    u8 flags[6];
    u8 count;
    u8 burst_count;
    SVECTOR origin;
    VECTOR offsets[32];
    Model683Element orbs[96];
    Model683Element fade;
    Model683Element bursts[3];
} Model683State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Model683ViewSnapshot;

extern MATRIX D_800FE168;
#ifndef VERSION_FRENCH
/* "E_DATA size = %x byte\n". */
extern char gModelVariant683_D2514[];
#endif
extern GsIMAGE gModelVariant683_D252C[6];
extern Model683Command gModelVariant683_D25D4[];

s32 ModelVariant683_HandleRequest(u8 *context, s32 command);

#endif
