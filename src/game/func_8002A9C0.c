#include "../types.h"
#include "display_object_projection.h"
#include "../psyq/libgte.h"
#include "gpu_packets.h"
#include "display_object_api.h"
#include "display_object.h"
#include "func_8002A9C0.h"

/* The update callback func_8002ABB4 installs at 0x4C on the object it
   builds: it fades the object out over its own field_60 countdown, projects
   it, and emits a four-vertex gouraud line and then a flat one through
   func_8005B260. The record is the canonical DisplayObject.

   Two of its reads keep the byte pointer, and that is measured. The four
   SVECTORs and the packet this function fills live in scratchpad and are
   written through pointers of their own; a read of the object that sits
   between those stores can be floated across them once it is a struct
   reference, and the target does not float these two. 0x32 grows the
   function past its segment and 0x3E changes its code outright, while their
   neighbours 0x30 and 0x3C convert with no change at all. So those two stay
   spelled off `bytes`, and the comment on each says why. Everything else the
   function touches is an ordinary member read. */
void func_8002A9C0(DisplayObject *o, s32 arg1)
{
    u8 *bytes = (u8 *)o;
    s32 sp28;
    s32 sp2C;
    u8 *q;
    u8 *r;
    u8 *b0;
    u8 *b1;
    u8 *b2;
    u8 *b3;
    s32 n;
    s32 e;
    s32 m;
    s32 z;
    s32 k;
    s32 v;
    s32 w;

    n = *(u16 *)&o->field_60 - 0x10;
    o->field_60 = n;
    if ((s16)n <= 0) {
        func_8004036C(o);
        return;
    }

    r = (u8 *)0x1F800398;
    ((u8 *)&o->field_0C)[2] = *(u8 *)&o->field_60;
    func_80041F90(o,
                  *(s16 *)&o->field_30.h.field_30 + *(s16 *)&o->field_18,
                  *(s16 *)&o->field_30.h.field_32 + *(s16 *)&o->field_1A,
                  (struct ProjectionOut *)0x1F800398);

    q = (u8 *)0x1F8002A0;
    b0 = (u8 *)0x1F800300;
    m = 0x55555555;
    b2 = (u8 *)0x1F800310;
    b3 = (u8 *)0x1F800318;
    b1 = (u8 *)0x1F800308;

    z = *(s32 *)&o->field_0C;
    q[3] = 6;
    *(s32 *)(q + 0x18) = m;
    *(s32 *)(q + 4) = z;
    q[7] = 0x4C;

    v = *(u16 *)&o->field_30.h.field_30 - *(u16 *)r;
    *(s16 *)b2 = v;
    *(s16 *)b0 = v;
    v += *(u16 *)&o->field_3C.h.field_3C;
    *(s16 *)b3 = v;
    *(s16 *)(b0 + 8) = v;

    /* 0x32: see the note above -- as a member read this grows .text. */
    w = *(u16 *)(bytes + 0x32) - *(u16 *)(r + 2);
    *(s16 *)(b1 + 2) = w;
    *(s16 *)(b0 + 2) = w;
    /* 0x3E: likewise, and here the code changes rather than the size. */
    e = *(u16 *)(bytes + 0x3E);
    *(s16 *)(b3 + 4) = 0;
    *(s16 *)(b2 + 4) = 0;
    *(s16 *)(b1 + 4) = 0;
    *(s16 *)(b0 + 4) = 0;

    w += e;
    *(s16 *)(b3 + 2) = w;
    *(s16 *)(b0 + 0x12) = w;

    RotAverage4(
        (SVECTOR *)0x1F800300, (SVECTOR *)0x1F800308,
        (SVECTOR *)0x1F800310, (SVECTOR *)0x1F800318,
        (long *)0x1F8002A8, (long *)0x1F8002AC,
        (long *)0x1F8002B4, (long *)0x1F8002B0,
        (long *)&sp28, (long *)&sp2C
    );

    func_8005B260((u32 *)0x1F8002A0, (GsOT *)arg1, *(u16 *)&o->field_14, 1);
    q[3] = 3;
    q[7] = 0x40;
    k = *(u16 *)&o->field_14;
    *(s32 *)(q + 0xC) = *(s32 *)(q + 0x14);
    func_8005B260((u32 *)0x1F8002A0, (GsOT *)arg1, (u16)k, 1);
}
