#ifndef MEMORIES_DECOMP_MAIN_MENU_CARD_TABLES_H
#define MEMORIES_DECOMP_MAIN_MENU_CARD_TABLES_H

#include "../../types.h"

/* The two resident card tables this overlay reads, both indexed by card id
 * minus one.
 *
 * D_801D4244 is the packed card stat word. trade_screen_helpers.c and
 * card_stat_comparators.c take its type field with CARD_STAT_TYPE_SHIFT and
 * CARD_STAT_TYPE_MASK, which main_menu_functions.csv describes as bits 26..30,
 * 0..19 being monsters and 20..23 Magic, Trap, Ritual and Equip.
 *
 * D_801D4D8E is the signed name sort key. The comparators map an id to
 * D_801D4D8E[id - 1] and order on it, substituting INT_MAX for id 0 so the
 * empty slot sorts last. main_menu_functions.csv calls these
 * "gCard_asNameSortKey entries indexed by ID-1".
 *
 * Both keep their address-based names on purpose. D_801D4244 is the same
 * object the resident tree declares as gDuel_adwCardStats -- symbols.txt has
 * `gDuel_adwCardStats = 0x801D4244` and duel_card.h declares it under that
 * name -- but an overlay cannot say so. Overlay links resolve resident data
 * through splat's generated undefined_syms_auto.txt, which for main_menu emits
 *
 *     D_801D4244 = 0x801D4244;
 *     D_801D4D8E = 0x801D4D8E;
 *
 * and nothing else, so the semantic name is not visible here. Using it would
 * be an undefined reference, the same limitation the resident function
 * TextBox_Create runs into from overlay code.
 */
extern s32 D_801D4244[];
extern s16 D_801D4D8E[];

#endif
