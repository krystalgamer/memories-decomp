#include "../../types.h"
#define INPUT_PADS_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x800ECD88-0x800ECDA8 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 D_800ECD88_slpm_86398[0x20];

#define VERSION_JAPAN
#include "../input_pads.c"
