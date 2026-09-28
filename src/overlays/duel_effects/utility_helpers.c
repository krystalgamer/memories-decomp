#include "../../types.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"

void func_8014F010(u8 *color, u8 value)
{
    color[0] = value;
    color[1] = value;
    color[2] = value;
}

void func_8014F020(u8 *color, u8 red, u8 green, u8 blue)
{
    color[0] = red;
    color[1] = green;
    color[2] = blue;
}

void func_8014F030(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        positions[i].vx = 0;
        positions[i].vy = 0;
        positions[i].vz = 0;
        velocities[i].vx = scale * ((rand() - rand()) % 4096) / 4096;
        velocities[i].vy = scale * ((rand() - rand()) % 4096) / 4096;
        velocities[i].vz = scale * ((rand() - rand()) % 4096) / 4096;
    }
}

void func_8014F180(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        positions[i].vx = 0;
        positions[i].vy = 0;
        positions[i].vz = 0;
        velocities[i].vx = scale * ((rand() - rand()) % 4096) / 4096;
        velocities[i].vy = -scale * (rand() % 4096) / 4096;
        velocities[i].vz = scale * ((rand() - rand()) % 4096) / 4096;
    }
}

void func_8014F2D4(SVECTOR *vertices, SVECTOR *offset)
{
    s32 i;

    func_8014F358(vertices, 4);
    for (i = 0; i < 4; i++) {
        vertices->vx += offset->vx;
        vertices->vy += offset->vy;
        vertices->vz += offset->vz;
        vertices++;
    }
}

void func_8014F358(SVECTOR *vertices, u16 size)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        vertices[i].vx = (s16)func_8014F524(-1, i + 1) * size;
        vertices[i].vy = (i - 2 < 0 ? -1 : 1) * size;
        vertices[i].vz = 0;
    }
}

void func_8014F3E8(SVECTOR *vertices, u16 width, u16 height, u16 depth)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        vertices[i].vx = (s16)func_8014F524(-1, i + 1) * width;
        vertices[i].vy = height;
        vertices[i].vz = (i - 2 < 0 ? -1 : 1) * depth;
    }
}
