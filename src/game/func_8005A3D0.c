#include "../types.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h. model.h deliberately
   keeps clear of that chain and leaves field_D18 incomplete, so a unit that
   reaches through a coordinate unit pulls the chain in itself. */
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../game/model_parent_search.h"

/* ModelSlot.entries is the base of the MODEL_SLOT_DATA_ENTRY_SIZE-stride run
   model.h describes, and that stride is sizeof(GsCOORDUNIT). The word this
   function searches on sits at 0x4C, which is the unit's `super`, so the
   record it walks is the coordinate unit itself and the search is over the
   parent links. model.h already types the slot's own entry out of the same
   run as a GsCOORDUNIT. */

s32 func_8005A3D0(ModelSlot *model, void *parent)
{
    s32 index;
    s32 offset;
    GsCOORDUNIT *link;
    s32 backlink_index;
    s32 count;
    GsCOORDUNIT *target;
    u32 current;

    {
        current = model->entry_count;

        index = 0;
        if (current == 0) {
            goto done;
        }
        offset = index;
        link = (GsCOORDUNIT *)model->entries;

outer:
        /* Single-pass regions preserve retail allocation under GCC 2.8.1. */
        do {
            backlink_index = 0;
        } while (0);
        if (link->super != parent) {
            goto next;
        }
        if (current == 0) {
            goto next;
        }
        count = current;
        target = (GsCOORDUNIT *)(model->entries + offset);
    }

    {
        current = (u32)model->entries;

inner:
        do {
            do {
                if (((GsCOORDUNIT *)current)->super == target) {
                    goto after_inner;
                }
            } while (0);
        } while (0);
        backlink_index++;
        do {
            if (backlink_index < count) {
                current += sizeof(GsCOORDUNIT);
                goto inner;
            }
        } while (0);
    }

after_inner:
    {
        s32 found = backlink_index < model->entry_count;

        if (found) {
            goto done;
        }
    }

next:
    offset += sizeof(GsCOORDUNIT);
    link++;
    {
        current = model->entry_count;

        index++;
        if (index < (s32)current) {
            goto outer;
        }
    }

done:
    return index;
}
