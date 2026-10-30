#include "../../types.h"
#include "variant89_entry.h"

/* Header 89's entry: a flash fan bursts from a model part, fifteen panels
 * fly out to a 4x6 grid that fades in and rises in front of the slot, the
 * panels gather back to the part, and a two-sided shell sweeps the slot
 * while the screen fades from white. */

static const VECTOR D_8013D40C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_GT3 tri;
    POLY_F4 flash;
    POLY_GT4 fan;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR target;
    DVECTOR screen[24];
    u16 depths[24], projection[24], flags[24];

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D40C;
    {
        u8 order[15] = {0, 1, 2, 3, 11, 4, 5, 6, 7, 8, 11, 9, 10, 11, 11};
        PSXLONG flag, interpolation;
        Model89State *work;
        s32 direction;
        Model89Config *config;
        GsOT *ot;
        s32 j;
        SVECTOR *point, *p;
        u8 *cell;
        DVECTOR *inner_xy, *outer_xy;
        u16 *inner_depth, *outer_depth, *inner_flag, *outer_flag;
        s32 i, time, value;
        s32 shade;
        u8 red, green, blue;
        s32 level;
        PSXLONG depth;

        work = (Model89State *)context;
        direction = Model_GetActiveSlotIndex() * 2 - 1;
        if (command >= 0) {
            config = work->config = &D_8013D464[command];
            point = work->ring;
            setVector(point, 0, 0, 0);
            point++;
            for (i = 0; i < 17; point++, i++) {
                point->vx = config->ring_radius * ccos(i * 256) / 4096;
                point->vy = config->ring_radius * csin(i * 256) / 4096;
                point->vz = 0;
            }
            point = work->grid;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 6; point++, j++) {
                    point->vx = 0;
                    point->vy = config->panel_size * (i - 1) - 350;
                    point->vz = direction * config->panel_size * (j - 5);
                }
            }
            setVector(&work->center, 0, 0, 0);
            point = work->arc;
            for (i = 0; i < 9; point++, i++) {
                point->vx = config->arc_radius * csin(i * 128 + 512) / 4096 - config->arc_radius / 2;
                point->vy = config->arc_radius * ccos(i * 128 + 512) / 4096;
                point->vz = 0;
            }
            for (i = 0; i < 9; point++, i++) {
                point->vx = config->arc_outer * csin(i * 128 + 512) / 4096 - config->arc_radius / 2;
                point->vy = config->arc_outer * ccos(i * 128 + 512) / 4096;
                point->vz = -direction * config->arc_radius / 3;
            }
            for (i = 0; i < 2; i++) {
                work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013D42C[i]);
            }
            work->elapsed = -config->delay;
            work->completed = 0;
            return 0;
        }
        config = work->config;
        ot = func_80058F10();
        if (work->elapsed < 0) {
            work->elapsed += Model_GetFrameStep();
            return 0;
        }
        PushMatrix();
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        SetPolyFT4(&quad);
        SetPolyGT3(&tri);
        SetPolyF4(&flash);
        SetPolyGT4(&fan);
        tri.tpage = work->texture[1] >> 16;
        tri.clut = work->texture[1];
        SetSemiTrans(&fan, 1);
        setUV3(&tri, 16, 255, 0, 128, 31, 128);
        if (work->elapsed < config->flash_duration) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
            value = work->elapsed * 4096 / config->flash_duration;
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 18);
            value = (config->flash_duration - work->elapsed) * 4096 / config->flash_duration;
            setRGB0(&tri, 0, 0, 0);
            setRGB1(&tri, config->flash_red * value / 4096, config->flash_green * value / 4096, config->flash_blue * value / 4096);
            setRGB2(&tri, tri.r1, tri.g1, tri.b1);
            for (i = 1; i < 17; i++) {
                setXY3(&tri, screen[0].vx, screen[0].vy, screen[i].vx, screen[i].vy,
                    screen[i + 1].vx, screen[i + 1].vy);
                depth = AverageZ3(depths[0], depths[i], depths[i + 1]);
                flag = (flags[0] | flags[i] | flags[i + 1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&tri, ot, (u16)depth, 1);
                }
            }
            point = work->positions;
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            for (i = 0; i < 5; i++) {
                for (j = 0; j < 3; point++, j++) {
                    setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
                }
            }
        }
        time = work->elapsed - config->flash_duration;
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        if (time > 0 && time <= config->travel + config->stagger * 15) {
            p = &work->grid[16];
            point = work->positions;
            cell = &order[14];
            setRGB0(&quad, config->panel_red, config->panel_green, config->panel_blue);
            for (j = 0; j < 3; p--, j++) {
                for (i = 0; i < 5; point++, p--, cell--, i++) {
                    if (time < (j + i * 3) * config->stagger) {
                        continue;
                    }
                    GsSetLsMatrix(&base);
                    if (time <= (j + i * 3) * config->stagger + config->travel) {
                        setVector(&vertices[0], 0, -config->panel_size / 2, -config->panel_size / 2);
                        setVector(&vertices[3], 0, config->panel_size / 2, config->panel_size / 2);
                        setVector(&vertices[1], 0, vertices[0].vy, vertices[3].vz);
                        setVector(&vertices[2], 0, vertices[3].vy, vertices[0].vz);
                        setVector(&position, point->vx, point->vy, point->vz);
                        position.vx += work->center.vx;
                        position.vy += work->center.vy;
                        position.vz += work->center.vz;
                        value = (time + i) * 128;
                        setVector(&rotation, (time + i) * 256, 0, value);
                        RotTrans(&position, (VECTOR *)matrix.t, &flag);
                        RotMatrix(&rotation, &matrix);
                        MulMatrix2(&base, &matrix);
                        ScaleMatrix(&matrix, &scale);
                        GsSetLsMatrix(&matrix);
                    } else {
                        setVector(&vertices[0], p[0].vx, p[0].vy, p[0].vz);
                        setVector(&vertices[1], p[1].vx, p[1].vy, p[1].vz);
                        setVector(&vertices[2], p[6].vx, p[6].vy, p[6].vz);
                        setVector(&vertices[3], p[7].vx, p[7].vy, p[7].vz);
                    }
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                    if (*cell < 4) {
                        value = 0;
                    } else if (*cell < 8) {
                        value = 32;
                    } else {
                        value = 64;
                    }
                    quad.u0 = (*cell % 4) * 32;
                    quad.v0 = value;
                    quad.u1 = (*cell % 4) * 32 + 31;
                    quad.v1 = value;
                    quad.u2 = (*cell % 4) * 32;
                    quad.v2 = value + 31;
                    quad.u3 = (*cell % 4) * 32 + 31;
                    quad.v3 = value + 31;
                    SetSemiTrans(&quad, 0);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    if (time < (j + i * 3) * config->stagger + config->travel) {
                        value = config->travel - (time - (j + i * 3) * config->stagger);
                        setVector(&vertices[0], p[0].vx, p[0].vy, p[0].vz);
                        vertices[0].vx += p[1].vx;
                        vertices[0].vy += p[1].vy;
                        vertices[0].vz += p[1].vz;
                        vertices[0].vx += p[6].vx;
                        vertices[0].vy += p[6].vy;
                        vertices[0].vz += p[6].vz;
                        vertices[0].vx += p[7].vx;
                        vertices[0].vy += p[7].vy;
                        vertices[0].vz += p[7].vz;
                        vertices[0].vx /= 4;
                        vertices[0].vy /= 4;
                        vertices[0].vz /= 4;
                        point->vx += (vertices[0].vx - point->vx) / value;
                        point->vy += (vertices[0].vy - point->vy) / value;
                        point->vz += (vertices[0].vz - point->vz) / value;
                    }
                }
            }
        }
        time -= (config->travel + config->stagger * 15);
        if (time >= 0 && time < config->fade_in * 2 + config->rise + config->lift) {
            setVector(&position, work->center.vx, work->center.vy, work->center.vz);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->grid, screen, depths, projection, flags, 24);
            cell = order;
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 5; cell++, j++) {
                    /* Row indices kept in the multi-block value/shade
                     * variables so local-alloc cannot tie the i * 6 temp
                     * into them. */
                    value = i * 6 + j;
                    shade = i * 6 + j + 1;
                    setXY4(&flash,
                        screen[value].vx, screen[value].vy,
                        screen[shade].vx, screen[shade].vy,
                        screen[(i + 1) * 6 + j].vx, screen[(i + 1) * 6 + j].vy,
                        screen[(i + 1) * 6 + j + 1].vx, screen[(i + 1) * 6 + j + 1].vy);
                    setXY4(&quad,
                        screen[value].vx, screen[value].vy,
                        screen[shade].vx, screen[shade].vy,
                        screen[(i + 1) * 6 + j].vx, screen[(i + 1) * 6 + j].vy,
                        screen[(i + 1) * 6 + j + 1].vx, screen[(i + 1) * 6 + j + 1].vy);
                    depth = AverageZ4(depths[value], depths[shade],
                        depths[(i + 1) * 6 + j], depths[(i + 1) * 6 + j + 1]);
                    flag = (flags[value] | flags[shade]
                        | flags[(i + 1) * 6 + j] | flags[(i + 1) * 6 + j + 1]) & 0x20;
                    SetSemiTrans(&quad, 1);
                    if (time < config->fade_in) {
                        value = (config->fade_in - time) * 255 / config->fade_in;
                        setRGB0(&flash, value, value, value);
                        if (depth >= 0 && flag == 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)depth, 1);
                        }
                    }
                    if (time >= config->fade_in + config->rise + config->lift) {
                        value = (config->fade_in * 2 + config->rise + config->lift - time) * 255
                            / config->fade_in;
                        setRGB0(&flash, value, value, value);
                        if (depth >= 0 && flag == 0) {
                            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)depth, 1);
                        }
                        SetSemiTrans(&quad, 0);
                    }
                    if (*cell < 4) {
                        value = 0;
                    } else if (*cell < 8) {
                        value = 32;
                    } else {
                        value = 64;
                    }
                    quad.u0 = (*cell % 4) * 32;
                    quad.v0 = value;
                    quad.u1 = (*cell % 4) * 32 + 31;
                    quad.v1 = value;
                    quad.u2 = (*cell % 4) * 32;
                    quad.v2 = value + 31;
                    quad.u3 = (*cell % 4) * 32 + 31;
                    quad.v3 = value + 31;
                    if (depth >= 0 && flag == 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                }
            }
            if (time >= config->fade_in && time < config->fade_in + config->rise) {
                work->center.vz += (direction * 1350 - work->center.vz)
                    / (config->rise - time + config->fade_in);
            } else if (time >= config->fade_in + config->rise
                && time < config->fade_in + config->rise + config->lift) {
                work->center.vz += (direction * (config->panel_size * 5 + 1350) - work->center.vz)
                    / (config->lift - time + config->fade_in + config->rise);
            }
        }
        time -= (config->fade_in * 2 + config->rise + config->lift);
        if (time >= 0 && time < config->gather) {
            setVector(&vertices[0], 0, -config->panel_size / 2, -config->panel_size / 2);
            setVector(&vertices[3], 0, config->panel_size / 2, config->panel_size / 2);
            setVector(&vertices[1], 0, vertices[0].vy, vertices[3].vz);
            setVector(&vertices[2], 0, vertices[3].vy, vertices[0].vz);
            cell = order;
            point = work->positions;
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(&target, matrix.t[0], matrix.t[1], matrix.t[2]);
            target.vx -= work->center.vx;
            target.vy -= work->center.vy;
            target.vz -= work->center.vz + direction * 450;
            for (j = 0; j < 3; j++) {
                for (i = 0; i < 5; cell++, point++, i++) {
                    GsSetLsMatrix(&base);
                    value = (time + i) * 128;
                    setVector(&rotation, (time + i) * 256, 1024, value);
                    setVector(&position, point->vx, point->vy, point->vz);
                    position.vx += work->center.vx;
                    position.vy += work->center.vy;
                    position.vz += work->center.vz;
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    MulMatrix2(&base, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                    if (*cell < 4) {
                        value = 0;
                    } else if (*cell < 8) {
                        value = 32;
                    } else {
                        value = 64;
                    }
                    quad.u0 = (*cell % 4) * 32;
                    quad.v0 = value;
                    quad.u1 = (*cell % 4) * 32 + 31;
                    quad.v1 = value;
                    quad.u2 = (*cell % 4) * 32;
                    quad.v2 = value + 31;
                    quad.u3 = (*cell % 4) * 32 + 31;
                    quad.v3 = value + 31;
                    SetSemiTrans(&quad, 0);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    point->vx += (target.vx - point->vx) / (config->gather - time);
                    point->vy += (target.vy - point->vy) / (config->gather - time);
                    point->vz += (target.vz - point->vz) / (config->gather - time);
                }
            }
        }
        time = work->elapsed - config->flash_duration;
        time -= config->travel + config->stagger * 15;
        time -= config->fade_in + config->rise;
        if (time >= 0 && time < config->shell_duration) {
            fan.tpage = work->texture[1] >> 16;
            fan.clut = work->texture[1];
            SetSemiTrans(&fan, 1);
            setUV4(&fan, 0, 128, 31, 128, 0, 255, 31, 255);
            setVector(&position, 0, -350, direction * 450);
            value = time * 4096 / config->shell_duration;
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->arc, screen, depths, projection, flags, 18);
            inner_xy = screen;
            outer_xy = &screen[9];
            inner_depth = depths;
            outer_depth = &depths[9];
            inner_flag = flags;
            outer_flag = &flags[9];
            for (i = 0; i < 8; inner_xy++, outer_xy++, inner_depth++, outer_depth++, inner_flag++, outer_flag++, i++) {
                setXY4(&fan, inner_xy[0].vx, inner_xy[0].vy, inner_xy[1].vx, inner_xy[1].vy,
                    outer_xy[0].vx, outer_xy[0].vy, outer_xy[1].vx, outer_xy[1].vy);
                value = csin(i * 224 + 128);
                shade = csin((i + 1) * 224 + 128);
                red = config->shell_red * value / 4096 * (config->shell_duration - time) / config->shell_duration;
                green = config->shell_green * value / 4096 * (config->shell_duration - time) / config->shell_duration;
                blue = config->shell_blue * value / 4096 * (config->shell_duration - time) / config->shell_duration;
                setRGB0(&fan, red, green, blue);
                setRGB2(&fan, red, green, blue);
                red = config->shell_red * shade / 4096 * (config->shell_duration - time) / config->shell_duration;
                green = config->shell_green * shade / 4096 * (config->shell_duration - time) / config->shell_duration;
                blue = config->shell_blue * shade / 4096 * (config->shell_duration - time) / config->shell_duration;
                setRGB1(&fan, red, green, blue);
                setRGB3(&fan, red, green, blue);
                depth = AverageZ4(inner_depth[0], inner_depth[1], outer_depth[0], outer_depth[1]);
                flag = (inner_flag[0] | inner_flag[1] | outer_flag[0] | outer_flag[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&fan, ot, (u16)depth);
                }
            }
            setVector(&rotation, 0, 0, 2048);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->arc, screen, depths, projection, flags, 18);
            inner_xy = screen;
            outer_xy = &screen[9];
            inner_depth = depths;
            outer_depth = &depths[9];
            inner_flag = flags;
            outer_flag = &flags[9];
            for (i = 0; i < 8; inner_xy++, outer_xy++, inner_depth++, outer_depth++, inner_flag++, outer_flag++, i++) {
                setXY4(&fan, inner_xy[0].vx, inner_xy[0].vy, inner_xy[1].vx, inner_xy[1].vy,
                    outer_xy[0].vx, outer_xy[0].vy, outer_xy[1].vx, outer_xy[1].vy);
                value = csin(i * 224 + 128);
                shade = csin((i + 1) * 224 + 128);
                red = config->shell_red * value / 4096 * (config->shell_duration - time) / config->shell_duration;
                green = config->shell_green * value / 4096 * (config->shell_duration - time) / config->shell_duration;
                blue = config->shell_blue * value / 4096 * (config->shell_duration - time) / config->shell_duration;
                setRGB0(&fan, red, green, blue);
                setRGB2(&fan, red, green, blue);
                red = config->shell_red * shade / 4096 * (config->shell_duration - time) / config->shell_duration;
                green = config->shell_green * shade / 4096 * (config->shell_duration - time) / config->shell_duration;
                blue = config->shell_blue * shade / 4096 * (config->shell_duration - time) / config->shell_duration;
                setRGB1(&fan, red, green, blue);
                setRGB3(&fan, red, green, blue);
                depth = AverageZ4(inner_depth[0], inner_depth[1], outer_depth[0], outer_depth[1]);
                flag = (inner_flag[0] | inner_flag[1] | outer_flag[0] | outer_flag[1]) & 0x20;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&fan, ot, (u16)depth);
                }
            }
        }
        time = work->elapsed - config->flash_duration;
        time -= config->travel + config->stagger * 15;
        time -= config->fade_in + config->rise;
        if (time >= 0 && time < config->shell_duration) {
            setXY4(&flash, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT,
                GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT);
            level = (config->shell_duration - time) * 255 / config->shell_duration;
            setRGB0(&flash, level, level, level);
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
        }
        time = work->elapsed - config->flash_duration;
        time -= config->travel + config->stagger * 15;
        time -= config->fade_in * 2 + config->rise + config->lift;
        time -= config->gather;
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        if (time < -(config->gather + config->fade_in + config->lift)) {
            return 0;
        }
        if (time >= 0) {
            return 2;
        }
        if (time < -config->gather) {
            if (work->completed == 0) {
                work->completed++;
                return 1;
            }
        }
        return 0;
    }
}
