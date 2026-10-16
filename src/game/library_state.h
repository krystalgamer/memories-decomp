#ifndef MEMORIES_DECOMP_LIBRARY_STATE_H
#define MEMORIES_DECOMP_LIBRARY_STATE_H

#include "../types.h"
#include "../ygo_types.h"
#include "card_constants.h"

#define LIBRARY_CARD_FLAG_KNOWN 0x80
#define LIBRARY_CARD_FLAG_UNOWNED 0x01

/* Card IDs index the table directly; entry zero is the empty selection. */
typedef struct {
    s16 clut_x;
    u8 flags;
    u8 pad_03;
} LibraryCardRecord;

typedef struct {
    LibraryMotionState motion;
    struct DisplayObject *G32 selector_sprite;
    struct DisplayObject *G32 card_view;
    struct DisplayObject *G32 loading_icon;
    LibraryCardRecord cards[CARD_ID_END];
} LibraryState;

#define LIBRARY_STATE_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef char LibraryCardRecord_flags_offset_must_be_0x2[
    LIBRARY_STATE_OFFSET(LibraryCardRecord, flags) == 0x2 ? 1 : -1
];
typedef char LibraryCardRecord_size_must_be_0x4[
    sizeof(LibraryCardRecord) == 0x4 ? 1 : -1
];
typedef char LibraryState_selector_sprite_offset_must_be_0x48[
    LIBRARY_STATE_OFFSET(LibraryState, selector_sprite) == 0x48 ? 1 : -1
];
typedef char LibraryState_card_view_offset_must_be_0x4c[
    LIBRARY_STATE_OFFSET(LibraryState, card_view) == 0x4C ? 1 : -1
];
typedef char LibraryState_loading_icon_offset_must_be_0x50[
    LIBRARY_STATE_OFFSET(LibraryState, loading_icon) == 0x50 ? 1 : -1
];
typedef char LibraryState_cards_offset_must_be_0x54[
    LIBRARY_STATE_OFFSET(LibraryState, cards) == 0x54 ? 1 : -1
];
typedef char LibraryState_size_must_be_0xba0[
    sizeof(LibraryState) == 0xBA0 ? 1 : -1
];

#undef LIBRARY_STATE_OFFSET

extern LibraryState D_800EA1E8;

#endif
