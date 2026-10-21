#ifndef FRENCH_MODEL_VARIANT373_RIBBONS_H
#define FRENCH_MODEL_VARIANT373_RIBBONS_H
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
} Family373Ribbon;

typedef struct {
    u8 unknown_00[0x80];
    u8 outer;
    u8 unknown_81[3];
    u8 inner;
    u8 unknown_85[3];
} Family373RibbonColors;

typedef struct {
    u8 unknown_00[0xC];
    u16 count_0C;
} Family373RibbonConfig;

typedef struct {
    u8 unknown_0000[0xFD0];
    Family373RibbonColors colors;
    Family373Ribbon ribbons[8];
    u8 unknown_13B8[0xA00];
    POLY_G3 triangle;
    u8 unknown_1DD4[0x5F0];
    s32 origin[3];
    u8 unknown_23D0[8];
    s32 delta[3];
    u8 unknown_23E4[8];
    s32 view_direction[3];
    u8 unknown_23F8[0xC];
    s32 flags;
    u8 unknown_2408[8];
    s32 step;
    u8 unknown_2414[4];
    Family373RibbonConfig *G32 config;
    u8 unknown_241C[0x10];
    s32 index_242C;
    u8 unknown_2430[0x14];
    s32 scale;
    u8 unknown_2448[0x2C];
    s16 factor;
    u8 unknown_2476[2];
    s32 angle;
    u8 unknown_247C[8];
    s32 phase;
    u8 unknown_2488[0x14];
    s16 mode;
} Family373RibbonView;

void func_8013D784(u8 *context);
#endif
