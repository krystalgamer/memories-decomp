#ifndef FRENCH_MODEL_VARIANT389_RIBBONS_H
#define FRENCH_MODEL_VARIANT389_RIBBONS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Ribbon389Screen;

typedef struct {
    SVECTOR a[17];
    Ribbon389Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Ribbon389Screen sb[17];
    s32 width[17];
    u8 unknown_220[0x44];
    CVECTOR color[17];
    u8 unknown_2A8[0x28];
    PSXLONG flag[17];
    s32 depth[17];
    s16 ox[17];
    s16 oy[17];
    s32 progress[17];
    s32 done;
} Ribbon389;

typedef struct {
    Ribbon389 ribbons[8];
    ModelVariantSheetSet sheets[16];
    u8 unknown_28E0[0xA8];
    POLY_GT4 quad;
    u8 unknown_29BC[0xA0];
    MATRIX transforms[8];
    VECTOR positions[8];
    VECTOR directions[8];
    u8 unknown_2C5C[0x14];
    s32 view_direction[3];
    u8 unknown_2C7C[0x18];
    s32 step;
    u8 unknown_2C98[0x34];
    s32 angle_a;
    s32 angle_b;
    u8 unknown_2CD4[0xC];
    s32 phase;
    s16 mode;
} Ribbons389State;

#endif
