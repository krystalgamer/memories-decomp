#ifndef FRENCH_MODEL_VARIANT450_COILS_H
#define FRENCH_MODEL_VARIANT450_COILS_H
#include "../../types.h"
#include "variant450_bands.h"
#include "../model_variant/model_variant.h"
#include "../spanish_model_variant/variant450_quads.h"

typedef struct {
    SVECTOR points[17];
    Model450Screen screen[17];
    s32 angle[17];
    SVECTOR edges[17];
    Model450Screen edge_screen[17];
    s32 width[17];
    u8 unknown_220[0x44];
    CVECTOR color[17];
    s32 field_2A8;
    s32 field_2AC[17];
    s32 depth[17];
    s16 x_offset[17];
    s16 y_offset[17];
} Model450Coil;

typedef struct {
    u8 unknown_0000[0x3598];
    Variant450QuadGroup groups[7];
    Model450Coil coils[2];
    u8 unknown_40E8[0xA8];
    POLY_FT4 packet;
    u8 unknown_41B8[0xBC];
    SVECTOR origin;
    u8 unknown_427C[0x14];
    s32 view_direction[3];
    u8 unknown_429C[0xC];
    s32 flags;
    u8 unknown_42AC[0x28];
    s32 length;
    s32 rotation[3];
    u8 unknown_42E4[0x14];
    s32 state;
} Model450CoilView;

void func_8013E10C(u8 *context);
#endif
