#include "../../types.h"
#include "../../psyq/rand.h"
#include "effect_geometry.h"
#include "color_semantic_views.h"
#include "geometry_semantic_views.h"
#include "number_semantic_views.h"
#include "texture_semantic_views.h"

void DuelEffect_InitXZRingVertices(u16 width, u16 depth, s16 height, SVECTOR *vertices, u16 count)
{
    s32 i;
    s32 angle;

    for (i = 0; i < count; i++) {
        angle = (4096 / count) * i;
        vertices->vx = width * ccos(angle) / 4096;
        vertices->vy = -height;
        vertices->vz = depth * csin(angle) / 4096;
        vertices++;
    }
}

void DuelEffect_InitRandomVectorsUnscaled(u16 count, SVECTOR *vertices)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vertices[i].vx = (rand() - rand()) % 4096;
        vertices[i].vy = (rand() - rand()) % 4096;
        vertices[i].vz = (rand() - rand()) % 4096;
    }
}

void DuelEffect_SetGrayColor(CVECTOR *color, u8 value)
{
    color->r = value;
    color->g = value;
    color->b = value;
}

void DuelEffect_SetRgbColor(CVECTOR *color, u8 red, u8 green, u8 blue)
{
    color->r = red;
    color->g = green;
    color->b = blue;
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

s32 DuelEffect_PowS16(s16 base, u16 exponent)
{
    s32 result = 1;
    s32 i;

    for (i = 0; i < exponent; i++) {
        result *= base;
    }
    return (s16)result;
}

void DuelEffect_PackTexturePageClut(u16 *output, GsIMAGE *image, s32 mode)
{
    output[0] = ((image->pmode & 3) << 7) |
                ((mode & 3) << 5) |
                (((s32)((u16)image->py & 0x100) << 16) >> 20) |
                ((image->px & 0x3FF) >> 6) |
                ((image->py & 0x200) << 2);
    output[1] = (image->cy << 6) |
                ((image->cx >> 4) & 0x3F);
}

void DuelEffect_PackFixedPageClut(u16 *output, GsIMAGE *image)
{
    output[0] = ((image->pmode & 3) << 7) | 0x3E;
    output[1] = (image->cy << 6) |
                ((image->cx >> 4) & 0x3F);
}

void DuelEffect_InitRandomVectorsXYZ(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vectors[i].vx = x_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vy = y_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vz = z_scale * ((rand() - rand()) % 4096) / 4096;
    }
}

void DuelEffect_InitRandomVectorsNegatedY(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vectors[i].vx = x_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vy = -y_scale * (rand() % 4096) / 4096;
        vectors[i].vz = z_scale * ((rand() - rand()) % 4096) / 4096;
    }
}

void DuelEffect_InitRandomVectorsXY(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vectors[i].vx = x_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vy = y_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vz = 0;
    }
}

void DuelEffect_InitIndexedQuadVertices(SVECTOR *vertices, u16 top_width, u16 bottom_width,
                    u16 top_height, u16 bottom_height, s16 *widths, s16 *heights)
{
    vertices[0].vx = -widths[top_width];
    vertices[0].vy = heights[top_height];
    vertices[0].vz = 0;
    vertices[1].vx = widths[top_width];
    vertices[1].vy = heights[top_height];
    vertices[1].vz = 0;
    vertices[2].vx = -widths[bottom_width];
    vertices[2].vy = heights[bottom_height];
    vertices[2].vz = 0;
    vertices[3].vx = widths[bottom_width];
    vertices[3].vy = heights[bottom_height];
    vertices[3].vz = 0;
}

void DuelEffect_InitIndexedQuadToYZero(SVECTOR *vertices, u16 top_width, u16 bottom_width,
                    u16 top_height, s16 *widths, s16 *heights)
{
    vertices[0].vx = -widths[top_width];
    vertices[0].vy = heights[top_height];
    vertices[0].vz = 0;
    vertices[1].vx = widths[top_width];
    vertices[1].vy = heights[top_height];
    vertices[1].vz = 0;
    vertices[2].vx = -widths[bottom_width];
    vertices[2].vy = 0;
    vertices[2].vz = 0;
    vertices[3].vx = widths[bottom_width];
    vertices[3].vy = 0;
    vertices[3].vz = 0;
}

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
            /* Integer address arithmetic keeps the retail addu operand order. */
            vertex = (SVECTOR *)(i * sizeof(SVECTOR) + (s32)vertices);
            vertex->vy = step * i - height;
            vertex->vz = z + radius_z * ((sine = csin(angle_step * i), z < 0) ? (sine = -sine) : sine) / 4096;
        } while (0);
    }
}

void DuelEffect_InitQuadrantQuadVertices(SVECTOR *vertices, u16 size, u16 index)
{
    vertices[0].vx = (s16)DuelEffect_PowS16(-1, index + 1) * size;
    vertices[0].vy = (index - 2 < 0 ? -1 : 1) * size;
    vertices[0].vz = 0;
    vertices[1].vx = 0;
    vertices[1].vy = (index - 2 < 0 ? -1 : 1) * size;
    vertices[1].vz = 0;
    vertices[2].vx = (s16)DuelEffect_PowS16(-1, index + 1) * size;
    vertices[2].vy = 0;
    vertices[2].vz = 0;
    vertices[3].vx = 0;
    vertices[3].vy = 0;
    vertices[3].vz = 0;
}

void DuelEffect_SwapVectorRecords(SVECTOR *first, SVECTOR *second)
{
    SVECTOR temporary;

    temporary = *first;
    *first = *second;
    *second = temporary;
}
