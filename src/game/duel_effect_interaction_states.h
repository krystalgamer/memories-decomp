#ifndef MEMORIES_DECOMP_DUEL_EFFECT_INTERACTION_STATES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_INTERACTION_STATES_H

#include "../types.h"
#include "duel_effect.h"

/* Five more D_80090E64 entries: the states that wait on the player. They open
 * the choice list through Dialog_OpenChoice, run the SQUARE-button hold-to-skip
 * countdown in D_8009B32C, and release the choice object through func_8004036C
 * when the state ends. Like the rest of the table they take the state record
 * and report through its 0x51 byte. */
void func_800375A4(DuelEffectChannel *object);
void func_8003767C(DuelEffectChannel *object);
void func_8003771C(DuelEffectChannel *object);
void func_800377AC(DuelEffectChannel *object);
void func_800377C8(DuelEffectChannel *object);

#endif
