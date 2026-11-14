#ifndef MEMORIES_DECOMP_CARD_LIST_ROWS_H
#define MEMORIES_DECOMP_CARD_LIST_ROWS_H

#include "../types.h"
#include "duel_scene.h"
#include "file_staging_buffer.h"

#define CARD_LIST_ROW_SET_OFFSET(member) \
    ((u32)&(((CardListRowSet *)0)->member))

/* The number of card entries in a row set: seven in the US build. The
 * European build (src/game/european/) defines its own, eight, which makes
 * the record 0x1C bytes. */
#ifndef CARD_LIST_ROW_ENTRY_COUNT
#define CARD_LIST_ROW_ENTRY_COUNT 7
#endif

/* One of the gFile_StagingBuffer records (24 bytes; European 28) shared by the row
 * builder and renderer. CardList_RenderDeckBoxStats independently confirms
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
    CARD_LIST_ROW_SET_OFFSET(enabled) == 0x8 ? 1 : -1
];
typedef char CardListRowSet_entries_offset_must_be_0xA[
    CARD_LIST_ROW_SET_OFFSET(card_entries) == 0xA ? 1 : -1
];

#undef CARD_LIST_ROW_SET_OFFSET

/* The two row sets CardList_CreateDeckBoxStatRows builds, and the array
 * CardList_RenderDeckBoxStats and
 * CardList_BuildRowSetTextBoxes index by the display object's field_6A.
 *
 * The records live in gFile_StagingBuffer, a shared staging buffer that
 * unrelated subsystems reinterpret wholesale at different times: script
 * bytecode, file-transfer destinations, the password shop's price table and
 * the trade screen's state. It is declared once, as bytes, in
 * file_staging_buffer.h, and each user casts to its record type. */


#endif
