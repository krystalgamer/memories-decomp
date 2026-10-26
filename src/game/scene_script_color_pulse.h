#ifndef MEMORIES_DECOMP_SCENE_SCRIPT_COLOR_PULSE_H
#define MEMORIES_DECOMP_SCENE_SCRIPT_COLOR_PULSE_H

#include "../types.h"
#include "../ygo_types.h"

struct DisplayObject;

/* Entry 1 of the scene-script record callback table gSceneScript_RecordCallbacks
 * (scene_script_record_callbacks.c). It advances a sine phase at +6 of
 * `state`, latched by bit 0x8000 of +4 on the first call. From it, it writes
 * a pulsing grey into the colour bytes (rgb) of `color` -- the display
 * object SceneScript_UpdateSlots passes from the slot's own first word -- and a
 * separately computed component into the independently loaded display
 * objects in scene-script slots 0 and 1. These objects may alias `color`. */
void SceneScript_UpdateColorPulse(SceneScriptSlot *state, struct DisplayObject *color);

#endif
