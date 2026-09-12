#ifndef MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H
#define MEMORIES_DECOMP_DUEL_EFFECT_RESOURCE_SETUP_H

#include "../types.h"
#include "file_transfer.h"

/* Async phase callback for func_80029164's seven-sector request. It retains
 * the definition's byte view of the transfer descriptor. */
void func_800289BC(u8 *descriptor, s32 mode);

/* Starts the async read of one card's effect artwork into slot `slot` of the
 * D_800EA0E8 record array. `value` is the card id: it is stored at +0x30 of
 * the record and turned into the disc position (value - 1) * 7 + CARD_COUNT,
 * seven sectors long, so a card's art is seven sectors and the table begins
 * one card's worth past the card data.
 *
 * It hands back the transfer descriptor it started, with `slot` already
 * stored in the descriptor's callback_data for func_800289BC to pick up, and
 * it has published the descriptor's status through D_8009B0F4 with
 * FILE_TRANSFER_STATE_PRIMARY_ACTIVE set. Every caller ignores the value,
 * which is why they can: the descriptor is reachable without it. The password
 * overlay's shop.c calls it too. */
FileTransferDescriptor *func_80029164(s32 slot, s32 value);

#endif
