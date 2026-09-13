#ifndef MEMORIES_DECOMP_MAIN_MENU_CARD_TABLES_H
#define MEMORIES_DECOMP_MAIN_MENU_CARD_TABLES_H

#include "../../types.h"
#include "../../game/duel_card.h"

/* The two resident card tables this overlay reads, both indexed by card id
 * minus one. Neither is declared here any more: src/game/duel_card.h is their
 * one home and this header only points at it.
 *
 * gDuel_adwCardStats is the packed card stat word. trade_screen_helpers.c and
 * card_stat_comparators.c take its type field with CARD_STAT_TYPE_SHIFT and
 * CARD_STAT_TYPE_MASK, which main_menu_functions.csv describes as bits 26..30,
 * 0..19 being monsters and 20..23 Magic, Trap, Ritual and Equip.
 *
 * gCard_asNameSortKey is the signed name sort key. The comparators map an id
 * to gCard_asNameSortKey[id - 1] and order on it, substituting INT_MAX for id
 * 0 so the empty slot sorts last.
 *
 * This header used to declare both under their address names and say that an
 * overlay could not do otherwise, because overlay links resolve resident data
 * through splat's generated undefined_syms_auto.txt, which for main_menu emits
 *
 *     D_801D4244 = 0x801D4244;
 *     D_801D4D8E = 0x801D4D8E;
 *
 * That part is still true -- it is what tmp/overlays/main_menu/
 * undefined_syms_auto.txt holds today. What was wrong is the conclusion drawn
 * from it. That generated file is not the whole linker input: overlay_build.py
 * appends the module's linker_symbols file as a second -T script beside it, so
 * a resident symbol is nameable from an overlay as soon as it is listed there.
 *
 * main_menu_linker_symbols.txt already did that for resident *data* when the
 * comment was written. At af5cf8930, the commit that added it, the file was
 * eight lines and two of them were
 *
 *     gInput_wPad1Repeat = 0x8009B394;
 *     gInput_wPad1Pressed = 0x8009B398;
 *
 * -- resident scalars, named semantically, in this module, on that day. The
 * two entries the tables needed are now in the same file.
 */

#endif
