#ifndef MEMORIES_DECOMP_FUNC_800179F4_H
#define MEMORIES_DECOMP_FUNC_800179F4_H

#include "display_object.h"

/* The duel entry point, and the two banner display objects it builds.

   func_800179F4 allocates both from func_800400AC and hands each to
   func_80042918, whose parameter is DisplayObject *, which is what #3176
   established and what func_80020F4C's source already declared. The
   other three consumers spelled them u8 *; they are declared once here
   instead.

   The prototype is the one main_run_duel_and_library.c held as its own
   extern, which was the only declaration anywhere. */
void func_800179F4(void);

extern DisplayObject *D_8009B214;
extern DisplayObject *D_8009B21C;

/* Save-data windows selected while the duel scene starts. The no-opponent
 * path points them at the two 0x1000-byte halves of D_801D1200; the normal
 * path uses the buffers returned by the duel package transfer. */
extern u8 *D_8009B1D8;
extern u8 *D_8009B1DC;

#endif
