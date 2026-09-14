#ifndef MEMORIES_DECOMP_FUNC_8002FED8_H
#define MEMORIES_DECOMP_FUNC_8002FED8_H

#include "../types.h"
#include "../ygo_types.h"

struct DisplayObject;

/* Entry 1 of the scene-script record callback table D_80090CAC
 * (scene_script_record_callbacks.c). It advances a sine phase at +6 of
 * `state`, latched by bit 0x8000 of +4 on the first call. From it, it writes
 * a pulsing grey into the colour bytes (field_0C) of `color` -- the display
 * object func_8002FFD4 passes from the slot's own first word -- and a
 * brighter one into those of the display objects in scene-script slots 0
 * and 1. */
void func_8002FED8(SceneScriptSlot *state, struct DisplayObject *color);

#endif
