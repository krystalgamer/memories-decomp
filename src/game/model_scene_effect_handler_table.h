#ifndef MEMORIES_DECOMP_MODEL_SCENE_EFFECT_HANDLER_TABLE_H
#define MEMORIES_DECOMP_MODEL_SCENE_EFFECT_HANDLER_TABLE_H

#include "../types.h"

/* The model effect handler table at 0x800114E8: four functions of (s32, s32)
 * returning s32.
 *
 * Model_UpdateAnimatedBattle, the model scene mode 15 controller
 * (src/game/model_scene_controllers.c), is the only source in the tree that
 * names it, and as a candidate it declared the symbol for itself because the
 * unit had no header to take it from. No C source defines the table; it is
 * data the listing reaches and nothing here initialises.
 *
 * Its four retail words are, in order, Model_UpdateBurstEffect (rings,
 * flash, dust, sparks, embers and smoke), Model_UpdateSubdividedEffect (the
 * subdivided geometry effect), Model_UpdateDiscEffect (the 67-point disc) and
 * Model_UpdateSparkEffect (the spark burst). Each handler's unit names the
 * table in its prose without declaring it.
 *
 * The four-entry bound is corroborated independently of the declaration. The
 * one consumer copies the whole table into a local array in a single
 * assignment through ModelSlotS32Quad (model.h):
 *
 *     *(ModelSlotS32Quad *)handlers = *(ModelSlotS32Quad *)gModel_apfnEffectHandlers;
 *
 * and that type carries a sizeof == 0x10 assertion, which is four pointers.
 *
 * Unlike the duel scene-state globals, this symbol is not gp-relative: the
 * matched controller reaches it with lui %hi / addiu %lo at its entry and
 * nothing else, zero gp-relative accesses.
 * That is a property of this symbol rather than of the function -- the same
 * body reaches gModel_pBackgroundRecord, gModel_wPresentationBgmCommand and gModel_bPresentationPhase through %gp_rel -- so the
 * declaration is moved verbatim, array form and all, because the form is what
 * the addressing depends on. */
extern s32 (*G32 gModel_apfnEffectHandlers[4])(s32, s32);

#endif
