#include "../types.h"

/* The eight bytes at 0x8009B058, the first of the three owners in the
 * 40-byte range the notes call the scattered tail. The movie pipeline's
 * block was carved in #3795 and the model transfer block in #3829, so this
 * completes the range.
 *
 * It holds one object, not two words. MainMenu_LoadPackageStage is its only
 * reader and it takes the whole thing at once, as
 *
 *     rect = *(RECT *)D_8009B058;
 *     LoadImage2(&rect, (u32 *)D_801DD000);
 *
 * so the eight bytes are a libgpu RECT. Read that way the values are
 * coherent: x 0, y 240, w 256, h 8, a 256-by-8 strip of VRAM sitting just
 * below a 240-line frame buffer, which is what a staging rectangle for
 * LoadImage2 should look like. Nothing names 0x8009B05C, and no reader
 * touches the second word on its own, so splat's single label for the pair
 * is the object rather than a label that ran long.
 *
 * The definition is still spelled as bytes. The declaration its reader
 * carries is `extern u8 D_8009B058[]`, and under -G8 an array is what keeps
 * that access off %gp_rel; a RECT is exactly eight bytes and would sit in
 * small data instead. That is measured rather than assumed: declaring it
 * `extern RECT D_8009B058` in MainMenu_LoadPackageStage and reading it
 * without the cast builds an executable of 0x1D07FC bytes against the
 * expected 0x1D0800. So the array spelling there is load bearing, and this
 * definition matches it rather than asserting a type the only consumer does
 * not use. The RECT is recorded here as what the bytes mean, not as how they
 * are declared.
 *
 * No linker alias covers this symbol, so unlike the two blocks after it
 * there is nothing to retire. */

u8 D_8009B058[8] __attribute__((section(".sdata"))) = {
    0x00,
    0x00,
    0xF0,
    0x00,
    0x00,
    0x01,
    0x08,
    0x00,
};
