#include "../../types.h"
/* This build has no raw pad buffers: the shared source defines only the
 * repeat timers here, at 0x800ECD88-0x800ECDA8 (#2602). */

#define VERSION_JAPAN
#include "../input_pads.c"
