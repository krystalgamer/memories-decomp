#ifndef MEMORIES_DECOMP_DUEL_RITUAL_CONTROLLER_H
#define MEMORIES_DECOMP_DUEL_RITUAL_CONTROLLER_H

#include "../types.h"
#include "display_object.h"

/* Shared ritual result id: func_8002622C and func_800262D4 retain the
 * signed low halfword returned by Duel_CheckRitual. */
extern s16 gDuel_RitualResultCardId;

/* Scratch display object retained across controller phases. Other duel
 * animations reuse this pointer; it does not own a private object pool. */
extern DisplayObject *gDuel_pCardPreviewObject;

/* The ritual caller sign-extends its result; placement passes its masked
 * card ID. The full word supplies the existing library-flag offset, not
 * an object address. The helper preserves modulo-32-bit addition. */
void Duel_MarkCreatedCardKnown(s32 card_id);
#endif
