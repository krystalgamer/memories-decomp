#ifndef MEMORIES_DECOMP_FUNC_8004E9A0_H
#define MEMORIES_DECOMP_FUNC_8004E9A0_H

#include "../psyq/libgpu.h"

/* Steps one of two VRAM copy animations, gated on D_800F569F[0] and selected
 * by the byte at D_8009AF88[0xA0]. Both arms build the same four-halfword RECT
 * at D_8009B468 and hand it to MoveImage; they differ in the source region and
 * in how the frame is derived from VSync(-1) -- case 1 cycles three 16-pixel
 * columns, case 4 indexes gModel_abImageCopyFrameRows. Any other selector
 * value does nothing.
 *
 * Declared in a header of its own because no subsystem owns the complete
 * operation. D_8009AF88 comes from model_graphics_state.h; the four
 * rectangle halfwords now use their native SDK type here, while the separate
 * animation gate remains centralized in unmatched.h.
 *
 * Model_UpdateScene in model_scene_states.c is the only caller and held the only
 * declaration. */
void func_8004E9A0(void);
extern RECT gModel_ImageCopyRect asm("D_8009B468");
/* Separate field aliases preserve the original gp-relative store schedule;
 * using the RECT base for every member makes GCC retain the y-field address
 * and synthesize the base before MoveImage. */
extern s16 gModel_ImageCopyRectY asm("D_8009B46A");
extern s16 gModel_ImageCopyRectW asm("D_8009B46C");
extern s16 gModel_ImageCopyRectH asm("D_8009B46E");

#endif
