#ifndef YUGIOH_GAME_DUEL_EFFECT_COMMAND_TABLE_H
#define YUGIOH_GAME_DUEL_EFFECT_COMMAND_TABLE_H

#include "../types.h"

struct DuelEffectChannel;

/* The duel effect command table at 0x80090EAC.
 *
 * gText_apfnPrimaryCommands and gDuelEffect_EntryHandlers follow it as
 * separate tables with their own calling conventions.
 * Every secondary entry receives the active DuelEffectChannel. Left unsized
 * deliberately: Text_DispatchSecondaryCommand selects an entry with a byte
 * taken from a bytecode stream, so nothing at the call site bounds the index
 * and a size here would assert more than is known. */
extern void (*gText_SecondaryCommandHandlers[])(struct DuelEffectChannel *);

#endif
