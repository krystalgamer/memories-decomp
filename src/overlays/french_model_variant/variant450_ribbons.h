#ifndef FRENCH_MODEL_VARIANT450_RIBBONS_H
#define FRENCH_MODEL_VARIANT450_RIBBONS_H
#include "../../types.h"
#include "variant450_bands.h"

typedef struct {
    s32 completed;
    VECTOR target;
    VECTOR delta;
} Model450RibbonTail;

typedef struct {
    SVECTOR points[9];
    Model450Screen screen[9];
    s32 angle[9];
    SVECTOR edges[9];
    Model450Screen edge_screen[9];
    s32 width[9];
    CVECTOR color;
    CVECTOR secondary_color;
    u8 unknown_128[0x60];
    s32 depth[9];
    s16 x_offset[9];
    s16 y_offset[9];
    s32 phase[9];
    Model450RibbonTail tail;
} Model450Ribbon;

typedef struct {
    u8 unknown_0000[0x270];
    Model450Ribbon ribbons[6];
    u8 unknown_0F00[0x32E0];
    POLY_FT4 packets[2];
    u8 unknown_4230[0x38];
    s32 origin[3];
    u8 unknown_4274[0x1C];
    s32 view_direction[3];
    u8 unknown_429C[0xC];
    s32 flags;
    u8 unknown_42AC[8];
    s32 frame_step;
    u8 unknown_42B8[0x16];
    s16 radius;
    u8 unknown_42D0[0x14];
    s32 ripple;
    s32 ripple2;
    u8 unknown_42EC[0xC];
    s32 state;
} Model450RibbonView;

void func_8013BDD8(u8 *context);
#endif
