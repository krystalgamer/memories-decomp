/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 10 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_8005A3D0.c.
 */
#include "../types.h"
/* libhmd.h names MATRIX, GsOT and GsCOORDUNIT without including anything, so
   it only parses after libgte.h, libgpu.h and libgs.h. model.h deliberately
   keeps clear of that chain and leaves field_D18 incomplete, so a unit that
   reaches through a coordinate unit pulls the chain in itself. */
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../game/model.h"

/* ModelSlot.entries is the base of the MODEL_SLOT_DATA_ENTRY_SIZE-stride run
   model.h describes, and that stride is sizeof(GsCOORDUNIT). The word this
   function searches on sits at 0x4C, which is the unit's `super`, so the
   record it walks is the coordinate unit itself and the search is over the
   parent links. model.h already types the slot's own entry out of the same
   run as a GsCOORDUNIT. */

s32 func_8005A3D0(ModelSlot *model, void *parent)
{
    register s32 index asm("t0");
    register s32 offset asm("t3");
    register GsCOORDUNIT *link asm("t2");
    register s32 backlink_index asm("a2");
    register s32 count asm("t1");
    register GsCOORDUNIT *target asm("a3");

    {
        register s32 current_count asm("v1") = model->entry_count;

        index = 0;
        if (current_count == 0) {
            goto done;
        }
        offset = index;
        link = (GsCOORDUNIT *)model->entries;

outer:
        backlink_index = 0;
        if (link->super != parent) {
            goto next;
        }
        if (current_count == 0) {
            goto next;
        }
        count = current_count;
        target = (GsCOORDUNIT *)(model->entries + offset);
    }

    {
        register GsCOORDUNIT *backlink asm("v1") = (GsCOORDUNIT *)model->entries;

inner:
        if (backlink->super == target) {
            goto after_inner;
        }
        backlink_index++;
        if (backlink_index < count) {
            backlink++;
            goto inner;
        }
    }

after_inner:
    {
        register s32 found asm("v0") = backlink_index < model->entry_count;

        if (found) {
            goto done;
        }
    }

next:
    offset += sizeof(GsCOORDUNIT);
    link++;
    {
        register s32 current_count asm("v1") = model->entry_count;

        index++;
        if (index < current_count) {
            goto outer;
        }
    }

done:
    return index;
}
