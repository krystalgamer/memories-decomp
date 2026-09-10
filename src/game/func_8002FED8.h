#ifndef MEMORIES_DECOMP_FUNC_8002FED8_H
#define MEMORIES_DECOMP_FUNC_8002FED8_H

#include "../types.h"

/* Entry 1 of the scene-script record callback table D_80090CAC
 * (scene_script_record_callbacks.c). It advances a sine phase at +6 of
 * `state`, latched by bit 0x8000 of +4 on the first call. From it, it writes
 * a pulsing grey into bytes 0xC to 0xE of `color` and a brighter one into
 * the same bytes of the display objects in scene-script slots 0 and 1. */
void func_8002FED8(u8 *state, u8 *color);

#endif
