#ifndef FRENCH_MODEL_VARIANT383_RIBBONS_H
#define FRENCH_MODEL_VARIANT383_RIBBONS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    SVECTOR points[2];
    PSXLONG projected[2];
    s32 angle[2];
    SVECTOR edges[2];
    PSXLONG edge_projected[2];
    s32 width[2];
    u8 unknown_40[0x1C];
    PSXLONG depth[2];
    s16 x_offset[2];
    s16 y_offset[2];
} Family383Ribbon;

typedef struct {
    u8 unknown_00[0x80];
    u8 outer;
    u8 unknown_81[3];
    u8 inner;
    u8 unknown_85[3];
} Family383RibbonColors;

typedef struct {
    u8 unknown_00[0xC];
    u16 count_0C;
} Family383RibbonConfig;

typedef struct {
    u8 unknown_0000[0x6C];
    Family383RibbonColors colors;
    Family383Ribbon ribbons[8];
    u8 unknown_0454[0xEE0];
    POLY_G3 triangle;
    u8 unknown_1350[0x194];
    s32 origin[3];
    u8 unknown_14F0[8];
    s32 delta[3];
    u8 unknown_1504[8];
    s32 view_direction[3];
    u8 unknown_1518[0xC];
    s32 flags;
    u8 unknown_1528[8];
    s32 step;
    u8 unknown_1534[4];
    Family383RibbonConfig *G32 config;
    u8 unknown_153C[0x10];
    s32 index_154C;
    s32 scale;
    u8 unknown_1554[0x24];
    s16 factor;
    u8 unknown_157A[2];
    s32 angle;
    s32 phase;
    u8 unknown_1584[0x10];
    s16 mode;
} Family383RibbonView;

void func_8013C204(u8 *context);
#endif
