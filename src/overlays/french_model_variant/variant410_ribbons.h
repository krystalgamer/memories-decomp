#ifndef FRENCH_MODEL_VARIANT410_RIBBONS_H
#define FRENCH_MODEL_VARIANT410_RIBBONS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Family410Screen;

typedef struct {
    SVECTOR a[17];
    Family410Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Family410Screen sb[17];
    s32 width[17];
    CVECTOR inner;
    CVECTOR outer;
    u8 unknown_228[0x28];
    s32 depth[17];
    s16 ox[17];
    s16 oy[17];
    s32 progress[17];
} Family410Ribbon;

typedef struct {
    u8 unknown_0000[0x2FC];
    Family410Ribbon ribbons[9];
    u8 unknown_1EF8[0x174];
    POLY_G4 quad;
    u8 unknown_2090[0x100];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_21A4[8];
    s32 view_direction[3];
    u8 unknown_21B8[0x18];
    s32 step;
    u8 unknown_21D4[0x14];
    s32 animation_angle;
    s32 animation_angle2;
    s32 phase;
} Family410RibbonView;

void func_8013BF58(u8 *context);
#endif
