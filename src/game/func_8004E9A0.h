#ifndef MEMORIES_DECOMP_FUNC_8004E9A0_H
#define MEMORIES_DECOMP_FUNC_8004E9A0_H

/* Steps one of two VRAM copy animations, gated on D_800F569F[0] and selected
 * by the byte at D_8009AF88[0xA0]. Both arms build the same four-halfword RECT
 * at D_8009B468 and hand it to MoveImage; they differ in the source region and
 * in how the frame is derived from VSync(-1) -- case 1 cycles three 16-pixel
 * columns, case 4 indexes gModel_abImageCopyFrameRows. Any other selector
 * value does nothing.
 *
 * Declared in a header of its own because no subsystem owns the complete
 * operation. D_8009AF88 comes from model_graphics_state.h; the five otherwise
 * homeless linker-assigned globals are centralized in unmatched.h without
 * assigning them broader ownership.
 *
 * func_80059CE4 in model_scene_states.c is the only caller and held the only
 * declaration. */
void func_8004E9A0(void);

#endif
