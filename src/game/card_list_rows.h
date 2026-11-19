#ifndef MEMORIES_DECOMP_CARD_LIST_ROWS_H
#define MEMORIES_DECOMP_CARD_LIST_ROWS_H

#include "../types.h"

/* The number of card entries in a row set: seven in the North American and
 * Japanese builds, eight in Europe, which makes the record 0x1C bytes. */
#ifndef CARD_LIST_ROW_ENTRY_COUNT
#ifdef VERSION_EUROPE
#define CARD_LIST_ROW_ENTRY_COUNT 8
#else
#define CARD_LIST_ROW_ENTRY_COUNT 7
#endif
#endif

/* CardList_CreateDeckBoxStatRows builds one row set per player. */
#define CARD_LIST_ROW_SET_COUNT 2

/* One of the gFile_StagingBuffer records (24 bytes; European 28) shared by the row
 * builder, the renderer and the trade screen. CardList_RenderDeckBoxStats independently confirms
 * the enabled halfword and the card entries below. */
typedef struct {
    struct DisplayObject *object;
    s16 x;
    s16 y;
    u16 enabled;
    u16 card_entries[CARD_LIST_ROW_ENTRY_COUNT];
} CardListRowSet;

typedef char CardListRowSet_size_must_match_the_entry_count[
    sizeof(CardListRowSet) ==
            (CARD_LIST_ROW_ENTRY_COUNT == 7 ? 0x18 : 0x1C)
        ? 1
        : -1
];
typedef char CardListRowSet_enabled_offset_must_be_0x8[
    OFFSETOF(CardListRowSet, enabled) == 0x8 ? 1 : -1
];
typedef char CardListRowSet_entries_offset_must_be_0xA[
    OFFSETOF(CardListRowSet, card_entries) == 0xA ? 1 : -1
];

/* The two row sets CardList_CreateDeckBoxStatRows builds, and the array
 * CardList_RenderDeckBoxStats and CardList_BuildRowSetTextBoxes index by the
 * display object's kind_index.row_index. The trade screen's refresh writes
 * each row object's sort-icon variant through the same records. They live in
 * the card_list_rows view of gFile_StagingBuffer (file_staging_buffer.h). */

#endif
