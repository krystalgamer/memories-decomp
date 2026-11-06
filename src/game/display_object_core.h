#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_CORE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_CORE_H

#include "../types.h"

struct DisplayObject;

extern u8 tail_data_start[];

s32 DisplayObject_FindFreeGeneralSlot(void);
s32 DisplayObject_FindFreeSlot(void);
void *DisplayObject_AcquireSlot(s32 index, s32 key);

void DisplayObject_Release(struct DisplayObject *slot);
void DisplayObject_MoveToListHead(struct DisplayObject *slot, s32 key);

void DisplayObject_ReleaseIfPresent(void *object);

void DisplayObject_Reset(void);
void DisplayObject_ConfigureSpriteAtPosition(
    void *object, s32 x, s32 y, s32 resource_root_index, s32 resource_branch_index,
    s32 resource_variant, s32 texture_config, s32 palette_config
);

#endif
