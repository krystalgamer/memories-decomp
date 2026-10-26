#ifndef MEMORIES_DECOMP_MODEL_IS_CAMERA_MOVE_COMPLETE_H
#define MEMORIES_DECOMP_MODEL_IS_CAMERA_MOVE_COMPLETE_H

#include "../types.h"

/* The camera-move completion gate at 0x8005A878.
 *
 * It returns nonzero when the camera move record D_800F2B20 (ModelCameraMove,
 * model.h) has `flags == 0`, and -- only when `arg` is zero -- additionally
 * requires `mode` or `follow_countdown` to be zero. The unit defines this one function
 * and nothing else.
 *
 * Model_UpdateAnimatedBattle, the model scene mode 15 controller
 * (src/game/model_scene_controllers.c), is the only source in the tree that
 * names it, and as a candidate it declared the prototype for itself because
 * this unit had no header to take it from. All twelve call sites pass 1, so there the test
 * reduces to `flags == 0` and the `arg == 0` path has no reader here; that is a
 * statement about this tree, not about the rest of the binary.
 * Follow/orbit motion can continue after flags clear; this is not a physical
 * stillness, effect-key queue, or request-permission query.
 *
 * The declaration is moved verbatim, `int` included: that is the spelling both
 * the definition and the former candidate already used. */
int Model_IsCameraMoveComplete(int arg);

#endif
