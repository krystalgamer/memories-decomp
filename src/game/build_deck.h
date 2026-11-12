#ifndef MEMORIES_DECOMP_BUILD_DECK_H
#define MEMORIES_DECOMP_BUILD_DECK_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "card_constants.h"
#include "file_transfer.h"

/* Small data that build_deck.c defines (#2602), for the units that reach it
 * without the unit's other declarations. */
extern u8 gBuildDeck_bConfirmMode;

/* The same sixteen-byte row is initialized, sorted, counted and drawn.
 * Existing unsigned ID readers retain that view; sorting and stat rendering
 * explicitly read it as s16, including the deck's -1 sentinel. */
typedef struct {
    u32 key;
    u16 id;
    s16 attack;
    s16 defense;
    u8 type;
    u8 pad_0B[2];
    u8 flags;
    u8 pad_0E[2];
} CardEntry;

#define CARD_ENTRY_OFFSET(member) ((u32)&(((CardEntry *)0)->member))

typedef char CardEntry_size_must_be_0x10[sizeof(CardEntry) == 0x10 ? 1 : -1];
typedef char CardEntry_key_offset_must_be_0[
    CARD_ENTRY_OFFSET(key) == 0 ? 1 : -1
];
typedef char CardEntry_id_offset_must_be_4[
    CARD_ENTRY_OFFSET(id) == 4 ? 1 : -1
];
typedef char CardEntry_attack_offset_must_be_6[
    CARD_ENTRY_OFFSET(attack) == 6 ? 1 : -1
];
typedef char CardEntry_defense_offset_must_be_8[
    CARD_ENTRY_OFFSET(defense) == 8 ? 1 : -1
];
typedef char CardEntry_type_offset_must_be_0xA[
    CARD_ENTRY_OFFSET(type) == 0xA ? 1 : -1
];
typedef char CardEntry_flags_offset_must_be_0xD[
    CARD_ENTRY_OFFSET(flags) == 0xD ? 1 : -1
];

/* The matching builder and NTSC renderer walk interior member cursors.
 * Recover the containing row without changing their cursor or its stride. */
#define CARD_ENTRY_FROM_MEMBER(pointer, member) \
    ((CardEntry *)((u8 *)(pointer) - CARD_ENTRY_OFFSET(member)))

/* The card list itself: CARD_ID_END rows, then the scroll offset the slot is
 * measured from and the list kind that picks the box template. */
typedef struct CardList {
    CardEntry entries[CARD_ID_END];
    u8 pad_2D30[4];
    /* The two text boxes the input handler moves. Both are display objects
       whose xy.h.y is a y position: BuildDeck_UpdateCardListInput sets the
       cursor box to cursor * 22 + 0x2A and the scroll box to the thumb
       position it derives from `first`. BuildDeck_Update (build_deck.c)
       pulses both lists' cursor boxes' colour bytes at +0xC/+0xD/+0xE, which
       is what fixes them as display records rather than rows of this list. */
    struct DisplayObject *cursor_box;
    struct DisplayObject *scroll_box;
    s16 first;
    /* Where `first` is heading. BuildDeck_UpdateCardListInput never jumps the scroll: it
       writes the destination here and then steps `first` one row per call
       until the two agree, rebuilding the page each step. The list
       initializer at 0x800325FC zeroes this and `first` together. */
    s16 first_target;
    /* Two row counts, both set to CARD_ID_END - 1 when the list is built.
       They are separate fields rather than one because they are read for
       different things and only one of them is written on the first of the
       two initializer paths:

         - row_count bounds the scroll. BuildDeck_UpdateCardListInput clamps `first` to
           row_count - 8, the last offset that still fills the eight-row
           page, and build_deck.c's shuffle mode sorts row_count rows.
         - sort_row_count is the population the rest of build_deck.c
           sorts, and the denominator BuildDeck_UpdateCardListInput divides the 152-pixel
           thumb travel by.

       Nothing seen so far sets them to different values, so which one a
       future reader should reach for is decided by the use, not by a known
       difference between them. */
    s16 row_count;
    s16 sort_row_count;
    u8 pad_2D44;
    /* The sort order build_deck.c switches on. It is not stored
       directly: BuildDeck_UpdateCardListInput cycles sort_choice and then looks this up as
       the low nibble of gCardList_SortIconTable[sort_choice * 2 + kind * 16 + 1]. */
    u8 sort_mode;
    /* Which entry of the sort menu is highlighted. SELECT and START step it,
       and it wraps within [0, 6]. */
    s8 sort_choice;
    u8 kind;
    /* The cursor row inside the visible page, which is why `first` above is
       the scroll offset: the two are added to reach an entry. BuildDeck_UpdateCardListInput
       clamps it to [0, 7] -- it resets to 7 on reaching 8 and to 0 on going
       below zero -- and multiplies it by 22 for the row's pixel position,
       and CardList_CreateSlotTextBoxes builds the page eight rows at a time. */
    s8 cursor;
} CardList;

#define CARD_LIST_OFFSET(member) ((u32)&(((CardList *)0)->member))

