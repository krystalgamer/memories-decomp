#ifndef MEMORIES_DECOMP_DUEL_SCENE_FIELD_ACTIONS_H
#define MEMORIES_DECOMP_DUEL_SCENE_FIELD_ACTIONS_H

#include "../types.h"

/* State private to duel scene-state 5, the field navigation, attack selection
 * and battle transition phase. DuelScene_UpdateFieldActions
 * (src/game/duel_scene_field_actions.c) is the only source in the tree that
 * names either of these. This is the arrangement duel_scene_hand_actions.h
 * records for the neighbouring scene-state 4.
 *
 * Every access to both is gp-relative in retail: 4 and 6 relocations, none
 * through %hi/%lo, so these are the plain scalar declarations. That the
 * distinction is real rather than an artefact of the pattern is visible in
 * the same function, where D_800EAE88 is reached through %hi/%lo five times
 * and gp-relatively never.
 */

/* The scene-state 5 callback, entry 4 of duel_scene_callbacks.c's table. */
void DuelScene_UpdateFieldActions(void);

/* The target colour of the field dim ramp: one grey level repeated in three
 * channels. The pass that dims the field sets it to 0x202020
 * (duel_scene_field_actions.c) and a second site sets it to 0x808080 while starting
 * the same walk over the D_801A7AD8 card records (:616). The ramp moves
 * D_8009B300 (sorted_entry.h, which this unit already includes) eight levels at
 * a time towards it, reading this symbol's low
 * channel through a (u8) cast to get the level, clamping there, repacking the
 * three channels, and comparing the whole word to know it has arrived
 * (:564, :577).
 *
 * Declared s32, which is what the private declaration said and what the
 * listing shows: the two stores are sw, and the two loads are one lw for that
 * word comparison and one lbu for the channel read -- so the narrow read is a
 * cast at the use rather than the declaration's width. */
extern s32 D_8009B1BC;

/* Which of the two field passes the state is in. It is cleared when the state
 * sets up (duel_scene_field_actions.c), raised once and cleared again,
 * and its three reads choose between the dim-and-ramp path and the path that
 * waits for the tagged display objects to retire before finishing
 *.
 *
 * Declared u8: the three loads are lbu and the three stores are sb. Every
 * value written is 0 or 1 and every read is a test against 0, so the stores
 * cannot settle a sign here -- but the loads are the unsigned form, which is
 * the spelling its one declarer chose, and it is kept verbatim. */
extern u8 D_8009B21A;

/* The attacker's and the target's record flags and stat modifiers, saved
 * when the field commits an attack or a fusion so the battle state can
 * restore them. All four are gp-relative halfwords in retail.
 *
 * DuelScene_UpdateBattle (src/game/duel_scene_battle.c) takes each pair as
 * a two-entry side array: as struct elements the loads cannot pass the
 * record stores, which is what reloads D_8009B17A after each flags store in
 * its listing. The scalar view stays for the setters here. */
#ifdef D_8009B170_AS_SIDE_ARRAY
extern u16 D_8009B170[2];
#else
extern u16 D_8009B170;
extern u16 D_8009B172;
#endif
#ifdef D_8009B178_AS_SIDE_ARRAY
extern u16 D_8009B178[2];
#else
extern u16 D_8009B178;
extern u16 D_8009B17A;
#endif

#endif
