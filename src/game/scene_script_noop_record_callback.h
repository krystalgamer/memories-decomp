#ifndef MEMORIES_DECOMP_SCENE_SCRIPT_NOOP_RECORD_CALLBACK_H
#define MEMORIES_DECOMP_SCENE_SCRIPT_NOOP_RECORD_CALLBACK_H

#include "../types.h"

/* Entry 0 of the scene-script record callback table D_80090CAC
 * (scene_script_record_callbacks.c). It is empty: a record whose selector
 * byte is 0 has no per-frame work. */
void SceneScript_NoOpRecordCallback(void);

#endif
