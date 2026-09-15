#include "../types.h"
#include "display_object_projection.h"
#include "../psyq/libgte.h"
#include "gpu_packets.h"
#include "display_object_core.h"
#include "display_object.h"
#include "func_8002A9C0.h"

/* The update callback func_8002ABB4 installs at 0x4C on the object it
   builds: it fades the object out over its own field_60 countdown, projects
   it, and emits a four-vertex flat polyline (one colour word, length 6)
   and then, with the length cut to 3 and x3/y3 copied into x1/y1, a single
   flat line from the first vertex to the last, both through func_8005B260.
   The record is the canonical DisplayObject.

   The scratchpad side is typed as well: a LINE_F4 at 0x1F8002A0, the four
   SVECTOR corners RotAverage4 projects into its x0..x3, and the
   ProjectionOut func_80041F90 leaves at 0x1F800398. That is load-bearing.
   Written through byte pointers, those stores are not struct references, and
   GCC floats the object's struct-member reads of 0x32 and 0x3E across them
   where the target does not. With both sides typed the plain member reads
   keep their place.

   b0[1].vx and b0[2].vy are b1->vx and b2->vy: the target addresses those
   two stores from the first corner's base, and spelling them through b1 and
   b2 changes the code. */
void func_8002A9C0(DisplayObject *o, s32 arg1)
{
    s32 sp28;
    s32 sp2C;
    LINE_F4 *q;
    struct ProjectionOut *r;
    SVECTOR *b0;
    SVECTOR *b1;
    SVECTOR *b2;
    SVECTOR *b3;
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

    r = (struct ProjectionOut *)0x1F800398;
    ((u8 *)&o->field_0C)[2] = *(u8 *)&o->field_60;
    func_80041F90(o,
                  *(s16 *)&o->field_30.h.field_30 + *(s16 *)&o->field_18,
                  *(s16 *)&o->field_30.h.field_32 + *(s16 *)&o->field_1A,
                  r);

    q = (LINE_F4 *)0x1F8002A0;
    b0 = (SVECTOR *)0x1F800300;
    m = 0x55555555;
    b2 = (SVECTOR *)0x1F800310;
    b3 = (SVECTOR *)0x1F800318;
    b1 = (SVECTOR *)0x1F800308;

    z = *(s32 *)&o->field_0C;
    setlen(q, 6);
    q->pad = m;
    *(s32 *)&q->r0 = z;
    q->code = 0x4C;

    v = o->field_30.h.field_30 - (u16)r->f0;
    b2->vx = v;
    b0->vx = v;
    v += o->field_3C.h.field_3C;
    b3->vx = v;
    b0[1].vx = v;

    w = o->field_30.h.field_32 - (u16)r->f2;
    b1->vy = w;
    b0->vy = w;
    e = o->field_3C.h.field_3E;
    b3->vz = 0;
    b2->vz = 0;
    b1->vz = 0;
    b0->vz = 0;

    w += e;
    b3->vy = w;
    b0[2].vy = w;

    RotAverage4(
        b0, b1, b2, b3,
        (long *)&q->x0, (long *)&q->x1,
        (long *)&q->x3, (long *)&q->x2,
        (long *)&sp28, (long *)&sp2C
    );

    func_8005B260((u32 *)q, (GsOT *)arg1, *(u16 *)&o->field_14, 1);
    setlen(q, 3);
    q->code = 0x40;
    k = *(u16 *)&o->field_14;
    *(s32 *)&q->x1 = *(s32 *)&q->x3;
    func_8005B260((u32 *)q, (GsOT *)arg1, (u16)k, 1);
}
