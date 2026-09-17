#ifndef MEMORIES_DECOMP_HIGH_MEMORY_ADDRESSES_H
#define MEMORIES_DECOMP_HIGH_MEMORY_ADDRESSES_H

#include "../types.h"

/* The first word is a reused payload pointer, not the payload itself.
 * The indexed consumers also reach the next two bases and two primary
 * modules; keep their D_80010000-relative relocations through a prefix view.
 * The incomplete array prevents -G8 from treating the base as small data.
 * See notes/high-memory-load-addresses.md for the distinct address cohorts. */
#ifdef HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "../ygo_types.h"
extern HighMemoryModelAddressPrefix D_80010000[];
#elif defined(HIGH_MEMORY_ADDRESSES_BASE_IN_DATA)
extern u8 *D_80010000 __attribute__((section(".data")));
#else
extern u8 *D_80010000;
#endif

/* Independently labeled transfer destinations. Do not substitute offsets
 * from D_80010000: the labels are part of each consumer's relocation input. */
extern u8 *D_80010004 __attribute__((section(".data")));
extern s32 D_80010008 __attribute__((section(".data")));
/* Primary control-module bases; the callable entry is four bytes into each. */
extern u8 *D_8001000C __attribute__((section(".data")));
extern u8 *D_80010010 __attribute__((section(".data")));
extern s32 D_80010014 __attribute__((section(".data")));
extern s32 D_80010018 __attribute__((section(".data")));
extern s32 D_8001002C __attribute__((section(".data")));

/* func_800507D0 passes these absolute word loads to argument 7 of
 * File_RequestAsyncTransfer and to func_80049394(u16 *), respectively.
 * The former requests SU.MRG sectors [1223, 1239); the latter is a data
 * argument, not an overlay entry point. */
extern s32 D_80010030 __attribute__((section(".data")));
extern void *D_80010034 __attribute__((section(".data")));

/* Staging base selected by duel package stage 7. */
extern u8 *D_800101DC __attribute__((section(".data")));

/* Module data arguments copied into channel +0xDE8/+0xDEC, not entry points.
 * func_8004CB0C reads each as an absolute word under -G8, so like the
 * destinations above they carry .data rather than a small-data view. */
extern s32 D_8001001C __attribute__((section(".data")));
extern s32 D_80010020 __attribute__((section(".data")));
extern s32 D_80010024 __attribute__((section(".data")));
extern s32 D_80010028 __attribute__((section(".data")));

#endif
