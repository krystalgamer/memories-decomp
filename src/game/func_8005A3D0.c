#include "../types.h"

typedef struct ModelLink {
    u8 pad_00[0x4C];
    void *parent;
    u8 pad_50[0x50 - 0x4C - 4];
} ModelLink;

typedef struct {
    u8 pad_000[0xD14];
    ModelLink *links;
    u8 pad_D18[0xE17 - 0xD18];
    u8 link_count;
} ModelSlot;

s32 func_8005A3D0(ModelSlot *model, void *parent)
{
    register s32 index asm("t0");
    register s32 offset asm("t3");
    register ModelLink *link asm("t2");
    register s32 backlink_index asm("a2");
    register s32 count asm("t1");
    register ModelLink *target asm("a3");

    {
        register s32 current_count asm("v1") = model->link_count;

        index = 0;
        if (current_count == 0) {
            goto done;
        }
        offset = index;
        link = model->links;

outer:
        backlink_index = 0;
        if (link->parent != parent) {
            goto next;
        }
        if (current_count == 0) {
            goto next;
        }
        count = current_count;
        target = (ModelLink *)((u8 *)model->links + offset);
    }

    {
        register ModelLink *backlink asm("v1") = model->links;

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
        register s32 found asm("v0") = backlink_index < model->link_count;

        if (found) {
            goto done;
        }
    }

next:
    offset += sizeof(ModelLink);
    link++;
    {
        register s32 current_count asm("v1") = model->link_count;

        index++;
        if (index < current_count) {
            goto outer;
        }
    }

done:
    return index;
}
