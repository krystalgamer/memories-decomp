#ifndef MEMORIES_DECOMP_FUNC_800179F4_H
#define MEMORIES_DECOMP_FUNC_800179F4_H

#include "display_object.h"
#include "save_data.h"

/* The duel entry point, and the two banner display objects it builds.

   func_800179F4 allocates both from DisplayObject_AcquireSlot and hands each to
   DisplayObject_SelectOrderingTable1, whose parameter is DisplayObject *, which is what #3176
   established and what DuelScene_UpdateResultOutro's source already declared. The
   other consumers use the typed view. The Exodia candidate takes the guarded
   byte-pointer arm because its offset-based accesses are codegen-sensitive.

   The prototype is the one main_run_duel_and_library.c held as its own
   extern, which was the only declaration anywhere. */
void func_800179F4(void);

#ifdef D_8009B214_AS_BYTE_POINTER
extern u8 *D_8009B214;
#else
extern DisplayObject *D_8009B214;
#endif
#ifdef D_8009B21C_AS_BYTE_POINTER
extern u8 *D_8009B21C;
#else
extern DisplayObject *D_8009B21C;
#endif

/* Save-data windows selected while the duel scene starts. The no-opponent
 * path points them at the two 0x1000-byte halves of D_801D1200; the normal
 * path uses the buffers returned by the duel package transfer.
 * func_800218F0 indexes the two adjacent pointer words at B1D8/B1DC;
 * the bounded eight-byte pair remains GP-relative at -G8. That arm is
 * declared SaveDataState *, since every reach through it is a save-record
 * member; the scalar arm stays u8 * for func_800179F4, which only assigns
 * the two byte buffers into it. Both words
 * already have real backing in bss_image_after_viewport. This view does
 * not allocate over two separately defined C scalars. Existing consumers
 * retain their independent scalar linker identities. */
#ifdef DUEL_SAVE_WINDOWS_AS_PAIR
extern SaveDataState *D_8009B1D8[2];
#else
extern u8 *D_8009B1D8;
#endif
extern u8 *D_8009B1DC;

#endif
