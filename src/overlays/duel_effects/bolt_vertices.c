#include "../../types.h"
#include "utility_helpers.h"
#include "../../psyq/rand.h"

/* Builds a jagged column of `count` vertices for the bolt effects: the base
 * sits at (x, -height, z) with x and z jittered by `spread`, and every later
 * vertex steps down the column and lies on a ring of random radius around
 * it. csin is called inside the multiply's ternary condition, so the
 * multiplicand is sign-extended before the call, as in retail. */

void func_8014FABC(u16 radius, u16 spread, u16 height, u16 count, SVECTOR *vertices)
{
    s16 x;
    s16 z;
    s16 step;
    s16 a;
    s16 b;
    u16 i;
    s32 s;
    s32 t;
    s32 n;
    SVECTOR *v;

    x = spread * ((rand() - rand()) % 4096) / 4096;
    z = spread * ((rand() - rand()) % 4096) / 4096;
    step = (height + 16) / count;
    vertices[0].vx = x;
    vertices[0].vy = -height;
    vertices[0].vz = z;
    for (i = 1; i < count; i++) {
        a = radius + (radius >> 1) * ((rand() - rand()) % 4096) / 4096;
        do {
            b = radius + (radius >> 1) * ((rand() - rand()) % 4096) / 4096;
            n = 2048 / count * i;
            vertices[i].vx = x + a * ((s = csin(n), x < 0) ? (s = -s) : s) / 4096;
        } while (0);
        do {
            t = 2048 / count;
            v = (SVECTOR *)(i * sizeof(SVECTOR) + (s32)vertices);
            v->vy = step * i - height;
            v->vz = z + b * ((s = csin(t * i), z < 0) ? (s = -s) : s) / 4096;
        } while (0);
    }
}
