#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/strings.h"
#include "../psyq/stdio.h"
#include "model_geometry_tables.h"
#include "func_80057E20.h"
#include "func_80058E1C.h"
#include "model_copy_slot_u16_values.h"
#include "gpu_packets.h"
#include "model_subdivided_effect.h"
#include "triangle_subdivision.h"
#include "model_graphics_state.h"
#include "screen_projection.h"

#define CLAMP(value, lower, upper) \
    ((value) < (lower) ? (lower) : ((value) > (upper) ? (upper) : (value)))

s32 func_8006AF74(void *data, s32 mode)
{
    MATRIX base;
    MATRIX matrix;
    POLY_G3 triangle;
    POLY_G4 quad;
    GsLINE line;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    ModelEffectAdjustment bounds;
    s32 flag;
    s32 interpolation;
    s32 depth;
    GsOT *ot;
    s32 i;
    ModelSubdividedEffect *effect;
    ModelSubdividedEffectConfig *config;
    SVECTOR *vertex;
    Triplet *color;
    SVECTOR **vertices;
    u8 **colors;
    s32 height;
    s32 elapsed;
    s32 remaining;
    s32 work;
    s32 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8001185C;
    effect = data;
    func_80058DCC();
    {
        s32 ticks = func_80058E1C();
        if (mode >= 0) {
            config = effect->config = (ModelSubdividedEffectConfig *)D_800915E8;
            func_80057E20(func_80058DCC(), &bounds);
            work = height = bounds.max;
            work /= 2;
            height /= 2;
            work = CLAMP(work, config->minimum_radius, 700);
            height = CLAMP(height, config->minimum_height, 700);
            vertex = &effect->vertices[0];
            vertex->vx = 0;
            vertex->vy = -height;
            vertex->vz = 0;
            vertex = &effect->vertices[1];
            vertex->vx = 0;
            vertex->vy = 0;
            vertex->vz = -work;
            vertex = &effect->vertices[2];
            vertex->vx = -work;
            vertex->vy = 0;
            vertex->vz = 0;
            vertex = &effect->vertices[3];
            vertex->vx = 0;
            vertex->vy = 0;
            vertex->vz = work;
            vertex = &effect->vertices[4];
            vertex->vx = work;
            vertex->vy = 0;
            vertex->vz = 0;
            vertex = &effect->vertices[5];
            vertex->vx = 0;
            vertex->vy = height;
            vertex->vz = 0;
            color = effect->colors;
            for (i = 0; i < 6; color++, i++) {
                func_8006C2FC(*color, config->colors[0][i],
                    config->colors[1][i], config->colors[2][i]);
            }
            vertex = effect->vertices;
            color = effect->colors;
            vertices = effect->vertex_links;
            colors = effect->color_links;
            for (i = 1; i < 5; i++) {
                *vertices = vertex;
                vertices++;
                *colors = color[0];
                colors++;
                *vertices = &vertex[i];
                vertices++;
                *colors = color[i];
                colors++;
                *vertices = &effect->vertices[i % 4] + 1;
                vertices++;
                *colors = effect->colors[i % 4] + 4;
                colors++;
            }
            for (i = 1; i < 5; i++) {
                *vertices = &vertex[5];
                vertices++;
                *colors = color[5];
                colors++;
                *vertices = &effect->vertices[i % 4] + 1;
                vertices++;
                *colors = effect->colors[i % 4] + 4;
                colors++;
                *vertices = &vertex[i];
                vertices++;
                *colors = color[i];
                colors++;
            }
            vertex = effect->subdivided_vertices;
            color = effect->subdivided_colors;
            vertices = effect->vertex_links;
            colors = effect->color_links;
            for (i = 0; i < 8; i++) {
                func_8006BCA4((s16 *)vertices[0], (s16 *)vertices[1],
                    (s16 *)vertices[2], (s16 *)vertex, 2);
                vertices += 3;
                vertex += 48;
                func_8006C120(colors[0], colors[1], colors[2], color, 2);
                colors += 3;
                color += 48;
            }
            effect->elapsed = 0;
            effect->remaining = config->fade_duration;
            effect->field_130C = 0;
            effect->field_130D = 0;
            printf(D_8001186C, 0x1318, 0x1318);
            return 0;
        }

        config = effect->config;
        vertex = effect->subdivided_vertices;
        ot = func_80058F10();
        base = *(MATRIX *)func_80059220();
        color = effect->subdivided_colors;
        PushMatrix();
        SetPolyG3(&triangle);
        SetPolyG4(&quad);
        line.attribute = 0x50000000;
        elapsed = effect->elapsed;
        remaining = effect->remaining;
        rotation.vx = 0;
        rotation.vy = (u32)elapsed << 5;
        rotation.vz = 0;
        GsSetLsMatrix(&base);
        Model_CopySlotU16Values(func_80058DCC(), (u16 *)&position);
        GsSetLsMatrix(&base);
        /* RotTrans writes three words, exactly the extent of MATRIX.t. */
        RotTrans(&position, (VECTOR *)matrix.t, (long *)&flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        {
            s32 triangle_index;
            for (triangle_index = 0; triangle_index < 128; triangle_index++) {
                work = RotAverageNclip3(vertex, vertex + 1, vertex + 2,
                    (long *)&triangle.x0, (long *)&triangle.x1, (long *)&triangle.x2,
                    (long *)&interpolation, (long *)&depth, (long *)&flag);
                if (remaining >= 0 && remaining < config->fade_duration) {
                    red = (*color)[0] * remaining / config->fade_duration;
                    green = (*color)[1] * remaining / config->fade_duration;
                    blue = (*color)[2] * remaining / config->fade_duration;
                    color++;
                    triangle.r0 = red;
                    triangle.g0 = green;
                    triangle.b0 = blue;
                    red = (*color)[0] * remaining / config->fade_duration;
                    green = (*color)[1] * remaining / config->fade_duration;
                    blue = (*color)[2] * remaining / config->fade_duration;
                    color++;
                    triangle.r1 = red;
                    triangle.g1 = green;
                    triangle.b1 = blue;
                    red = (*color)[0] * remaining / config->fade_duration;
                    green = (*color)[1] * remaining / config->fade_duration;
                    blue = (*color)[2] * remaining / config->fade_duration;
                    color++;
                    triangle.r2 = red;
                    triangle.g2 = green;
                    triangle.b2 = blue;
                } else if (elapsed >= 0 && elapsed < config->growth_duration) {
                    red = (*color)[0] * elapsed / config->growth_duration;
                    green = (*color)[1] * elapsed / config->growth_duration;
                    blue = (*color)[2] * elapsed / config->growth_duration;
                    color++;
                    triangle.r0 = red;
                    triangle.g0 = green;
                    triangle.b0 = blue;
                    red = (*color)[0] * elapsed / config->growth_duration;
                    green = (*color)[1] * elapsed / config->growth_duration;
                    blue = (*color)[2] * elapsed / config->growth_duration;
                    color += 2;
                    triangle.r1 = red;
                    triangle.r2 = red;
                    triangle.g1 = green;
                    triangle.g2 = green;
                    triangle.b1 = blue;
                    triangle.b2 = blue;
                } else {
                    triangle.r0 = (*color)[0];
                    triangle.g0 = (*color)[1];
                    triangle.b0 = (*color)[2];
                    color++;
                    triangle.r1 = (*color)[0];
                    triangle.g1 = (*color)[1];
                    triangle.b1 = (*color)[2];
                    color++;
                    triangle.r2 = (*color)[0];
                    triangle.g2 = (*color)[1];
                    triangle.b2 = (*color)[2];
                    color++;
                }
                if (depth >= 0 && flag >= 0 && work > 0) {
                    func_8005B260((u32 *)&triangle, ot, (u16)depth, 1);
                }
                vertex += 3;
            }
        }
        if (remaining >= 0 && remaining < config->fade_duration) {
            line.r = (remaining << 5) / config->fade_duration;
            line.g = (remaining << 5) / config->fade_duration;
            line.b = (remaining << 5) / config->fade_duration;
        } else if (elapsed >= 0 && elapsed < config->growth_duration) {
            line.r = (elapsed << 5) / config->growth_duration;
            line.g = (elapsed << 5) / config->growth_duration;
            line.b = (elapsed << 5) / config->growth_duration;
        } else {
            line.r = 32;
            line.g = 32;
            line.b = 32;
        }
        vertices = effect->vertex_links;
        for (i = 0; i < 8; i++) {
            work = RotAverageNclip3(vertices[0], vertices[1], vertices[2],
                (long *)&triangle.x0, (long *)&triangle.x1, (long *)&triangle.x2,
                (long *)&interpolation, (long *)&depth, (long *)&flag);
            if (depth >= 0 && flag >= 0 && work > 0) {
                line.x0 = triangle.x0;
                line.y0 = triangle.y0;
                line.x1 = triangle.x1;
                line.y1 = triangle.y1;
                GsSortLine(&line, ot, (u16)depth);
                line.x0 = triangle.x2;
                line.y0 = triangle.y2;
                GsSortLine(&line, ot, (u16)depth);
                line.x1 = triangle.x0;
                line.y1 = triangle.y0;
                GsSortLine(&line, ot, (u16)depth);
            }
            vertices += 3;
        }
        PopMatrix();
        effect->elapsed += ticks;
        if (mode == -2) {
            effect->remaining -= ticks;
        }
        return (effect->remaining < 0) * 2;
    }
}
