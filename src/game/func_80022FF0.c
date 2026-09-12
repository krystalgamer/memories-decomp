#include "../types.h"
#include "duel_grid.h"
#include "display_parent_links.h"

void func_80022FF0(DisplayParent *parent, s32 clear)
{
    DisplayLinkEntry *entry;
    DisplayLinkEntry *child;
    s32 slot;

    entry = parent->entries;
    if (entry != 0) {
        func_80022F98(parent, parent->base);
        parent->base = 0;
        for (slot = 0; slot < DUEL_FIELD_ROW_SIZE; entry++, slot++) {
            child = (DisplayLinkEntry *)((u8 *)entry + 4);
            func_80022F98(parent, entry->object);
            func_80022F98(parent, child->object);
            if (clear) {
                entry->object = 0;
                child->object = 0;
            }
        }
    }
}
