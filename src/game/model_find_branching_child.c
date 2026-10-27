#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model_parent_search.h"

s32 Model_FindBranchingChildCoord(ModelSlot *model, GsCOORDUNIT *parent)
{
    s32 entry_count_snapshot = model->entry_count;
    s32 child_index = 0;
    s32 coordinate_byte_offset;
    s32 grandchild_index;
    s32 entry_count_limit;
    GsCOORDUNIT *child_candidate;
    GsCOORDUNIT *candidate_coordinate;
    GsCOORDUNIT *grandchild_candidate;
    u8 *coordinate_bytes;

    if (entry_count_snapshot == 0)
        goto done;
    coordinate_byte_offset = child_index;
    child_candidate = (GsCOORDUNIT *)model->entries;
    do {
        grandchild_index = 0;
        if (child_candidate->super != parent)
            goto next;
        if (entry_count_snapshot == 0)
            goto next;
        entry_count_limit = entry_count_snapshot;
        coordinate_bytes = model->entries;
        candidate_coordinate = (GsCOORDUNIT *)(coordinate_bytes + coordinate_byte_offset);
        grandchild_candidate = (GsCOORDUNIT *)coordinate_bytes;
inner:
        if (grandchild_candidate->super == candidate_coordinate)
            goto after_inner;
        grandchild_index++;
        if (grandchild_index < entry_count_limit) {
            grandchild_candidate++;
            goto inner;
        }
after_inner:
        if (grandchild_index < model->entry_count)
            goto done;
next:
        coordinate_byte_offset += sizeof(GsCOORDUNIT);
        child_candidate++;
        entry_count_snapshot = model->entry_count;
        child_index++;
    } while (child_index < entry_count_snapshot);
done:
    return child_index;
}
