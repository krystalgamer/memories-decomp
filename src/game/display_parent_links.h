#ifndef MEMORIES_DECOMP_DISPLAY_PARENT_LINKS_H
#define MEMORIES_DECOMP_DISPLAY_PARENT_LINKS_H

#include "../types.h"
#include "display_object.h"

#define DISPLAY_PARENT_OFFSET(member) \
    ((u32)&(((DisplayParent *)0)->member))

typedef struct DisplayLinkEntry {
    DisplayObject *object;
    u8 pad_04[8];
} DisplayLinkEntry;

typedef struct DisplayParent {
    DisplayObject *position_base;
    DisplayObject *base;
    DisplayLinkEntry *entries;
    u8 pad_0C[0xB];
    u8 index;
} DisplayParent;

typedef char DisplayLinkEntry_size_must_be_0xC[
    sizeof(DisplayLinkEntry) == 0xC ? 1 : -1
];
typedef char DisplayParent_entries_offset_must_be_0x8[
    DISPLAY_PARENT_OFFSET(entries) == 0x8 ? 1 : -1
];
typedef char DisplayParent_index_offset_must_be_0x17[
    DISPLAY_PARENT_OFFSET(index) == 0x17 ? 1 : -1
];
typedef char DisplayParent_size_must_be_0x18[
    sizeof(DisplayParent) == 0x18 ? 1 : -1
];

#undef DISPLAY_PARENT_OFFSET

void func_80022F98(DisplayParent *parent, volatile DisplayObject *object);
void func_80022FF0(DisplayParent *parent, s32 clear);

#endif
