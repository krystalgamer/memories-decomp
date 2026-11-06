#ifndef MEMORIES_DECOMP_HIGH_MEMORY_ADDRESSES_H
#define MEMORIES_DECOMP_HIGH_MEMORY_ADDRESSES_H

#include "../types.h"

/* The block starts at 0x80010000. Its first word, gRuntime_pSharedWorkBase,
 * is declared by shared_work_base.h, and the payload and credits
 * words by model_load_words.h. See notes/high-memory-load-addresses.md for
 * the distinct address cohorts. */

/* Primary control-module bases; the callable entry is four bytes into each. */
extern u8 *gModel_pSlot0PrimaryModule;
extern u8 *gModel_pSlot1PrimaryModule;
extern s32 gModel_Slot0VariantModuleAddress;
extern s32 gModel_Slot1VariantModuleAddress;
extern s32 gMainMenu_dwSuModuleBase;

/* Staging base selected by duel package stage 7. The retail word holds
 * 0x80146000, the base of the WA_MRG shared duel-bank overlay slot
 * (notes/memory-map.md). */
extern u8 *gFile_pDuelBankBase;

/* Module data arguments copied into channel +0xDE8/+0xDEC, not entry points.
 * Model_InitSlot reads each as an absolute word under -G8, so like the
 * destinations above they carry .data rather than a small-data view. */
extern s32 gModel_Slot0PrimaryDataAddress;
extern s32 gModel_Slot1PrimaryDataAddress;
extern s32 gModel_Slot0VariantDataAddress;
extern s32 gModel_Slot1VariantDataAddress;

#endif
