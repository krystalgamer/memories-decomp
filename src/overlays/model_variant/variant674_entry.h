#ifndef MEMORIES_DECOMP_MODEL_VARIANT674_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT674_ENTRY_H

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

/* One 0xC-byte command row: the start delay, the ring radius, the ring
 * height, the ring colour and the dome colour. */
typedef struct {
    s16 delay;
    u16 radius, height;
    u8 red, green, blue, dome_red, dome_green, dome_blue;
} Model674Command;

/* One 0x50-byte effect record: a ring of strips or a dome at position. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    u16 ticks, unknown_12;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR growth;
} Model674Element;

/* The work area: the command, the slot, the packets, the flash square, the
 * upper, lower and middle ring outlines, the dome grid (17 meridians of 9
 * points), the inner, outer and middle cone outlines, the flags, and the
 * ring and dome records. */
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
    POLY_FT4 flat;
    POLY_GT4 shaded;
    SVECTOR square[4];
    SVECTOR ring_upper[17];
    SVECTOR ring_lower[17];
    SVECTOR ring_middle[17];
    SVECTOR dome[17][9];
    SVECTOR cone_inner[9];
    SVECTOR cone_outer[9];
    SVECTOR cone_middle[9];
    u8 flags[5];
    u8 count;
    u8 unknown_7DA[2];
    Model674Element rings[3];
    Model674Element domes[3];
} Model674State;

/* An eight-byte copy of the view metrics; the entry reads the yaw at +2 and
 * the pitch at +4. */
typedef struct {
    u8 bytes[8];
} Model674ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013CE70[5];
extern Model674Command D_8013CEFC[];

s32 func_8013B004(u8 *context, s32 command);

#endif
