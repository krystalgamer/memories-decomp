#include "../../types.h"
#include "variant144_entry.h"
#include "../../game/graphics_constants.h"

/* Header 144's entry: five rings fire one after another from the followed
 * part towards it, then a screen flash and a burst of sparks, an animated
 * grid sprite over the burst centre, and a fading ribbon traced through
 * five parts of the model. The setup builds the rings, the random sparks,
 * the grid and its quads and the ten textures, and prints the work size. */

static const VECTOR D_8013C8E4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model144State *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_GT4 strip;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    DVECTOR screen[128];
    u16 depths[128], interpolation[128], flags[128];
    SVECTOR vertices[4];
    PSXLONG flag, unused;
    GsOT *ot;
    s32 step;
    Model144Config *config;
    SVECTOR *point, *angle;
    SVECTOR *G32 *corner;
    s32 red, green, blue, flash_red, flash_green, flash_blue, trail_red, trail_green, trail_blue;
    s32 i, j, time, b, top, right, bottom, elevation;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C8E4;
    work = (Model144State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013CA20[command];
        point = work->ring;
        for (i = 0; i < 33; point++, i++) {
            point->vx = config->outer_radius * ccos(i * 128) / 4096;
            point->vy = config->outer_radius * csin(i * 128) / 4096;
            point->vz = direction * config->ring_depth;
        }
        for (i = 0; i < 33; point++, i++) {
            point->vx = config->inner_radius * ccos(i * 128) / 4096;
            point->vy = config->inner_radius * csin(i * 128) / 4096;
            point->vz = 0;
        }
        point = work->sparks;
        for (i = 0; i < 128; point++, i++) {
            time = (rand() % config->spark_radius) / 2 + config->spark_radius / 2;
            b = rand() % 4096;
            elevation = rand() % 2048;
            point->vx = time * ccos(b) / 4096 * csin(elevation) / 4096;
            point->vy = time * ccos(elevation) / 4096;
            point->vz = time * csin(b) / 4096 * csin(elevation) / 4096;
        }
        point = work->grid;
        for (i = -1; i < 2; i++) {
            for (j = -1; j < 2; point++, j++) {
                point->vx = j * config->grid_spacing;
                point->vy = i * config->grid_spacing;
                point->vz = 0;
            }
        }
        work->quads[0][0] = &work->grid[0];
        work->quads[0][1] = &work->grid[1];
        work->quads[0][2] = &work->grid[3];
        work->quads[0][3] = &work->grid[4];
        work->quads[1][0] = &work->grid[2];
        work->quads[1][1] = &work->grid[1];
        work->quads[1][2] = &work->grid[5];
        work->quads[1][3] = &work->grid[4];
        work->quads[2][0] = &work->grid[8];
        work->quads[2][1] = &work->grid[7];
        work->quads[2][2] = &work->grid[5];
        work->quads[2][3] = &work->grid[4];
        work->quads[3][0] = &work->grid[6];
        work->quads[3][1] = &work->grid[7];
        work->quads[3][2] = &work->grid[3];
        work->quads[3][3] = &work->grid[4];
        work->head = 0;
        work->remaining = 13;
        for (i = 0; i < 10; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C908[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf(D_8013C8F4, sizeof(Model144State), sizeof(Model144State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyGT4(&strip);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    point = work->nodes;
    angle = work->angles;
    for (i = 0; i < 5; point++, angle++, i++) {
        time = work->elapsed - config->ring_delay - i * 2;
        if (time < 0) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
            point->pad = 0;
            angle->vx = 0;
            angle->vy = 0;
            angle->vz = 0;
        } else if (time < config->ring_duration) {
            red = config->ring_red * (config->ring_duration - time) / config->ring_duration;
            green = config->ring_green * (config->ring_duration - time) / config->ring_duration;
            blue = config->ring_blue * (config->ring_duration - time) / config->ring_duration;
            setRGB0(&quad, red, green, blue);
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            if (point->pad == 0) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
                vertices[0].vx = matrix.t[0];
                vertices[0].vy = matrix.t[1];
                vertices[0].vz = matrix.t[2];
                angle->vx = ratan2(point->vy - vertices[0].vy, vertices[0].vz - point->vz);
                angle->vy = ratan2(vertices[0].vx - point->vx, vertices[0].vz - point->vz);
                angle->vz = 0;
                point->pad = 1;
            }
            rotation.vx = angle->vx;
            rotation.vy = angle->vy;
            rotation.vz = angle->vz;
            time = time * 4096 / config->ring_duration;
            setVector(&scale, time, time, time);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, interpolation, flags, 66);
            for (j = 0; j < 32; j++) {
                time = j + 33;
                setXY4(&quad, screen[j].vx, screen[j].vy, screen[j + 1].vx, screen[j + 1].vy,
                    screen[time].vx, screen[time].vy, screen[j + 34].vx, screen[j + 34].vy);
                setUV4(&quad, j * 8, 0, j * 8 + 8, 0, j * 8, 31, j * 8 + 8, 31);
                depth = AverageZ4(depths[j], depths[j + 1], depths[j + 33], depths[j + 34]);
                flag = (flags[j] | flags[j + 1] | flags[j + 33] | flags[j + 34]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    time = work->elapsed - config->burst_delay;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        flash_red = config->flash_red * (config->flash_duration - time) / config->flash_duration;
        flash_green = config->flash_green * (config->flash_duration - time) / config->flash_duration;
        flash_blue = config->flash_blue * (config->flash_duration - time) / config->flash_duration;
        setRGB0(&flash, flash_red, flash_green, flash_blue);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    time = work->elapsed - config->burst_delay;
    if (time < 0) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        work->center.vx = matrix.t[0];
        work->center.vy = matrix.t[1];
        work->center.vz = matrix.t[2];
        point->pad = 0;
    } else if (time < config->spark_duration) {
        box.r = config->spark_red * (config->spark_duration - time) / config->spark_duration;
        box.g = config->spark_green * (config->spark_duration - time) / config->spark_duration;
        box.b = config->spark_blue * (config->spark_duration - time) / config->spark_duration;
        position.vx = work->center.vx;
        position.vy = work->center.vy;
        position.vz = work->center.vz;
        setVector(&rotation, 0, 0, 0);
        time = time * 4096 / config->spark_duration;
        setVector(&scale, time, time, time);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->sparks, screen, depths, interpolation, flags, 128);
        for (i = 0; i < 128; i++) {
            box.x = screen[i].vx;
            box.y = screen[i].vy;
            depth = depths[i] >> 2;
            flag = flags[i] & 0x20;
            if (depth >= 0 && flag == 0 && rand() % 100 < 80) {
                GsSortBoxFill(&box, ot, depth);
            }
        }
    }
    time = work->elapsed - config->burst_delay;
    if ((u32)time < 24) {
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        setVector(&vertices[0], -config->grid_spacing / 2, -config->grid_spacing / 2, 0);
        setVector(&vertices[1], config->grid_spacing / 2, -config->grid_spacing / 2, 0);
        setVector(&vertices[2], -config->grid_spacing / 2, config->grid_spacing / 2, 0);
        setVector(&vertices[3], config->grid_spacing / 2, config->grid_spacing / 2, 0);
        position.vx = work->center.vx;
        position.vy = work->center.vy;
        position.vz = work->center.vz;
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setRGB0(&quad, config->grid_red, config->grid_green, config->grid_blue);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        if (time >= 3) {
            time /= 3;
            setUV4(&quad, time % 4 * 32, time / 4 * 32 + 32, time % 4 * 32 + 31, time / 4 * 32 + 32,
                time % 4 * 32, time / 4 * 32 + 63, time % 4 * 32 + 31, time / 4 * 32 + 63);
            corner = work->quads[0];
            for (j = 0; j < 4; corner += 4, j++) {
                depth = RotAverage4(corner[0], corner[1], corner[2], corner[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &unused, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        } else {
            setUV4(&quad, 0, 32, 31, 32, 0, 63, 31, 63);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &unused, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    time = work->elapsed - config->trail_delay;
    SetSemiTrans(&strip, 1);
    strip.tpage = work->texture[1] >> 16;
    strip.clut = work->texture[1];
    if (time < 0) {
        point = work->trail[0];
        for (i = 0; i < 14; i++) {
            point++;
            for (j = 0; j < 5; point++, j++) {
                GsSetLsMatrix(&base);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->trail_parts[j]), &matrix);
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
            }
            point[-6].vx = point[-5].vx;
            point[-6].vy = point[-5].vy;
            point[-6].vz = point[-5].vz;
            point[-6].vx += point[-5].vx - point[-4].vx;
            point[-6].vy += point[-5].vy - point[-4].vy;
            point[-6].vz += point[-5].vz - point[-4].vz;
            point->vx = point[-1].vx;
            point->vy = point[-1].vy;
            point->vz = point[-1].vz;
            point->vx += point[-1].vx - point[-2].vx;
            point->vy += point[-1].vy - point[-2].vy;
            point->vz += point[-1].vz - point[-2].vz;
            point++;
        }
    } else if (time < config->trail_duration) {
        point = &work->trail[work->head][1];
        for (i = 0; i < 5; point++, i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->trail_parts[i]), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
        }
        point = work->trail[work->head];
        point[0].vx = point[1].vx;
        point[0].vy = point[1].vy;
        point[0].vz = point[1].vz;
        point[0].vx += point[1].vx - point[2].vx;
        point[0].vy += point[1].vy - point[2].vy;
        point[0].vz += point[1].vz - point[2].vz;
        point = &work->trail[work->head][6];
        point->vx = point[-1].vx;
        point->vy = point[-1].vy;
        point->vz = point[-1].vz;
        point->vx += point[-1].vx - point[-2].vx;
        point->vy += point[-1].vy - point[-2].vy;
        point->vz += point[-1].vz - point[-2].vz;
        work->head = (work->head + 1) % 14;
        work->remaining--;
        GsSetLsMatrix(&base);
        RotTransPersN(work->trail[0], screen, depths, interpolation, flags, 98);
        for (i = 0; i < 13; i++) {
            for (j = 0; j < 6; j++) {
                time = (work->head + i) % 14 * 7 + j;
                b = (work->head + i + 1) % 14 * 7 + j;
                setXY4(&strip, screen[time].vx, screen[time].vy, screen[time + 1].vx, screen[time + 1].vy,
                    screen[b].vx, screen[b].vy, screen[b + 1].vx, screen[b + 1].vy);
                right = 127;
                bottom = 175;
                if (i == 0 || work->remaining - i >= 0) {
                    top = 96;
                } else {
                    top = 174;
                }
                setUV4(&strip, 0, top, right, top, 0, bottom, right, bottom);
                trail_red = config->trail_red;
                trail_green = config->trail_green;
                trail_blue = config->trail_blue;
                if (j == 0) {
                    setRGB0(&strip, 0, 0, 0);
                    setRGB1(&strip, trail_red, trail_green, trail_blue);
                    setRGB2(&strip, 0, 0, 0);
                    setRGB3(&strip, trail_red, trail_green, trail_blue);
                } else if (j == 5) {
                    setRGB0(&strip, trail_red, trail_green, trail_blue);
                    setRGB1(&strip, 0, 0, 0);
                    setRGB2(&strip, trail_red, trail_green, trail_blue);
                    setRGB3(&strip, 0, 0, 0);
                } else {
                    setRGB0(&strip, trail_red, trail_green, trail_blue);
                    setRGB1(&strip, trail_red, trail_green, trail_blue);
                    setRGB2(&strip, trail_red, trail_green, trail_blue);
                    setRGB3(&strip, trail_red, trail_green, trail_blue);
                }
                depth = AverageZ4(depths[time], depths[time + 1], depths[b], depths[b + 1]);
                flag = (flags[time] | flags[time + 1] | flags[b] | flags[b + 1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&strip, ot, (u16)depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    time = work->elapsed - config->burst_delay;
    if (time < 0) {
        return 0;
    }
    if (time < config->spark_duration) {
        if (work->completed == 0) {
            work->completed++;
            return 1;
        }
    } else {
        return 2;
    }
    return 0;
}
