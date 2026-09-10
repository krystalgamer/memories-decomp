#ifndef YUGIOH_GAME_DUEL_EFFECT_COMMAND_TABLE_H
#define YUGIOH_GAME_DUEL_EFFECT_COMMAND_TABLE_H

#include "../types.h"

/* The duel effect command table at 0x80090EAC.
 *
 * Left unsized deliberately. Text_DispatchSecondaryCommand selects an entry
 * with a byte taken from a bytecode stream, so nothing at the call site bounds
 * the index and a size here would assert more than is known. */
extern void (*D_80090EAC[])(u8 *);

#endif
