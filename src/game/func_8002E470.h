#ifndef MEMORIES_DECOMP_FUNC_8002E470_H
#define MEMORIES_DECOMP_FUNC_8002E470_H

#include "../types.h"

/* D_80090C50 handler: enters a script image scene. Resets the viewport pair,
 * releases the three image slots through func_8002E00C, then takes a 16-bit
 * image id from the stream -- and when bit 15 is set, two further halfwords
 * that seed the viewport, advancing the cursor past all six bytes. */
void func_8002E470(void);

#endif
