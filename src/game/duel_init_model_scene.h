#ifndef MEMORIES_DECOMP_DUEL_INIT_MODEL_SCENE_H
#define MEMORIES_DECOMP_DUEL_INIT_MODEL_SCENE_H

#include "../types.h"

/* Runs the two model setup steps Model_ResetSceneView and
 * Model_ResetSceneState, then calls Model_LoadSlot with selector 2,
 * gRuntime_pSharedWorkBase, 0x63000, and 4. The callee uses the pointer
 * only as a non-null gate; this call does not load or read the payload
 * (notes/duel-package-unused-data.md).
 *
 * The name is kept address-based: the call sequence is legible but what the
 * selector and the trailing 4 choose is not established here, and nothing in
 * this unit names the buffer's contents.
 *
 * Duel_InitScene (src/game/duel_init_scene.c) is the only consumer and
 * reached it through a local extern that already agreed with this. */
void Duel_InitModelScene(void);

#endif
