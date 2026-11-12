#ifndef MEMORIES_DECOMP_BUILD_DECK_H
#define MEMORIES_DECOMP_BUILD_DECK_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "build_deck_transition_state.h"
#include "display_object.h"
#include "build_deck_draw_card_list_page_data.h"
#include "card_list_text_boxes.h"
#include "card_constants.h"
#include "file_transfer.h"

/* Puts the build-deck screen's two counters -- chest_total and deck_total --
 * into the gText_StagedValues staging pair, then opens the text box
 * that prints them and takes its display object out of screen space. Called
 * after every add or remove, which is what refreshes the count on screen. */
void BuildDeck_RefreshCountDisplay(BuildDeckTransitionState *record);

/* Returns one copy of card `index` to the chest: bumps
 * base->chest_card_quantities[index] and base->chest_total.
 *
 * The two arms differ in what else they touch. From zero owned, the card was
 * not in the chest list at all, so the entry whose id matches is marked
 * visible and the list is re-sorted; from a non-zero count below
 * CARD_CHEST_QUANTITY_MAX only the two numbers move. At the maximum it does
 * nothing, not even the total. The matched visibility-byte write is volatile
 * so the post-search pointer adjustment is not folded into its store offset. */
void BuildDeck_ReturnCardToChest(BuildDeckTransitionState *base, s32 index);

/* Two updates to the build-deck screen's per-card counts, both taking the
 * complete BuildDeckTransitionState. BuildDeck_RecountDeckCards indexes
 * deck_card_quantities by card id and walks the deck rows as CardEntry
 * records, comparing each row's id as signed so the empty rows' -1 never
 * matches.
 *
 * BuildDeck_TakeCardFromChest decrements one card's count and, when it reaches zero, clears
 * that card's deck record flag and re-sorts. It does nothing at all when the
 * count is already zero, and returns nothing to say so.
 *
 * BuildDeck_RecountDeckCards recounts every card from the deck records and stores how many
 * leading records are still marked. */
void BuildDeck_TakeCardFromChest(BuildDeckTransitionState *state, s32 id);
void BuildDeck_RecountDeckCards(BuildDeckTransitionState *state);

/* Fills the first free entry of the workspace's second CardList with card
 * `card_id`: marks the entry used, writes the id, and copies the card's type,
 * attack and defense out of gDuel_adwCardStats[card_id - 1] with the same
 * shifts and CARD_STAT_SCALE the rest of the build-deck screen uses, walking
 * the deck list as CardEntry records. It then re-sorts through
 * BuildDeck_SortCardList, refreshes the pane, and returns, so a full deck of DECK_SIZE
 * entries is a silent no-op.
 *
 * `base` is s32 rather than a pointer because that is what the definition
 * takes; the body casts it to BuildDeckTransitionState * to reach lists[1].
 * Its one caller holds the same record as u8 * and casts. */
void BuildDeck_AddCard(s32 base, s32 card_id);

/* Draws one page of a Build Deck card list: the sort-menu icons above it and
 * eight rows (nine in Europe) of card id and ATK/DEF. The object's field_67
 * picks the list; list 0 also marks ranked cards and shows chest and deck counts,
 * the other list numbers its rows instead. No C code calls it:
 * BuildDeck_InitScreen stores its address in a display object's field_4C,
 * the (DisplayObject *, GsOT *) draw callback slot. */
void BuildDeck_DrawCardListPage(DisplayObject *obj, GsOT *ot);

/* Draws the seven sort-menu icons of a Build Deck card list through one
 * scratchpad GsSPRITE, 18 pixels apart. `data` is the list kind's row of
 * gCardList_SortIconTable byte pairs; the icon whose low nibble equals `selected` is
 * drawn at full grey, the others dimmed. `ot` is the ordering table it sorts
 * each icon into, which is what its one caller already holds. */
void BuildDeck_DrawSortIcons(
    GsSPRITE *sprite, GsOT *ordering_table, u8 *data, s32 selected);

#ifdef VERSION_EUROPE
void CardList_DrawHeaderDecimalDigits(
    GsSPRITE *sprite, GsOT *ordering_table, u8 *digits, s32 count);
#endif

/* Sets up the Build Deck screen over two contiguous workspaces at `base`.
 * The implementation retains a byte cursor to preserve retail allocation;
 * that cursor does not change the caller-visible workspace type.
 * `deck` is the first pane's deck (forty card ids, then the 722 chest counts at +0x50),
 * `other` the second pane's deck or 0, and `flags` the byte kept at +0x6343.
 * Two callers: Main_RunBuildDeckMenu and Main_RunDuel. */
void BuildDeck_InitScreen(BuildDeckTransitionState *base, void *deck, s32 other, s32 flags);

/* Returns the id of the entry the cursor is on, or 0 when that row's flag
   byte is clear. Both pane-input steps read the highlighted card through it. */
s32 BuildDeck_GetActiveCardID(CardList *list);

/* The two input steps of the Build Deck step table gBuildDeck_apfnStepHandlers
 * (build_deck.c). Both receive the shared transition state;
 * their bodies retain a byte-pointer alias for offset-heavy workspace access.
 *
 * BuildDeck_UpdateChestPaneInput is step 2, the chest pane. Its confirm moves the highlighted
 * card into the deck: BuildDeck_AddCard adds it and BuildDeck_TakeCardFromChest takes one
 * off the chest count. RIGHT slides the viewport to 0x140 through
 * pane-transition step 1, landing in step 3.
 *
 * BuildDeck_UpdateDeckPaneInput is step 3, the deck pane, and does the reverse. Its confirm
 * clears the highlighted deck record, recounts the deck (BuildDeck_RecountDeckCards) and
 * puts the card back in the chest (BuildDeck_ReturnCardToChest). LEFT slides back to 0 and
 * returns to step 2.
 *
 * In both, TRIANGLE opens the card viewer on the highlighted card, and
 * CANCEL enters exit step 4 (BuildDeck_UpdateExitConfirmation) with next_state recording which
 * of the two to come back to. */
