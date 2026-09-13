#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model_parent_search.h"

s32 func_8005A3D0(ModelSlot *model, void *parent)
{
    s32 current_count = model->entry_count;
    s32 index = 0;
    s32 offset;
    s32 backlink_index;
    s32 count;
    GsCOORDUNIT *link;
    GsCOORDUNIT *target;
    GsCOORDUNIT *backlink;
    u8 *entries;

    if (current_count == 0)
        goto done;
    offset = index;
    link = (GsCOORDUNIT *)model->entries;
    do {
        backlink_index = 0;
        if (link->super != parent)
            goto next;
        if (current_count == 0)
            goto next;
        count = current_count;
        entries = model->entries;
        target = (GsCOORDUNIT *)(entries + offset);
        backlink = (GsCOORDUNIT *)entries;
inner:
        if (backlink->super == target)
            goto after_inner;
        backlink_index++;
        if (backlink_index < count) {
            backlink++;
            goto inner;
        }
after_inner:
        if (backlink_index < model->entry_count)
            goto done;
next:
        offset += sizeof(GsCOORDUNIT);
        link++;
        current_count = model->entry_count;
        index++;
    } while (index < current_count);
done:
    return index;
}
