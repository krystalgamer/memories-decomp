#include "../types.h"
#include "high_memory_addresses.h"
#include "model_load_words.h"

/* The first object in the executable: the read-only address words at
   0x80010000, ahead of file_setup's read-only data. Each word is four bytes,
   which a -G8 unit would put in .sdata, so they are a -G0 unit of their own.
   shared_work_base.h is not included: its readers were compiled against a
   plain u8 * view of the first word, which DuelEffect_AllocateRequest's
   load order needs. See notes/high-memory-load-addresses.md. */
u8 *const gRuntime_pSharedWorkBase = (u8 *)0x80100000;
u8 *const gModel_pSlot1PayloadBase = (u8 *)0x80140000;
u8 *const gModel_pAuxiliaryPayload = (u8 *)0x80180000;
u8 *const gModel_pSlot0PrimaryModule = (u8 *)0x8013A000;
u8 *const gModel_pSlot1PrimaryModule = (u8 *)0x8017A000;
u8 *const gModel_pSlot0VariantModule = (u8 *)0x8013B000;
u8 *const gModel_pSlot1VariantModule = (u8 *)0x8017B000;
const s32 gModel_Slot0PrimaryDataAddress = (s32)0x80135000;
const s32 gModel_Slot1PrimaryDataAddress = (s32)0x80175000;
const s32 gModel_Slot0VariantDataAddress = (s32)0x80136000;
const s32 gModel_Slot1VariantDataAddress = (s32)0x80176000;
u8 *const gMainMenu_pSuModuleBase = (u8 *)0x80180000;
const s32 gModel_CreditsModuleAddress = (s32)0x80180000;
#ifdef VERSION_EUROPE
void *const gModel_pCreditsMusicTrackBuffer = (void *)0x80185F00;
#else
void *const gModel_pCreditsMusicTrackBuffer = (void *)0x80185CD4;
#endif
