#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/strings.h"
#include "model_geometry_tables.h"
#include "model_ring_effect.h"
#include "model_graphics_state.h"
#include "screen_projection.h"
#include "ordering_tables.h"
#include "func_80057E20.h"
#include "func_80058E1C.h"
#include "model_copy_slot_u16_values.h"
#include "gpu_packets.h"

s32 func_8006C37C(RingWork *state, s32 command)
{
    MATRIX saved_matrix;
    MATRIX matrix;
    POLY_G3 triangle;
    POLY_G4 quad;
    SVECTOR angles;
    SVECTOR position;

    memset((u8 *)&angles, 0, sizeof(angles));
    memset((u8 *)&position, 0, sizeof(position));
    {
        VECTOR scale = {4096, 4096, 4096, 0};
        RingQueryFrameReservation dimensions;
        DVECTOR screen[67];
        u16 depths[67];
        u16 perspective[67];
        u16 flags[67];
        long flag;
        GsOT *ordering_table;
        u8 delta;
        RingWork *work = state;
        RingSettings *settings;
        s32 radius;
        s32 selected;
        u16 axis;
        s32 i;
        s32 step;
        s32 angle;
        s32 depth;
        u8 *vertex;
        u8 *inner_vertex;
        u8 *z_cursor;
        s32 red;
        s32 green;
        s32 blue;

        func_80058DCC();
        delta = func_80058E1C();
        if (command >= 0) {
            work->settings = (RingSettings *)D_80091604;
            settings = work->settings;
            func_80057E20(func_80058DCC(), &dimensions.value);
            radius = dimensions.value.y < dimensions.value.z ?
                dimensions.value.z : dimensions.value.y;
            if (radius == dimensions.value.z)
                axis = radius < dimensions.value.x ?
                    *(u16 *)&dimensions.value.x : *(u16 *)&dimensions.value.z;
            else
                axis = dimensions.value.y < dimensions.value.x ?
                    *(u16 *)&dimensions.value.x : *(u16 *)&dimensions.value.y;
            radius = (s16)axis;
            radius = 5 * radius / 4;
            selected = settings->minimum_radius;
            if (radius >= selected) {
                selected = 700;
                if (radius < 701)
                    selected = radius;
            }
            radius = selected;
            vertex = (u8 *)work + 4;
            work->vertices[0].vx = radius / 3;
            *(s16 *)(vertex + 2) = radius / 3;
            *(s16 *)(vertex + 4) = 0;
            step = 0;
            vertex = (u8 *)work + 12;
            z_cursor = (u8 *)work + 0x10;
            outer_ring:
            {
                angle = step << 7;
                *(s16 *)vertex = radius * ccos(angle) / 4096;
                *(s16 *)(z_cursor - 2) = radius * csin(angle) / 4096;
                *(s16 *)z_cursor = radius * csin(angle) / 32768;
                z_cursor += 8;
                step++;
                vertex += 8;
            }
            if (step < 33)
                goto outer_ring;
            step = 0;
            inner_vertex = vertex;
            z_cursor = (u8 *)inner_vertex + 4;
            radius = 4 * radius / 5;
            inner_ring:
            {
                angle = step << 7;
                *(s16 *)inner_vertex = radius * ccos(angle) / 4096;
                *(s16 *)(z_cursor - 2) = radius * csin(angle) / 4096;
                *(s16 *)z_cursor = radius * csin(angle) / 32768;
                z_cursor += 8;
                step++;
                inner_vertex += 8;
            }
            if (step < 33)
                goto inner_ring;
            work->color.b = 17 * (command % 10 + 6);
            work->color.r = 17 * ((command - command % 100) / 100 + 6);
            work->color.g = 17 * ((command % 100 - command % 10) / 10 + 6);
            work->fade = settings->fade_divisor;
            work->growth = 0;
            work->phase = 0;
            return 0;
        }

        settings = work->settings;
        ordering_table = func_80058F10();
        SetPolyG3(&triangle);
        SetPolyG4(&quad);
        PushMatrix();
        saved_matrix = *(MATRIX *)func_80059220();
        GsSetLsMatrix(&saved_matrix);
        angles.vx = 0;
        angles.vy = 0;
        angles.vz = 0;
        radius = (work->growth << 12) / settings->growth_divisor +
            csin((work->phase & 3) << 10) / 16;
        scale.vx = radius;
        scale.vy = radius;
        scale.vz = radius;
        Model_CopySlotU16Values(func_80058DCC(), (u16 *)&position);
        GsSetLsMatrix(&saved_matrix);
        RotTrans(&position, (VECTOR *)&matrix.t[0], &flag);
        RotMatrix(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        red = work->color.r * work->fade / settings->fade_divisor *
            work->growth / settings->growth_divisor;
        green = work->color.g * work->fade / settings->fade_divisor *
            work->growth / settings->growth_divisor;
        blue = work->color.b * work->fade / settings->fade_divisor *
            work->growth / settings->growth_divisor;
        triangle.r0 = red;
        triangle.g0 = green;
        triangle.b0 = blue;
        red = settings->color.r * work->fade /
            settings->fade_divisor * work->growth / settings->growth_divisor;
        green = settings->color.g * work->fade /
            settings->fade_divisor * work->growth / settings->growth_divisor;
        blue = settings->color.b * work->fade /
            settings->fade_divisor * work->growth / settings->growth_divisor;
        triangle.r1 = red;
        triangle.r2 = red;
        triangle.g1 = green;
        triangle.g2 = green;
        triangle.b1 = blue;
        triangle.b2 = blue;
        i = 0;
        RotTransPersN(work->vertices, screen, depths, perspective, flags, 67);
        do {
            triangle.x0 = (u16)screen[0].vx;
            triangle.y0 = (u16)screen[0].vy;
            triangle.x1 = (u16)screen[i + 1].vx;
            triangle.y1 = (u16)screen[i + 1].vy;
            triangle.x2 = (u16)screen[i + 2].vx;
            triangle.y2 = (u16)screen[i + 2].vy;
            depth = AverageZ3(depths[0],
                depths[i + 1],
                depths[i + 2]);
            flag = (flags[0] | flags[1] | flags[2]) & 0x20;
            depth /= 2;
            if (depth >= 0 && !flag)
                func_8005B260((u32 *)&triangle, ordering_table, (u16)depth, 1);
            i++;
        } while (i < 32);
        PopMatrix();
        work->phase++;
        if (command == -2) {
            work->fade -= delta;
        } else {
            s32 next_growth = work->growth + delta;
            work->growth = next_growth;
            next_growth = next_growth < 0 ? 0 :
                (next_growth > settings->growth_divisor ? settings->growth_divisor : next_growth);
            work->growth = next_growth;
        }
        return 2 * (work->fade < 1);
    }
}
