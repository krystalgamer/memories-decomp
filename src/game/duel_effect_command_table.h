#ifndef YUGIOH_GAME_DUEL_EFFECT_COMMAND_TABLE_H
#define YUGIOH_GAME_DUEL_EFFECT_COMMAND_TABLE_H

#include "../types.h"

struct DuelEffectChannel;

/* The duel effect command table at 0x80090EAC.
 *
 * The entry-handler view starts inside this owner, at +0xAC in NA/Japan
 * and +0xB4 in Europe; its different two-argument ABI remains separate.
 * Every secondary entry receives the active DuelEffectChannel. Left unsized
 * deliberately: Text_DispatchSecondaryCommand selects an entry with a byte
 * taken from a bytecode stream, so nothing at the call site bounds the index
 * and a size here would assert more than is known. */
extern void (*gText_SecondaryCommandHandlers[])(struct DuelEffectChannel *);

#endif
