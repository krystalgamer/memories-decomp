#ifndef MEMORIES_DECOMP_FUNC_8005A878_H
#define MEMORIES_DECOMP_FUNC_8005A878_H

#include "../types.h"

/* The camera-move gate at 0x8005A878 (src/game/func_8005A878.c, matching_c).
 *
 * It returns nonzero when the camera move record D_800F2B20 (ModelCameraMove,
 * model.h) has `flags == 0`, and -- only when `arg` is zero -- additionally
 * requires `mode` or `field_02` to be zero. The unit defines this one function
 * and nothing else.
 *
 * func_8004EB00, the model scene mode 15 controller
 * (src/candidates/func_8004EB00.c), is the only source in the tree that names
 * it, and it declared the prototype for itself because this unit had no header
 * to take it from. All eleven of its call sites pass 1, so there the test
 * reduces to `flags == 0` and the `arg == 0` path has no reader here; that is a
 * statement about this tree, not about the rest of the binary.
 *
 * The declaration is moved verbatim, `int` included: that is the spelling both
 * the definition and the candidate already used. */
int func_8005A878(int arg);

#endif
