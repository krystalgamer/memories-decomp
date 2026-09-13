#ifndef MEMORIES_DECOMP_FUNC_800556E8_H
#define MEMORIES_DECOMP_FUNC_800556E8_H

#include "../types.h"

/* Per-frame animation tick for one model slot, selected by index into
 * D_800F2C40. It returns immediately when the slot is inactive, otherwise
 * advances or rewinds the frame counter and, at the end of an animation with
 * no hold pending, switches to the queued one. func_80059CE4 in
 * model_scene_states.c drives slots 0, 1 and 2 with it once per frame.
 *
 * The address-based name is kept: the defining unit describes the tick in
 * detail, but which subsystem owns the slot sequence is not settled by that
 * description alone. */
void func_800556E8(s32 index);

#endif
