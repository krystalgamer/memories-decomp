#ifndef MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H
#define MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H

#include "../types.h"
#include "file_transfer.h"

/* Starts the async read of one card's effect artwork into slot `slot` of the
 * D_800EA0E8 record array. `value` is the card id: it is stored at +0x30 of
 * the record and turned into the build's card-art disc position. US/Japan
 * use seven sectors per card; Europe uses eight.
 *
 * It hands back the transfer descriptor it started, with `slot` already
 * stored in the descriptor's callback_data for func_800289BC to pick up, and
 * it has published the descriptor's status through D_8009B0F4 with
 * FILE_TRANSFER_STATE_PRIMARY_ACTIVE set. Every caller ignores the value,
 * which is why they can: the descriptor is reachable without it. The password
 * overlay's shop.c calls it too. */
FileTransferDescriptor *func_80029164(s32 slot, s32 value);

/* Async phase callback for func_80029164's card-art request. Phase 0
 * points both buffer words at D_8009B118 for 0x3800 bytes; the next phase
 * uploads the four rects of the D_800EA0E8 entry whose index func_80029164
 * left in callback_data. */
void func_800289BC(FileTransferDescriptor *descriptor, s32 mode);

/* European vertical layout selector used while preparing preview sprites. */
#ifdef D_8009C02B_IN_DATA
extern u8 D_8009C02B __attribute__((section(".data")));
#else
extern u8 D_8009C02B;
#endif

/* The value D_8009C02B takes at boot and wraps back to in the debug menu:
   the European release starts at 0 and asks, the single-language releases
   fix their own index. */
#ifndef BUILD_LANGUAGE_INDEX
#define BUILD_LANGUAGE_INDEX 0
#endif

#endif
