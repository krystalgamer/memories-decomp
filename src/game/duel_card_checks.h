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
/* The two tables those lookups read, which the comment above already names.
 * Duel_CheckEquip walks gDuel_awEquipTable as u16 pairs; Duel_CheckFusion
 * takes gDuel_aFusionTable's base as a u8 * and strides it itself.
 *
 * Neither is defined in C. Duel_LoadPackageStage
 * (src/candidates/func_800171A8.c) fills both from the disc, handing their
 * addresses to a transfer descriptor as
 * `d->value_08 = d->value_0C = (u32)gDuel_awEquipTable`, which is why that
 * source declared them too and now includes this header instead.
 *
 * Both keep the incomplete-array spelling both consumers already used; the
 * loader is the only thing that knows how long they are and it learns that
 * from the package. */
extern u16 gDuel_awEquipTable[];
extern u16 gDuel_aFusionTable[];

s32 Duel_CheckEquip(s32 equipment, s32 monster);
s32 Duel_CheckFusion(s32 card_a, s32 card_b);

#endif
