#ifndef FRENCH_MODEL_VARIANT450_BANDS_H
#define FRENCH_MODEL_VARIANT450_BANDS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Model450Screen;

typedef struct {
    SVECTOR points[9];
    Model450Screen screen[9];
    s32 angle[9];
    SVECTOR edges[9];
    Model450Screen edge_screen[9];
    s32 width[9];
    CVECTOR color;
    u8 unknown_124[0x74];
    s32 depth[9];
    s16 x_offset[9];
    s16 y_offset[9];
    u8 unknown_1E0[0x28];
} Model450Band;

typedef struct {
    u8 unknown_0000[0xF00];
    Model450Band bands[3];
    u8 unknown_1518[0x2CC8];
    POLY_FT4 packets[2];
    u8 unknown_4230[0x38];
    s32 origin[3];
    u8 unknown_4274[8];
    s32 delta[3];
    u8 unknown_4288[8];
    s32 view_direction[3];
    u8 unknown_429C[0xC];
    s32 flags;
    u8 unknown_42AC[0x22];
    s16 radius;
} Model450BandView;

void func_8013C794(u8 *context);
#endif
