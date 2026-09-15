#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_CORE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_CORE_H

#include "../types.h"

struct DisplayObject;

extern s16 D_800EFE3A[];

s32 func_8004002C(void);
s32 func_8004006C(void);
void *func_800400AC(s32 index, s32 key);

void func_8004020C(struct DisplayObject *slot);
void func_800402A0(struct DisplayObject *slot, s32 key);

#ifdef FUNC_8004036C_AMBIENT_OBJECT
void func_8004036C(void);
#else
void func_8004036C(void *object);
#endif

void func_800403F0(void);
void func_800404CC(
    void *object, s32 x, s32 y, s32 field_67, s32 field_68,
    s32 field_69, s32 color, s32 texture
);

#endif
