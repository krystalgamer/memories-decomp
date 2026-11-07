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
extern DisplayObject *gDuel_pTerrainIcon;
extern DisplayObject *gDuel_pLifePointsPanel;

/* Save-data windows selected while the duel scene starts, one per duel side.
 * The no-opponent path points them at the two 0x1000-byte halves of
 * gSaveData_aPairWorkspace; the normal path uses the buffers returned by the
 * duel package transfer. They are two adjacent scalars, each defined once in
 * duel_scene.c: Duel_InitScene stores each through its own
 * GP-relative address, which one two-entry array would fold into a shared
 * base register.
 *
 * DuelResult instead indexes the pair by side, and retail reloads side 0's
 * pointer after each starchip store. GCC 2.8 only reloads a value that a
 * store through a structure field may alias when the read itself is a
 * structure or array reference, so DUEL_SAVE_WINDOW reads the two adjacent
 * pointers as the pair they are. */
extern SaveDataState *gDuel_pSide0SaveData;
extern SaveDataState *gDuel_pSide1SaveData;

typedef struct DuelSaveWindows {
    SaveDataState *side[2];
} DuelSaveWindows;

#define DUEL_SAVE_WINDOW(side_index) \
    (((DuelSaveWindows *)&gDuel_pSide0SaveData)->side[side_index])

#endif