void BuildDeck_UpdateDeckPaneInput(BuildDeckTransitionState *state);
void BuildDeck_UpdateChestPaneInput(BuildDeckTransitionState *state);

/* Marks the build-deck transition record initialized and reports whether its
 * bit was already set. */
s32 BuildDeck_TestAndSetInitialized(BuildDeckTransitionState *state);

/* Reports whether the deck has a free slot.
 *
 * It walks DECK_SIZE entries of sixteen bytes each, starting 0x2D50 into the
 * Build Deck workspace, and returns 1 as soon as it finds one whose
 * byte at +0x0D is zero. It returns 0 when every entry is occupied.
 * BuildDeck_UpdateExitConfirmation is the only consumer. */
int BuildDeck_HasOpenDeckSlot(void);

/* Exit step 4 of the Build Deck step table gBuildDeck_apfnStepHandlers
 * (build_deck.c). It asks for confirmation and either
 * returns to the step saved in next_state or writes the deck out and clears
 * the state word. */
void BuildDeck_UpdateExitConfirmation(BuildDeckTransitionState *record);

/* The per-frame Build Deck driver. It pulses the two pane colours with a
 * triangle wave, dispatches gBuildDeck_apfnStepHandlers[gBuildDeck_pState->state & 0x3F] once
 * DuelEffect_UpdateState reports idle, and returns the state word. The exit
 * step above clears that word when it commits the deck, and both callers
 * (Main_RunBuildDeckMenu and Main_RunDuel) leave the menu when it reads zero.
 */
s32 BuildDeck_Update(void);

void BuildDeck_UpdatePaneTransition(BuildDeckTransitionState *state);

/* 0x801D07BC, 0x56C bytes above gLibrary_abCardChest (0x801D0250). */
extern s16 gDuel_awRecentCardDrops[DUEL_RECENT_CARD_DROP_COUNT];
#define DUEL_RECENT_DROPS_CHEST_DISTANCE 0x56C

/* Shared card-list graphics transfer: four stages in NA/JP, five in Europe. */
void CardList_LoadSharedGraphicsStage(FileTransferDescriptor *descriptor, s32 mode);

/* Loads and waits for the graphics shared by Main_RunTrade and
 * BuildDeck_InitScreen. The request is NA 0x2189/0x4C sectors,
 * JP 0x2184/0x4C, or EU 0x2997/0x51. */
void CardList_LoadSharedGraphics(void);

/* Prunes gDuel_awRecentCardDrops in two passes. The first walks the table
 * backwards and clears any ID whose current chest quantity, read from the
 * region 0x56C below the table at ID - 1, is zero. The second moves surviving
 * entries to the front, zeroing each slot it moves out of, so the table ends
 * up densely packed with the order preserved.
 *
 * The ordinary C uses the split-address compiler profile for the initial
 * table address. Its reverse cursor is an integer address, so the final
 * decrement does not form a pointer before the array.
 *
 * BuildDeck_InitScreen and BuildDeck_UpdateExitConfirmation both call it.
 * This is chest quantity, not total ownership: a card with all copies in
 * the deck may lose its recent marker. Duplicates are not removed. */
void Library_PruneRecentCardDrops(void);

/* Both break equal unsigned keys with ascending name keys. Equal name keys
 * still return 1, as in retail; these are not inverse total comparators. */
s32 BuildDeck_CompareSortKeyDescending(CardEntry *left, CardEntry *right);
s32 BuildDeck_CompareSortKeyAscending(CardEntry *left, CardEntry *right);

/* Re-sorts the shared sixteen-byte rows in place, then rebuilds the page. */
void BuildDeck_SortCardList(CardList *list);

/* Handles scrolling, page changes and sort selection for one card list. */
s32 BuildDeck_UpdateCardListInput(CardList *list);

void CardList_UpdateSortMode(CardList *list);

/* Spawns the little card-type icon for card `index` at (start_x, start_y): a 0x10 by 0x10
 * display object whose texture page is stepped by the card's type -- +0x10 for
 * magic and equip, +0x20 for trap, +0x30 for ritual, nothing for a monster --
 * read out of gDuel_adwCardStats[index - 1].
 *
 * `target_x` and `target_y` become the object's field_18.target pair, both
 * narrowed by halfword stores; BuildDeck_UpdateCardTransferIcon moves the icon
 * toward that point and finally snaps it there. The object is returned in v0, although both current
 * callers discard it; the old void declaration hid that return value. */
DisplayObject *BuildDeck_CreateCardTransferIcon(s32 index, s32 start_x, s32 start_y,
                                                s32 target_x, s32 target_y);

void BuildDeck_UpdateCardTransferIcon(DisplayObject *object);

/* Draws `count` bytes of `digits`, last byte first, as 8-pixel digit glyphs
 * through `sprite`, moving it 8 pixels right per byte. A byte that is not a
 * digit leaves a gap. */
void CardList_DrawDecimalDigits(
    GsSPRITE *sprite, GsOT *ot, const u8 *digits, s32 count
);

void BuildDeck_EnterNarrowConfirmMode(void);
void BuildDeck_EnterWideConfirmMode(void);

#endif
