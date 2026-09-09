#ifndef MEMORIES_DECOMP_DUEL_CARD_EFFECTS_H
#define MEMORIES_DECOMP_DUEL_CARD_EFFECTS_H

#include "../types.h"

/* The five life-point and marker handlers of gDuelEffect_apfnGroupHandler,
 * defined in one unit because they share the D_801A7AD8 record walk and the
 * DUEL_EFFECT_LP_SCALE table scaling.
 *
 * func_800250C8 runs the table-driven LP change phases -- recovery values
 * scaled by 100 -- func_8002525C applies one of the five direct-damage cards,
 * func_8002538C and func_800257A0 are the two remaining LP arms, and
 * DuelEffect_UpdateFieldMarker repositions the field marker object from the
 * card record it follows. All five are reached only as table entries, so they
 * take no arguments and report through D_8009B220 like every other handler. */
void func_800250C8(void);
void func_8002525C(void);
void func_8002538C(void);
void func_800257A0(void);
void DuelEffect_UpdateFieldMarker(void);

#endif
