#ifndef MEMORIES_DECOMP_CARD_LIST_TEXT_BOXES_H
#define MEMORIES_DECOMP_CARD_LIST_TEXT_BOXES_H

#include "../types.h"
#include "card_constants.h"

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
       whose field_30.h.field_32 is a y position: BuildDeck_UpdateCardListInput sets the
       cursor box to cursor * 22 + 0x2A and the scroll box to the thumb
       position it derives from `first`. BuildDeck_Update (func_800339D0.c)
       pulses both lists' cursor boxes' colour bytes at +0xC/+0xD/+0xE, which
       is what fixes them as display records rather than rows of this list. */
    struct DisplayObject *G32 cursor_box;
    struct DisplayObject *G32 scroll_box;
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
           page, and card_list_sort.c's shuffle mode sorts row_count rows.
         - sort_row_count is the population the rest of card_list_sort.c
           sorts, and the denominator BuildDeck_UpdateCardListInput divides the 152-pixel
           thumb travel by.

       Nothing seen so far sets them to different values, so which one a
       future reader should reach for is decided by the use, not by a known
       difference between them. */
    s16 row_count;
    s16 sort_row_count;
    u8 pad_2D44;
    /* The sort order card_list_sort.c switches on. It is not stored
       directly: BuildDeck_UpdateCardListInput cycles sort_choice and then looks this up as
       the low nibble of D_80090DD8[sort_choice * 2 + kind * 16 + 1]. */
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
/* build_deck_pane_input.c reaches the pane's two lists as p + 4 and
   p + 0x2D50, and picks between them with p[0x6342] * 0x2D4C + 4. That
   stride is this record's size, which is what the trailing padding after
   `cursor` accounts for. */
typedef char CardListAssertSize[sizeof(CardList) == 0x2D4C ? 1 : -1];

#undef CARD_LIST_OFFSET

void CardList_CreateSlotTextBox(CardList *list, s32 slot);
void CardList_CreateSlotTextBoxes(CardList *list, s32 count);

#endif
