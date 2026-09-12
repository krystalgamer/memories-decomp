#ifndef MEMORIES_DECOMP_DUEL_CARD_VIEWER_H
#define MEMORIES_DECOMP_DUEL_CARD_VIEWER_H

#include "../types.h"

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
 * src/candidates/main_menu/func_801821DC.c, stores each pane's card id at
 * two sites. That unit spelled the address D_8009B246 in a private
 * declaration of the same type until then; nothing about its object changed
 * when it took this name, only which name its two relocations carry.
 *
 * DuelEffect_UpdateCardViewerState (src/candidates/func_800283F4.c) is the consumer. It hands
 * the id to func_80029164 to bring
 * the card's record in, and copies it into gDuel_wSelectedCardID for the
 * effect channels.
 *
 * Stored unsigned; readers cast to s16 before using it as a one-based index
 * into gDuel_adwCardStats. */
extern u16 gDuel_wViewerCardID;

/* The vertical offset the viewer draws at, set by the same screens that
 * publish the id above: build_deck_pane_input.c and the main menu's
 * MainMenu_UpdateTradeScreen (now a build-integrated candidate,
 * src/candidates/main_menu/func_801821DC.c) both store 20 (0x14) when they
 * open the viewer, and
 * DuelEffect_UpdateCardViewerState is the consumer:
 *
 *     adj = D_8009B24B;
 *     obj->field_30.h.field_32 += adj;
 *     func_800404CC((u8 *)obj, 0x148, D_8009B24B + 0xE, 0, 2, 0, 0xD, 0x107);
 *
 * so it lands on a display object's y and on the third argument of a draw
 * call. notes/fm-online.md documents that second site from the other
 * direction: retail loads a2 from this byte at 0x800284D8 and the later call
 * adds 0x0E.
 *
 * func_8001BD88 and func_8001D670 also write it and are still assembly. */
extern u8 D_8009B24B;

#endif
