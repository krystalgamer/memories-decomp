#ifndef YUGIOH_GAME_GRAPHICS_FRAME_H
#define YUGIOH_GAME_GRAPHICS_FRAME_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

/* The per-frame step multiplier. graphics_frame.c sets it to D_8009B0C1 + 1
 * once a frame and main_init.c seeds it at 1; every other consumer scales a
 * motion delta by it, which is why a dropped frame moves things twice as far.
 *
 * It is read at three widths across the tree and the width is a codegen
 * input, not a style choice, so the arms:
 *
 *   _IS_HALFWORD -- read into halfword arithmetic (lhu, not lw)
 *   _IS_VOLATILE -- main_init.c seeds it and must not have the store folded
 *   _IN_DATA     -- out of small data at the compiler, with its true width
 *                   at that call site
 *
 * display_object_fade_callbacks.c needs the plain and the volatile spelling
 * in one translation unit, which no single arm can give, so it keeps its
 * asm("D_8009B0D8") alias -- a second name for one symbol, not a duplicate
 * declaration. */
#ifdef D_8009B0D8_IN_DATA
extern u8 D_8009B0D8 __attribute__((section(".data")));
#elif defined(D_8009B0D8_IS_HALFWORD)
extern u16 D_8009B0D8;
#elif defined(D_8009B0D8_IS_VOLATILE)
extern volatile s32 D_8009B0D8;
#else
extern s32 D_8009B0D8;
#endif

extern DISPENV gGraphics_DispEnv;

/* The tint colour, three consecutive bytes with the components in address
 * order blue, green, red.  func_8005B8A0 and func_8005BB7C pass them straight
 * to ClearImage(RECT *, u8 r, u8 g, u8 b) as r = D_8009B144, g = D_8009B143,
 * b = D_8009B142, which is what fixes the roles; graphics_frame.c copies the
 * same three into the display list at 0x19/0x1A/0x1B.
 *
 * func_80015310.c is not converted, and its functions.csv row says why: the
 * three are DEFINED rather than declared there so the assembler resolves them
 * gp-relative and supplies the three load-delay nops in the tint copy.  That
 * file still builds byte-identical with the declaration below visible ahead
 * of its definition, which is the only claim made here about the two.
 *
 *   _IN_DATA      -- out of small data at the compiler
 *   _IS_AGGREGATE -- unsized array, read as [0]
 */
#ifdef D_8009B142_IN_DATA
extern u8 D_8009B142 __attribute__((section(".data")));
extern u8 D_8009B143 __attribute__((section(".data")));
extern u8 D_8009B144 __attribute__((section(".data")));
#elif defined(D_8009B142_IS_AGGREGATE)
extern u8 D_8009B142[];
extern u8 D_8009B143[];
extern u8 D_8009B144[];
#else
extern u8 D_8009B142;
extern u8 D_8009B143;
extern u8 D_8009B144;
#endif

/* Viewport scroll offset in pixels, signed: func_8002A660 subtracts it from a
   sprite position and func_80040588 subtracts it from a primitive's, and both
   results have to be able to go negative.

   Files that reach these through `__attribute__((section(".data")))`, or as an
   unsized or [4] array, are deliberately not converted -- those spellings
   change how the address is materialised, not just how the value reads. */
extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;

void Graphics_SyncFrame(void);
void Graphics_BeginFrame(void);

#endif
