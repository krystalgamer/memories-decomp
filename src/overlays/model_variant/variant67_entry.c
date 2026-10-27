#include "../../types.h"
#include "variant67_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Model67State *work;
    Model67Command *cmd;
    MATRIX view, matrix;
    POLY_GT4 strip;
    POLY_FT4 quad;
    DVECTOR screen[32];
    u16 depths[32];
    u16 projection[32];
    u16 flags[32];
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[4];
    SVECTOR base;
    PSXLONG flag, interpolation;
    GsOT *ot;
    SVECTOR *p, *ring;
    DVECTOR *q;
    s32 i, j, k, t, depth;
    s32 a, b, c, n;
    s32 top, bottom;

    work = (Model67State *)context;
    Model_GetActiveSlotIndex();
    if (command >= 0) {
        cmd = work->command = &D_8013C934[command];
        p = work->ribbon[0];
        for (j = 0; j < 8; j++) {
            for (i = 0; i < 16; p++, i++) {
                p->vx = (cmd->top_radius - (cmd->top_radius - cmd->base_radius) * ccos(j << 7) / 4096) * ccos(i << 8) / 4096;
                p->vy = -cmd->height * j / 8;
                p->vz = (cmd->top_radius - (cmd->top_radius - cmd->base_radius) * ccos(j << 7) / 4096) * csin(i << 8) / 4096;
            }
        }
        for (i = 0; i < 4; i++) {
            work->color[i].r = work->color[7 - i].r = work->color[8 + i].r = work->color[15 - i].r =
                (cmd->ribbon_red - cmd->edge_red) * i / 4;
            work->color[i].g = work->color[7 - i].g = work->color[8 + i].g = work->color[15 - i].g =
                (cmd->ribbon_green - cmd->edge_green) * i / 4;
            work->color[i].b = work->color[7 - i].b = work->color[8 + i].b = work->color[15 - i].b =
                (cmd->ribbon_blue - cmd->edge_blue) * i / 4;
        }
        p = work->ring;
        for (i = 0; i < 8; p++, i++) {
            p->vx = cmd->ring_radius * i / 8 * ccos(i << 9) / 4096;
            p->vy = 0;
            p->vz = cmd->ring_radius * i / 8 * csin(i << 9) / 4096;
        }
        for (i = 0; i < 16; i++) {
            work->frame[i] = rand() % 8;
        }
        work->elapsed = -cmd->delay;
        work->done = 0;
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C8E0[i]);
        }
        return 0;
    }
    cmd = work->command;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    SetPolyGT4(&strip);
    SetPolyFT4(&quad);
    PushMatrix();
    view = *(MATRIX *)Model_GetLightSourceMatrix();
    t = work->elapsed - cmd->open_time;
    if (t >= 0) {
        strip.tpage = work->texture[0] >> 16;
        strip.clut = work->texture[0];
        p = work->ribbon[0];
        ring = work->ring;
        rotation.vx = 0;
        rotation.vy = 0x1000 - t * cmd->spin_speed % 4096;
        rotation.vz = 0;
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        if (work->elapsed < cmd->grow_end) {
            scale.vx = scale.vy = scale.vz = (t << 12) / (cmd->grow_end - cmd->open_time);
        } else if (cmd->fade_start < work->elapsed) {
            scale.vx = scale.vz = ((work->elapsed - cmd->fade_start) << 13) / (cmd->end - cmd->fade_start) + 0x1000;
        }
        GsSetLsMatrix(&view);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), cmd->slot), &matrix);
        if (work->elapsed < cmd->grow_end) {
            position.vx = matrix.t[0];
            position.vy = 0;
            position.vz = matrix.t[2];
        } else {
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corners[0]);
            if (work->elapsed < cmd->grow_end + cmd->move_time) {
                position.vx = matrix.t[0] - matrix.t[0] * (work->elapsed - cmd->grow_end) / cmd->move_time;
                position.vy = 0;
                position.vz = matrix.t[2] + (corners[0].vz - matrix.t[2]) * (work->elapsed - cmd->grow_end) / cmd->move_time;
            } else {
                position.vx = 0;
                position.vy = 0;
                position.vz = corners[0].vz;
            }
        }
        position.vx += ring->vx;
        position.vy += ring->vy;
        position.vz += ring->vz;
        GsSetLsMatrix(&view);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&view, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(p, &screen[16], &depths[16], &projection[16], &flags[16], 16);
        p += 16;
        for (i = 1; i < 8; i++) {
            GsSetLsMatrix(&view);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), cmd->slot), &matrix);
            if (work->elapsed < cmd->grow_end) {
                position.vx = matrix.t[0];
                position.vy = 0;
                position.vz = matrix.t[2];
            } else {
                Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corners[0]);
                if (work->elapsed < cmd->grow_end + cmd->move_time) {
                    position.vx = matrix.t[0] - matrix.t[0] * (work->elapsed - cmd->grow_end) / cmd->move_time;
                    position.vy = 0;
                    position.vz = matrix.t[2] + (corners[0].vz - matrix.t[2]) * (work->elapsed - cmd->grow_end) / cmd->move_time;
                } else {
                    position.vx = 0;
                    position.vy = 0;
                    position.vz = corners[0].vz;
                }
            }
            position.vx += ring->vx;
            position.vy += ring->vy;
            position.vz += ring->vz;
            GsSetLsMatrix(&view);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&view, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(p, screen, depths, projection, flags, 16);
            SetSemiTrans(&strip, 1);
            for (j = 0; j < 16; j++) {
                a = j + 16;
                n = j + 1;
                c = n % 16;
                b = n % 16 + 16;
                strip.x0 = screen[a].vx;
                strip.y0 = screen[a].vy;
                strip.x1 = screen[b].vx;
                strip.y1 = screen[b].vy;
                strip.x2 = screen[j].vx;
                strip.y2 = screen[j].vy;
                strip.x3 = screen[c].vx;
                strip.y3 = screen[c].vy;
                depth = AverageZ4(depths[a], depths[b], depths[j], depths[c]);
                flag = (flags[a] | flags[b] | flags[j] | flags[c]) & 0x20;
                {
                    s32 x = j - 1;
                    a = (i + x) % 16;
                }
                c = (i + j) % 16;
                b = (i + n) % 16;
                strip.r0 = work->color[a].r;
                strip.g0 = work->color[a].g;
                strip.b0 = work->color[a].b;
                strip.r1 = work->color[c].r;
                strip.g1 = work->color[c].g;
                strip.b1 = work->color[c].b;
                strip.r2 = work->color[c].r;
                strip.g2 = work->color[c].g;
                strip.b2 = work->color[c].b;
                strip.r3 = work->color[b].r;
                strip.g3 = work->color[b].g;
                strip.b3 = work->color[b].b;
                strip.u0 = (a * 16) % 128;
                strip.v0 = (i - 1) * 16 % 128;
                strip.u1 = (c * 16) % 128;
                strip.v1 = (i - 1) * 16 % 128;
                strip.u2 = (c * 16) % 128;
                strip.v2 = i * 16 % 128;
                strip.u3 = (b * 16) % 128;
                strip.v3 = i * 16 % 128;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&strip, ot, depth);
                }
            }
            for (j = 0; j < 16; j++) {
                screen[j + 16] = screen[j];
            }
            p += 16;
            ring++;
        }
        p = work->ring;
        for (i = 0; i < 8; p++, i++) {
            p->vx = cmd->ring_radius * i / 8 * ccos((i << 9) + cmd->wobble_speed * work->elapsed % 4096) / 4096;
            p->vy = 0;
            p->vz = cmd->ring_radius * i / 8 * csin((i << 9) + cmd->wobble_speed * work->elapsed % 4096) / 4096;
        }
        if (cmd->fade_start < work->elapsed && work->elapsed < cmd->end) {
            for (i = 0; i < 16; i++) {
                work->color[i].r -= work->color[i].r / (cmd->end - work->elapsed) * 2;
                work->color[i].g -= work->color[i].g / (cmd->end - work->elapsed) * 2;
                work->color[i].b -= work->color[i].b / (cmd->end - work->elapsed) * 2;
            }
        }
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), cmd->slot), &matrix);
        p = work->ribbon[0];
        if (work->elapsed < cmd->grow_end) {
            base.vx = matrix.t[0];
            base.vy = 0;
            base.vz = matrix.t[2];
        } else {
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&corners[0]);
            if (work->elapsed < cmd->grow_end + cmd->move_time) {
                base.vx = matrix.t[0] - matrix.t[0] * (work->elapsed - cmd->grow_end) / cmd->move_time;
                base.vy = 0;
                base.vz = matrix.t[2] + (corners[0].vz - matrix.t[2]) * (work->elapsed - cmd->grow_end) / cmd->move_time;
            } else {
                base.vx = 0;
                base.vy = 0;
                base.vz = corners[0].vz;
            }
        }
        setVector(&corners[0], -cmd->spark_size / 2, -cmd->spark_size / 2, 0);
        setVector(&corners[1], cmd->spark_size / 2, -cmd->spark_size / 2, 0);
        setVector(&corners[2], -cmd->spark_size / 2, cmd->spark_size / 2, 0);
        setVector(&corners[3], cmd->spark_size / 2, cmd->spark_size / 2, 0);
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        setSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        for (i = 0; i < 16; i++, p++) {
            if (work->elapsed <= cmd->fade_start || work->frame[i] != 0) {
                position.vx = base.vx;
                position.vy = base.vy;
                position.vz = base.vz;
                position.vx += work->ring[0].vx;
                position.vy += work->ring[0].vy;
                position.vz += work->ring[0].vz;
                position.vx += p->vx;
                position.vy += p->vy;
                position.vz += p->vz;
                position.vy -= cmd->spark_size * (7 - work->frame[i]) / 24;
                GsSetLsMatrix(&view);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                                    &interpolation, &flag);
                quad.u0 = work->frame[i] << 5;
                quad.v0 = 0x80;
                quad.u1 = (work->frame[i] << 5) + 0x1F;
                quad.v1 = 0x80;
                quad.u2 = work->frame[i] << 5;
                quad.v2 = 0x9F;
                quad.u3 = (work->frame[i] << 5) + 0x1F;
                quad.v3 = 0x9F;
                k = 8 - work->frame[i];
                {
                    u8 r, g, b;
                    r = cmd->spark_red * k / 8;
                    g = cmd->spark_green * k / 8;
                    b = cmd->spark_blue * k / 8;
                    quad.r0 = r;
                    quad.g0 = g;
                    quad.b0 = b;
                }
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
                work->frame[i] = (work->frame[i] + 15) % 16;
            }
        }
    }
    if (work->elapsed < cmd->grow_end) {
        setVector(&corners[0], -cmd->flash_size, 0, -cmd->flash_size);
        setVector(&corners[1], cmd->flash_size, 0, -cmd->flash_size);
        setVector(&corners[2], -cmd->flash_size, 0, cmd->flash_size);
        setVector(&corners[3], cmd->flash_size, 0, cmd->flash_size);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), cmd->slot), &matrix);
        position.vx = matrix.t[0];
        position.vy = 0;
        position.vz = matrix.t[2];
        {
            s32 x = (work->elapsed << 7) / cmd->open_time;
            a = x * x;
        }
        rotation.vx = 0;
        rotation.vy = a;
        rotation.vz = 0;
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        GsSetLsMatrix(&view);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&view, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                            &interpolation, &flag);
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        quad.u0 = 0x80;
        quad.v0 = 0;
        quad.u1 = 0xBF;
        quad.v1 = 0;
        quad.u2 = 0x80;
        quad.v2 = 0x3F;
        quad.u3 = 0xBF;
        quad.v3 = 0x3F;
        {
        u8 r, g, b;
        if (work->elapsed < cmd->open_time) {
            r = cmd->flash_red * work->elapsed / cmd->open_time;
            g = cmd->flash_green * work->elapsed / cmd->open_time;
            b = cmd->flash_blue * work->elapsed / cmd->open_time;
        } else {
            r = cmd->flash_red * (cmd->grow_end - work->elapsed) / (cmd->grow_end - cmd->open_time);
            g = cmd->flash_green * (cmd->grow_end - work->elapsed) / (cmd->grow_end - cmd->open_time);
            b = cmd->flash_blue * (cmd->grow_end - work->elapsed) / (cmd->grow_end - cmd->open_time);
        }
        quad.r0 = r;
        quad.g0 = g;
        quad.b0 = b;
        }
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, depth);
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < cmd->grow_end + cmd->move_time) {
        return 0;
    }
    if (cmd->end < work->elapsed && work->done != 0) {
        return 2;
    }
    if (cmd->fade_start < work->elapsed) {
        if (work->done != 0) {
            return 0;
        }
        work->done = 1;
        return 1;
    }
    return 4;
}
