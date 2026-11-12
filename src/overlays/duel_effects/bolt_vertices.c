#include "../../types.h"
#include "utility_helpers.h"
#include "../../psyq/rand.h"

/* Builds a jagged column of `count` vertices for the bolt effects: the base
 * sits at (x, -height, z) with x and z jittered by `spread`, and every later
 * vertex steps down the column and lies on a ring of random radius around
 * it. csin is called inside the multiply's ternary condition, so the
 * multiplicand is sign-extended before the call, as in retail. */

void DuelEffect_InitJaggedColumnVertices(u16 radius, u16 spread, u16 height, u16 count, SVECTOR *vertices)
{
    s16 x;
    s16 z;
    s16 step;
    s16 radius_x;
    s16 radius_z;
    u16 i;
    s32 sine;
    s32 angle_step;
    s32 angle;
    SVECTOR *vertex;

    x = spread * ((rand() - rand()) % 4096) / 4096;
    z = spread * ((rand() - rand()) % 4096) / 4096;
    step = (height + 16) / count;
    vertices[0].vx = x;
    vertices[0].vy = -height;
    vertices[0].vz = z;
    for (i = 1; i < count; i++) {
        radius_x = radius + (radius >> 1) * ((rand() - rand()) % 4096) / 4096;
        do {
            radius_z = radius + (radius >> 1) * ((rand() - rand()) % 4096) / 4096;
            angle = 2048 / count * i;
            vertices[i].vx = x + radius_x * ((sine = csin(angle), x < 0) ? (sine = -sine) : sine) / 4096;
        } while (0);
        do {
            angle_step = 2048 / count;
            vertex = (SVECTOR *)(i * sizeof(SVECTOR) + (s32)vertices);
            vertex->vy = step * i - height;
            vertex->vz = z + radius_z * ((sine = csin(angle_step * i), z < 0) ? (sine = -sine) : sine) / 4096;
        } while (0);
    }
}
