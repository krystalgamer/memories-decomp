#include "../types.h"

typedef struct {
    u8 pad_00[0x54];
    void *resource;
} DisplayObjectResource;

extern void func_80040468(
    DisplayObjectResource *,
    s32,
    s32,
    s32,
    s32,
    s32
);
extern void func_800404CC(
    void *,
    s32,
    s32,
    s32,
    s32,
    s32,
    s32,
    s32
);

void func_80042874(
    DisplayObjectResource *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    void *resource
)
{
    object->resource = resource;
    func_80040468(object, arg1, arg2, arg3, arg4, arg5);
}

void func_800428A8(
    void *object,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    s32 arg4,
    s32 arg5,
    s32 arg6,
    s32 arg7,
    s32 arg8
)
{
    *(s32 *)((u8 *)object + 0x54) = arg8;
    func_800404CC(object, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}
