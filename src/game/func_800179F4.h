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

#endif
