#ifndef MEMORIES_DECOMP_DUEL_CARD_VIEWER_H
#define MEMORIES_DECOMP_DUEL_CARD_VIEWER_H

#include "../types.h"
#include "display_object.h"
#include "duel_effect.h"

/* The card id the viewer screen is currently showing.
 *
 * It is a publication point rather than a working variable: the pickers write
 * it and the viewer reads it, with nothing in between. build_deck_pane_input.c
 * stores the highlighted row's id at both of its pick sites, and
 * duel_update_card_pick_cursor.c does the same from the field cursor -- its
 * header comment describes a successful pick as being "published in
 * gDuel_wViewerCardID along with the event code 0x14 and state 2".
 *
 * A third writer since 2026-09-12: the Trade screen updater,
 * src/overlays/main_menu/trade_update.c, stores each pane's card id at
 * two sites. That unit spelled the address D_8009B246 in a private
 * declaration of the same type until then; nothing about its object changed
 * when it took this name, only which name its two relocations carry.
 *
 * DuelEffect_UpdateCardViewerState (src/game/func_800283F4.c) is the consumer. It hands
 * the id to func_80029164 to bring
 * the card's record in, and copies it into gDuel_wSelectedCardID for the
 * effect channels.
 *
 * Stored unsigned; readers cast to s16 before using it as a one-based index
 * into gDuel_adwCardStats.
 *
 * DuelScene_UpdateHandActions addresses it with %hi/%lo and defines
 * GDUEL_WVIEWERCARDID_IN_DATA to take the .data arm. */
#ifdef GDUEL_WVIEWERCARDID_IN_DATA
extern u16 gDuel_wViewerCardID __attribute__((section(".data")));
#else
extern u16 gDuel_wViewerCardID;
#endif

/* The vertical offset the viewer draws at, set by the same screens that
 * publish the id above: build_deck_pane_input.c and the main menu's
 * MainMenu_UpdateTradeScreen (src/overlays/main_menu/trade_update.c) both
 * store 20 (0x14) when they
 * open the viewer, and
 * DuelEffect_UpdateCardViewerState is the consumer:
 *
 *     adj = gDuel_bCardViewerYOffset;
 *     obj->field_30.h.field_32 += adj;
 *     DisplayObject_ConfigureSpriteAtPosition((u8 *)obj, 0x148,
 *                  gDuel_bCardViewerYOffset + 0xE, 0, 2, 0, 0xD, 0x107);
 *
 * so it lands on a display object's y and on the third argument of a draw
 * call. notes/fm-online.md documents that second site from the other
 * direction: retail loads a2 from this byte at 0x800284D8 and the later call
 * adds 0x0E.
 *
 * DuelScene_UpdateFieldActions also writes it and is still assembly.
 * DuelScene_UpdateHandActions writes it with %hi/%lo and defines
 * GDUEL_BCARDVIEWERYOFFSET_IN_DATA to take the .data arm. */
extern DisplayObject *gDuel_pCardViewerBackground;
#ifdef GDUEL_BCARDVIEWERYOFFSET_IN_DATA
extern u8 gDuel_bCardViewerYOffset __attribute__((section(".data")));
#else
extern u8 gDuel_bCardViewerYOffset;
#endif
extern DisplayObject *gDuel_pCardViewerCard;
extern DuelEffectChannel *gDuel_pCardViewerTextBox;

/* The card-viewer candidate keeps the original address names as lexical
 * contract keys while the linked symbols above carry the semantic names. */
#ifdef DUEL_CARD_VIEWER_ADDRESS_ALIASES
extern DisplayObject *D_8009B240;
extern DisplayObject *D_8009B24C;
extern DuelEffectChannel *D_8009B250;
#endif

#endif
