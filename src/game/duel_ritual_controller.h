#ifndef MEMORIES_DECOMP_DUEL_RITUAL_CONTROLLER_H
#define MEMORIES_DECOMP_DUEL_RITUAL_CONTROLLER_H

#include "../types.h"
#include "display_object.h"

/* Shared ritual result id: func_8002622C and func_800262D4 retain the
 * signed low halfword returned by Duel_CheckRitual. */
extern s16 gDuel_RitualResultCardId;

/* Scratch display object retained across controller phases. Other duel
 * animations reuse this pointer; it does not own a private object pool. */
extern DisplayObject *G32 gDuel_pCardPreviewObject;

/* Measured prefix only, not the full size of the image-backed workspace.
 * The controller writes a RECT at +8 and reads source X/Y at +0x28/+0x2A.
 * The last halfword ends at 0x800EA154, before the next mapped label
 * gLibrary_State. ApplyRitual borrows the independently identified preview
 * resource slot 1 through this same address, without widening this declaration.
 * See notes/ritual-controller-storage.md. */
#define DUEL_RITUAL_IMAGE_PREFIX_HALFWORDS 22
extern u16 D_800EA128[DUEL_RITUAL_IMAGE_PREFIX_HALFWORDS];

/* The ritual caller sign-extends its result; placement passes its masked
 * card ID. The full word supplies the existing library-flag offset, not
 * an object address. The helper preserves modulo-32-bit addition. */
void Duel_MarkCreatedCardKnown(s32 card_id);
#endif
