#include "../../types.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"
#include "color_semantic_views.h"
#include "geometry_semantic_views.h"
#include "number_semantic_views.h"

void DuelEffect_SetGrayColor(u8 *color, u8 value)
{
    color[0] = value;
    color[1] = value;
    color[2] = value;
}

void DuelEffect_SetRgbColor(u8 *color, u8 red, u8 green, u8 blue)
{
    color[0] = red;
    color[1] = green;
    color[2] = blue;
}

void DuelEffect_InitParticleVectors(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count)
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

void DuelEffect_InitParticleVectorsNegatedY(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count)
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

void DuelEffect_InitOffsetQuadVertices(SVECTOR *vertices, SVECTOR *offset)
{
    s32 i;

    DuelEffect_InitSquareVertices(vertices, 4);
    for (i = 0; i < 4; i++) {
        vertices->vx += offset->vx;
        vertices->vy += offset->vy;
        vertices->vz += offset->vz;
        vertices++;
    }
}

void DuelEffect_InitSquareVertices(SVECTOR *vertices, u16 size)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        vertices[i].vx = (s16)DuelEffect_PowS16(-1, i + 1) * size;
        vertices[i].vy = (i - 2 < 0 ? -1 : 1) * size;
        vertices[i].vz = 0;
    }
}

void DuelEffect_InitXZRectVertices(SVECTOR *vertices, u16 width, u16 height, u16 depth)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        vertices[i].vx = (s16)DuelEffect_PowS16(-1, i + 1) * width;
        vertices[i].vy = height;
        vertices[i].vz = (i - 2 < 0 ? -1 : 1) * depth;
    }
}

void DuelEffect_InitRectVertices(SVECTOR *vertices, s32 width, s16 height)
{
    s32 i;
    s32 horizontal = width;

    for (i = 0; i < 4; i++) {
        vertices[i].vx = horizontal * DuelEffect_PowS16(-1, i + 1);
        vertices[i].vy = (i - 2 < 0 ? -1 : 1) * height;
        vertices[i].vz = 0;
    }
}