typedef char CardListAssertCursorBox[
    CARD_LIST_OFFSET(cursor_box) == 0x2D34 ? 1 : -1
];
typedef char CardListAssertScrollBox[
    CARD_LIST_OFFSET(scroll_box) == 0x2D38 ? 1 : -1
];
typedef char CardListAssertFirst[CARD_LIST_OFFSET(first) == 0x2D3C ? 1 : -1];
typedef char CardListAssertFirstTarget[
    CARD_LIST_OFFSET(first_target) == 0x2D3E ? 1 : -1
];
typedef char CardListAssertRowCount[
    CARD_LIST_OFFSET(row_count) == 0x2D40 ? 1 : -1
];
typedef char CardListAssertSortRowCount[
    CARD_LIST_OFFSET(sort_row_count) == 0x2D42 ? 1 : -1
];
typedef char CardListAssertSortMode[
    CARD_LIST_OFFSET(sort_mode) == 0x2D45 ? 1 : -1
];
typedef char CardListAssertSortChoice[
    CARD_LIST_OFFSET(sort_choice) == 0x2D46 ? 1 : -1
];
typedef char CardListAssertKind[CARD_LIST_OFFSET(kind) == 0x2D47 ? 1 : -1];
typedef char CardListAssertCursor[
    CARD_LIST_OFFSET(cursor) == 0x2D48 ? 1 : -1
];
/* build_deck.c reaches the pane's two lists as p + 4 and
   p + 0x2D50, and picks between them with p[0x6342] * 0x2D4C + 4. That
   stride is this record's size, which is what the trailing padding after
   `cursor` accounts for. */
typedef char CardListAssertSize[sizeof(CardList) == 0x2D4C ? 1 : -1];

#undef CARD_LIST_OFFSET

void CardList_CreateSlotTextBox(CardList *list, s32 slot);
void CardList_CreateSlotTextBoxes(CardList *list, s32 count);

#define BUILD_DECK_CONFIRM_FLAG_WIDE_DIALOG 0x80

#define BUILD_DECK_TRANSITION_STATE_OFFSET(member) \
    ((u32)&(((BuildDeckTransitionState *)0)->member))

/* Shared Build Deck workspace used by the list, count, input, and transition
 * callbacks. The two CardList records account for the formerly repeated
 * +4/+0x2D50 views; the three card-indexed byte tables line up exactly at
 * +0x5AC4, +0x5D97, and +0x606A. */
typedef struct BuildDeckTransitionState {
    u16 *deck_cards;
    CardList lists[2];
    s32 chest_total;
    s32 deck_total;
    s32 viewport_target_x;
    s32 viewport_step_x;
    s32 transition_ticks;
    u8 pad_5AB0[0x14];
    u8 deck_card_quantities[CARD_ID_END];
    u8 chest_card_quantities[CARD_ID_END];
    u8 card_sort_rank[CARD_ID_END + 1];
    u16 state;
    u16 next_state;
    u8 pane_index;
    u8 pad_6343;
} BuildDeckTransitionState;

typedef char BuildDeckTransitionState_target_offset_must_be_0x5AA4[
    BUILD_DECK_TRANSITION_STATE_OFFSET(viewport_target_x) == 0x5AA4 ? 1 : -1
];
typedef char BuildDeckTransitionState_lists_offset_must_be_0x4[
    BUILD_DECK_TRANSITION_STATE_OFFSET(lists) == 0x4 ? 1 : -1
];
typedef char BuildDeckTransitionState_deck_list_offset_must_be_0x2D50[
    BUILD_DECK_TRANSITION_STATE_OFFSET(lists[1]) == 0x2D50 ? 1 : -1
];
typedef char BuildDeckTransitionState_chest_total_offset_must_be_0x5A9C[
    BUILD_DECK_TRANSITION_STATE_OFFSET(chest_total) == 0x5A9C ? 1 : -1
];
typedef char BuildDeckTransitionState_deck_quantity_offset_must_be_0x5AC4[
    BUILD_DECK_TRANSITION_STATE_OFFSET(deck_card_quantities) == 0x5AC4 ? 1 : -1
];
typedef char BuildDeckTransitionState_chest_quantity_offset_must_be_0x5D97[
    BUILD_DECK_TRANSITION_STATE_OFFSET(chest_card_quantities) == 0x5D97 ? 1 : -1
];
typedef char BuildDeckTransitionState_sort_rank_offset_must_be_0x606A[
    BUILD_DECK_TRANSITION_STATE_OFFSET(card_sort_rank) == 0x606A ? 1 : -1
];
typedef char BuildDeckTransitionState_state_offset_must_be_0x633E[
    BUILD_DECK_TRANSITION_STATE_OFFSET(state) == 0x633E ? 1 : -1
];
typedef char BuildDeckTransitionState_pane_index_offset_must_be_0x6342[
    BUILD_DECK_TRANSITION_STATE_OFFSET(pane_index) == 0x6342 ? 1 : -1
];
typedef char BuildDeckTransitionState_size_must_be_0x6344[
    sizeof(BuildDeckTransitionState) == 0x6344 ? 1 : -1
];

#undef BUILD_DECK_TRANSITION_STATE_OFFSET

/* Stored by build_deck.c, which writes the first workspace's address and
 * reloads it once both panes are set up. The pointer identifies one complete
 * workspace because that function advances its cursor by 0x6344 per pane.
 * build_deck.c reads it for lists[kind] and the three card-indexed byte
 * tables. Every global access is a %gp_rel
 * lw/sw (BuildDeck_SortCardList.s:258, BuildDeck_HasOpenDeckSlot,
 * BuildDeck_Update.s:17/:31/:43), so the plain declaration. Initial value not
 * read.
 *
 * BuildDeck_Update still spells the two scroll-box loads as byte offsets:
 * replacing those two expressions with the equivalent fields changes GCC
 * 2.8.1 instruction selection. The shared layout checks keep those preserved
 * raw accesses tied to the same record. The complete gBuildDeck_apfnStepHandlers callback
 * family receives this type directly. */
extern BuildDeckTransitionState *gBuildDeck_pState;

extern u8 gCardList_SortIconTable[];

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
