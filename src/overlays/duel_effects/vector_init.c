#include "../../types.h"
#include "../../psyq/rand.h"
#include "utility_helpers.h"

void func_8014F608(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vectors[i].vx = x_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vy = y_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vz = z_scale * ((rand() - rand()) % 4096) / 4096;
    }
}

void func_8014F754(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vectors[i].vx = x_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vy = -y_scale * (rand() % 4096) / 4096;
        vectors[i].vz = z_scale * ((rand() - rand()) % 4096) / 4096;
    }
}

void func_8014F89C(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 count)
{
    s32 i;

    for (i = 0; i < count; i++) {
        vectors[i].vx = x_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vy = y_scale * ((rand() - rand()) % 4096) / 4096;
        vectors[i].vz = 0;
    }
}

void func_8014F9A0(SVECTOR *vertices, u16 top_width, u16 bottom_width,
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

void func_8014FA3C(SVECTOR *vertices, u16 top_width, u16 bottom_width,
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
