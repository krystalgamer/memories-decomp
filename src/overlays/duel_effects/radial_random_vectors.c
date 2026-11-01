#include "../../types.h"
#include "utility_helpers.h"
#include "../../psyq/rand.h"
#include "geometry_semantic_views.h"

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
