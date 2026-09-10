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
extern s32 D_80010008 __attribute__((section(".data")));
extern s32 D_80010014 __attribute__((section(".data")));
extern s32 D_80010018 __attribute__((section(".data")));
extern s32 D_8001002C __attribute__((section(".data")));

/* Module data arguments copied into channel +0xDE8/+0xDEC, not entry points.
 * Keep the initializer candidate's signed-word ABI view. */
extern s32 D_8001001C;
extern s32 D_80010020;
extern s32 D_80010024;
extern s32 D_80010028;

#endif
