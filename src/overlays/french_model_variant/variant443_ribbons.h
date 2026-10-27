#ifndef FRENCH_MODEL_VARIANT443_RIBBONS_H
#define FRENCH_MODEL_VARIANT443_RIBBONS_H

#include "../../types.h"
#include "variant400_ribbons.h"

typedef struct {
    u8 unknown00[0x40];
    u32 spread_start;
    u32 spread_end;
} Ribbons443Timing;

typedef struct {
    u8 unknown0000[0x3570];
    VECTOR origins[3];
} Ribbons443State;

#endif
