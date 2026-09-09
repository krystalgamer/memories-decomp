#ifndef MEMORIES_DECOMP_DUEL_CARD_CHECKS_H
#define MEMORIES_DECOMP_DUEL_CARD_CHECKS_H

#include "../types.h"

/* The two table lookups that answer "may these two cards combine?". Both take
 * card ids and both return 0 for no, so a caller can test them as a
 * condition -- and every one of the five does.
 *
 * What a prototype cannot say is what the non-zero value is, and the two
 * differ. Duel_CheckEquip hands back `equipment` unchanged, so its result
 * carries no information the caller did not already have; ai_fusion.c and
 * duel_field_equip_search.c use it as a plain predicate. Duel_CheckFusion
 * returns the id of the card the pair fuses INTO, which ai_fusion.c reads as a
 * value and ai_turn_action.c only tests.
 *
 * The tables are also asymmetric in opposite ways. gDuel_awEquipTable is
 * keyed on the equipment card and searched for the monster, so the argument
 * order matters. Duel_CheckFusion sorts its pair first, so it does not. */
s32 Duel_CheckEquip(s32 equipment, s32 monster);
s32 Duel_CheckFusion(s32 card_a, s32 card_b);

#endif
