#ifndef MEMORIES_DECOMP_DUEL_RITUAL_CONTROLLER_H
#define MEMORIES_DECOMP_DUEL_RITUAL_CONTROLLER_H

#include "../types.h"
#include "display_object.h"

/* Shared ritual result id: func_8002622C and func_800262D4 retain the
 * signed low halfword returned by Duel_CheckRitual. */
extern s16 D_8009B1A0;

/* Scratch display object retained across controller phases. Other duel
 * animations reuse this pointer; it does not own a private object pool. */
extern DisplayObject *D_8009B1C0;

/* Measured prefix only, not the full size of the image-backed workspace.
 * The controller writes a RECT at +8 and reads source X/Y at +0x28/+0x2A.
 * The last halfword ends at 0x800EA154, before the next mapped label
 * D_800EA1E8. See notes/ritual-controller-storage.md. */
#define DUEL_RITUAL_IMAGE_PREFIX_HALFWORDS 22
extern u16 D_800EA128[DUEL_RITUAL_IMAGE_PREFIX_HALFWORDS];

/* The implementation's void * ABI carries a sign-extended card identifier,
 * not an object address to dereference. Its existing integer conversion is
 * retained; func_800262D4 passes (void *)(s32)D_8009B1A0. */
void func_80019CC8(void *card_id);
void func_800262D4(void);

#endif
