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

/* The motion view of a display object: a position triple at 0x30 with its
 * three 8.8 fraction bytes at 0x62, and a per-frame delta triple at 0x36.
 *
 * 0x36/0x38 is read two ways across the tree, and this is the second of them.
 * func_80043178.h's DisplayObjectSnapshot and display_object_interpolation.h's
 * DisplayObjectPosition both treat the pair as a *saved* or *source* position
 * that eases into the live pair at 0x30/0x32. Here the same halfwords are
 * added to the position every frame, which is only meaningful as a velocity.
 *
 * Neither view is wrong and neither generalises: the memory belongs to
 * whichever motion path owns the object, exactly as 0x44 and 0x4C in
 * display_object.h are a colour to one renderer and a scale or a callback to
 * another. name_entry_glyph_effects.c settles this reading for the objects
 * it drives -- it accelerates 0x38 toward 0x800 by 0x40 a frame through
 * DisplayObject_StepToward, decays 0x36 toward zero by 8 through
 * DisplayObject_StepTowardZero, then calls DisplayObject_StepPositionXY.
 * That is gravity, friction and integration, in that order. */
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
   there, not decorative.

   Indexed by a display object's ot_index: func_80042918 sets 1 and reads
   [1], func_8004293C sets 3 and reads [3], func_800400AC reads
   [ot_index]. [1] and [3] used to be spelled D_8009AF76 and D_8009AF7A
   privately. Seven byte readers (dialog_transition.c, func_800339D0.c,
   func_8003DA40.c, func_800388D8.c, func_800283F4.c and
   DuelEffect_CreateChannel) take the low byte of [1] through %hi/%lo,
   outside small data; dialog_transition.c, func_800339D0.c, func_8003DA40.c
   and func_800283F4.c define the arm below for that. */
#ifdef D_8009AF74_IN_DATA
extern volatile u16 D_8009AF74[4] __attribute__((section(".data")));
#else
extern volatile u16 D_8009AF74[4] __attribute__((section(".sdata")));
#endif

void func_800427DC(DisplayObject *object, int value);
void func_80042824(DisplayObject *object, int value);
void func_80042874( DisplayObjectResource *object, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, void *resource );
/* arg8 is a resource pointer, not an integer. Every caller passes one -- the
   overlays pass D_801AF000 / D_801AF800, the resident callers pass a spec or
   table address -- and the function stores it straight into the object's
   0x54 word, which DisplayObject already declares void *. func_80042874 just
   above it does the same job through DisplayObjectResource::resource and
   spells the parameter this way already. */
void func_800428A8( void *object, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, void *resource );
s32 func_800428EC(u8 *object, s8 value);
void func_80042918(DisplayObject *object);
void func_8004293C(DisplayObject *object);
int func_80042960(DisplayObject *object);
u32 func_800429A8(const u8 *data);
u8 *func_800429BC(DisplayObjectStream *object, const u8 *data);
void DisplayObject_ResetVelocity(DisplayObjectVelocity *object);
void DisplayObject_StepPositionX(DisplayObjectVelocity *object);
void DisplayObject_StepPositionY(DisplayObjectVelocity *object);
void DisplayObject_StepPositionZ(DisplayObjectVelocity *object);
void DisplayObject_StepPositionXY(DisplayObjectVelocity *object);
void DisplayObject_StepPositionXYZ(DisplayObjectVelocity *object);
s32 DisplayObject_StepToward(s32 value, s32 target, s32 step);
s32 DisplayObject_StepTowardZero(s32 value, s32 step);

#endif
