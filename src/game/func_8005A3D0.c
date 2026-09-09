#include "../types.h"
#include "model.h"

/* What ModelSlot.entries points at, as far as this function is concerned: a
   0x50-byte record whose 0x4C word is the parent pointer it searches for.
   model.h types that field as a bare u8 *, so the stride lives here rather
   than there, and this stays local until a second user needs it. */
typedef struct ModelLink {
    u8 pad_00[0x4C];
    void *parent;
    u8 pad_50[0x50 - 0x4C - 4];
} ModelLink;

s32 func_8005A3D0(ModelSlot *model, void *parent)
{
    register s32 index asm("t0");
    register s32 offset asm("t3");
    register ModelLink *link asm("t2");
    register s32 backlink_index asm("a2");
    register s32 count asm("t1");
    register ModelLink *target asm("a3");

    {
        register s32 current_count asm("v1") = model->entry_count;

        index = 0;
        if (current_count == 0) {
            goto done;
        }
        offset = index;
        link = (ModelLink *)model->entries;

outer:
        backlink_index = 0;
        if (link->parent != parent) {
            goto next;
        }
        if (current_count == 0) {
            goto next;
        }
        count = current_count;
        target = (ModelLink *)(model->entries + offset);
    }

    {
        register ModelLink *backlink asm("v1") = (ModelLink *)model->entries;

inner:
        if (backlink->parent == target) {
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
    offset += sizeof(ModelLink);
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
