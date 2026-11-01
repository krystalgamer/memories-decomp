#ifndef MEMORIES_DECOMP_DUEL_INIT_SCENE_H
#define MEMORIES_DECOMP_DUEL_INIT_SCENE_H

#include "display_object.h"
#include "save_data.h"

/* The duel entry point, and the two banner display objects it builds.

   Duel_InitScene allocates both from DisplayObject_AcquireSlot and hands each to
   DisplayObject_SelectOrderingTable1, whose parameter is DisplayObject *, which is what #3176
   established and what DuelScene_UpdateResultOutro's source already declared. The
   other consumers use the typed view. The Exodia candidate takes the guarded
   byte-pointer arm because its offset-based accesses are codegen-sensitive.

   The prototype is the one main_run_duel_and_library.c held as its own
   extern, which was the only declaration anywhere. */
void Duel_InitScene(void);

/* The two status sprites Duel_InitScene creates: the terrain icon at (12, 24),
   whose variant is gDuel_bTerrain and which DuelEffect_ApplyTerrain re-selects,
   and the panel at (280, 32) that Duel_DrawLifePointsAndDeckCounts positions
   its numbers against. The result outro slides both away. */
#ifdef D_8009B214_AS_BYTE_POINTER
extern u8 *G32 gDuel_pTerrainIcon;
#else
extern DisplayObject *G32 gDuel_pTerrainIcon;
#endif
#ifdef D_8009B21C_AS_BYTE_POINTER
extern u8 *G32 gDuel_pLifePointsPanel;
#else
extern DisplayObject *G32 gDuel_pLifePointsPanel;
#endif

/* Save-data windows selected while the duel scene starts. The no-opponent
 * path points them at the two 0x1000-byte halves of D_801D1200; the normal
 * path uses the buffers returned by the duel package transfer.
 * func_800218F0 indexes the two adjacent pointer words at B1D8/B1DC;
 * the bounded eight-byte pair remains GP-relative at -G8. That arm is
 * declared SaveDataState *, since every reach through it is a save-record
 * member; the scalar arm stays u8 * for Duel_InitScene, which only assigns
 * the two byte buffers into it. Both words
 * already have real backing in bss_image_after_viewport. This view does
 * not allocate over two separately defined C scalars. Existing consumers
 * retain their independent scalar linker identities. The pair arm indexes
 * the first word by duel side, so its element 1 is gDuel_pSide1SaveData;
 * DuelResult updates the winner's wins, the loser's losses and side 0's
 * starchips through it. */
#ifdef DUEL_SAVE_WINDOWS_AS_PAIR
extern SaveDataState *G32 gDuel_pSide0SaveData[2];
#else
extern u8 *G32 gDuel_pSide0SaveData;
#endif
extern u8 *G32 gDuel_pSide1SaveData;

#endif
