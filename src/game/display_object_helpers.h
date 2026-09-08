#ifndef DISPLAY_OBJECT_HELPERS_H
#define DISPLAY_OBJECT_HELPERS_H

#include "../types.h"
#include "display_object.h"

typedef struct {
    u8 pad_00[0x54];
    void *resource;
} DisplayObjectResource;

typedef struct {
    u8 pad_00[0x54];
    u8 *base;
} DisplayObjectStream;

typedef struct {
    u8 pad_00[0x30];
    s16 x;
    s16 y;
    s16 z;
    s16 velocity_x;
    s16 velocity_y;
    s16 velocity_z;
    u8 pad_3C[0x26];
    u8 fraction_x;
    u8 fraction_y;
    u8 fraction_z;
} DisplayObjectVelocity;

/* Lives in .sdata. Declared volatile because display_object_helpers.c
   reads it through a volatile pointer; the qualifier is load-bearing
   there, not decorative. */
extern volatile u16 D_8009AF74[4] __attribute__((section(".sdata")));

void func_800427DC(u8 *object, int value);
void func_80042824(u8 *object, int value);
void func_80042874( DisplayObjectResource *object, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, void *resource );
void func_800428A8( void *object, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8 );
s32 func_800428EC(u8 *object, s8 value);
void func_80042918(DisplayObject *object);
void func_8004293C(DisplayObject *object);
int func_80042960(char *object);
u32 func_800429A8(const u8 *data);
u8 *func_800429BC(DisplayObjectStream *object, const u8 *data);
void func_800429D8(DisplayObjectVelocity *object);
void func_80042A00(DisplayObjectVelocity *object);
void func_80042A28(DisplayObjectVelocity *object);
void func_80042A50(DisplayObjectVelocity *object);
void func_80042A78(DisplayObjectVelocity *object);
void func_80042AA4(DisplayObjectVelocity *object);
s32 func_80042AD8(s32 value, s32 target, s32 step);
s32 func_80042B08(s32 value, s32 step);

#endif
