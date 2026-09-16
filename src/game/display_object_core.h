#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_CORE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_CORE_H

#include "../types.h"

struct DisplayObject;

extern s16 D_800EFE3A[];

s32 DisplayObject_FindFreeGeneralSlot(void);
s32 DisplayObject_FindFreeSlot(void);
void *DisplayObject_AcquireSlot(s32 index, s32 key);

void DisplayObject_Release(struct DisplayObject *slot);
void DisplayObject_MoveToListHead(struct DisplayObject *slot, s32 key);

#ifdef DISPLAY_OBJECT_RELEASE_IF_PRESENT_AMBIENT_OBJECT
void DisplayObject_ReleaseIfPresent(void);
#else
void DisplayObject_ReleaseIfPresent(void *object);
#endif

void DisplayObject_Reset(void);
void DisplayObject_ConfigureSpriteAtPosition(
    void *object, s32 x, s32 y, s32 field_67, s32 field_68,
    s32 field_69, s32 color, s32 texture
);

#endif
