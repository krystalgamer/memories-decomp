#include "../../types.h"
#include "variant163_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 163's entry: after the start delay a disc grows on one of the
 * slot's parts, eight spinning tiles fly from it into a 4x2 grid near the
 * other slot, the grid flashes and is pushed towards the other slot, the
 * tiles fly back and a glowing double ring opens behind a fading screen
 * flash. A negative command draws a frame; any other command sets the work
 * area up. */

static const VECTOR D_8013D3FC = {4096, 4096, 4096, 0};

static const Model163Order D_8013D40C = {{0, 1, 2, 3, 4, 5, 6, 7}};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_G3 fan;
    POLY_F4 flash;
    POLY_GT4 band;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR target;
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    Model163Order order;
    PSXLONG flag, interpolation;
    Model163State *work;
    s32 direction;
    GsOT *ot;
    s32 i, j;
    Model163Config *config;
    SVECTOR *point, *cell;
    s32 t, s, b, c, d, depth;
    s32 start, fly;
    u8 *o;
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa, *fb;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D3FC;
    order = D_8013D40C;
    work = (Model163State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013D44C[command];
        point = work->disc;
        point->vx = 0;
        point->vy = 0;
        point->vz = 0;
        point++;
        for (i = 0; i < 33; i++) {
            point->vx = config->radius * ccos(i << 7) / 4096;
            point->vy = config->radius * csin(i << 7) / 4096;
            point->vz = 0;
            point++;
        }
        point = work->grid;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 5; j++) {
                point->vx = 0;
                point->vy = config->size * (i - 1) - 350;
                point->vz = direction * config->size * (j - 4);
                point++;
            }
        }
        point = work->rings;
        for (i = 0; i < 9; i++) {
            point->vx = config->inner * csin((i << 7) + 0x200) / 4096 - config->inner / 2;
            point->vy = config->inner * ccos((i << 7) + 0x200) / 4096;
            point->vz = 0;
            point++;
        }
        for (i = 0; i < 9; i++) {
            point->vx = config->outer * csin((i << 7) + 0x200) / 4096 - config->inner / 2;
            point->vy = config->outer * ccos((i << 7) + 0x200) / 4096;
            point->vz = -direction * config->inner / 3;
            point++;
        }
        work->offset.vx = 0;
        work->offset.vy = 0;
        work->offset.vz = 0;
        work->completed = 0;
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D414[i]);
        }
        work->elapsed = -config->delay;
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
    SetPolyG3(&fan);
    SetPolyF4(&flash);
    SetPolyGT4(&band);
    if (work->elapsed < config->grow) {
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        s = (work->elapsed << 12) / config->grow;
        setVector(&scale, s, s, s);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->disc, screen, depths, projection, flags, 34);
        s = ((config->grow - work->elapsed) << 12) / config->grow;
        setRGB0(&fan, 0, 0, 0);
        fan.r1 = config->red * s / 4096;
        fan.g1 = config->green * s / 4096;
        fan.b1 = config->blue * s / 4096;
        fan.r2 = fan.r1;
        fan.g2 = fan.g1;
        fan.b2 = fan.b1;
        for (i = 1; i < 33; i++) {
            fan.x0 = screen[0].vx;
            fan.y0 = screen[0].vy;
            fan.x1 = screen[i].vx;
            fan.y1 = screen[i].vy;
            fan.x2 = screen[i + 1].vx;
            fan.y2 = screen[i + 1].vy;
            depth = AverageZ3(depths[0], depths[i], depths[i + 1]);
            flag = (flags[0] | flags[i] | flags[i + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
        point = work->positions;
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 2; j++) {
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
                point++;
            }
        }
    }
    t = work->elapsed - config->grow;
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    if (t > 0 && t <= config->fly + config->spacing * 8) {
        point = work->positions;
        cell = &work->grid[8];
        o = &order.cell[7];
        quad.r0 = config->tile_red;
        quad.g0 = config->tile_green;
        quad.b0 = config->tile_blue;
        for (j = 0; j < 2; j++) {
            for (i = 0; i < 4; i++) {
                if (t >= (i * 2 + j) * config->spacing) {
                    GsSetLsMatrix(&base);
                    if (t <= (i * 2 + j) * config->spacing + config->fly) {
                        setVector(&vertices[0], 0, -config->size / 2, -config->size / 2);
                        setVector(&vertices[1], 0, config->size / 2, -config->size / 2);
                        setVector(&vertices[2], 0, -config->size / 2, config->size / 2);
                        setVector(&vertices[3], 0, config->size / 2, config->size / 2);
                        position.vx = point->vx;
                        position.vy = point->vy;
                        position.vz = point->vz;
                        position.vx += work->offset.vx;
                        position.vy += work->offset.vy;
                        position.vz += work->offset.vz;
                        s = (t + i) << 7;
                        setVector(&rotation, (t + i) << 8, 0, s);
                        RotTrans(&position, (VECTOR *)matrix.t, &flag);
                        RotMatrix(&rotation, &matrix);
                        MulMatrix2(&base, &matrix);
                        ScaleMatrix(&matrix, &scale);
                        GsSetLsMatrix(&matrix);
                    } else {
                        setVector(&vertices[0], cell[0].vx, cell[0].vy, cell[0].vz);
                        setVector(&vertices[1], cell[1].vx, cell[1].vy, cell[1].vz);
                        setVector(&vertices[2], cell[5].vx, cell[5].vy, cell[5].vz);
                        setVector(&vertices[3], cell[6].vx, cell[6].vy, cell[6].vz);
                    }
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    setUV4(&quad, (*o & 3) << 5, (*o >> 2) << 5, ((*o & 3) << 5) + 31, (*o >> 2) << 5,
                        (*o & 3) << 5, ((*o >> 2) << 5) + 31, ((*o & 3) << 5) + 31, ((*o >> 2) << 5) + 31);
                    SetSemiTrans(&quad, 0);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, (u16)depth);
                    }
                    start = (i * 2 + j) * config->spacing;
                    fly = config->fly;
                    if (t < start + fly) {
                        s = fly - (t - start);
                        vertices[0].vx = cell[0].vx;
                        vertices[0].vy = cell[0].vy;
                        vertices[0].vz = cell[0].vz;
                        vertices[0].vx += cell[1].vx;
                        vertices[0].vy += cell[1].vy;
                        vertices[0].vz += cell[1].vz;
                        vertices[0].vx += cell[5].vx;
                        vertices[0].vy += cell[5].vy;
                        vertices[0].vz += cell[5].vz;
                        vertices[0].vx += cell[6].vx;
                        vertices[0].vy += cell[6].vy;
                        vertices[0].vz += cell[6].vz;
                        vertices[0].vx /= 4;
                        vertices[0].vy /= 4;
                        vertices[0].vz /= 4;
                        if (s > 0) {
                            point->vx += (vertices[0].vx - point->vx) / s;
                            point->vy += (vertices[0].vy - point->vy) / s;
                            point->vz += (vertices[0].vz - point->vz) / s;
                        }
                    }
                }
                point++;
                cell--;
                o--;
            }
            cell--;
        }
    }
    t -= config->fly + config->spacing * 8;
    if (t >= 0 && t < config->flash * 2 + config->push + config->hold) {
        position.vx = work->offset.vx;
        position.vy = work->offset.vy;
        position.vz = work->offset.vz;
        GsSetLsMatrix(&base);
        o = order.cell;
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->grid, screen, depths, projection, flags, 15);
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 4; j++) {
                s = i * 5 + j;
                b = s + 1;
                c = (i + 1) * 5 + j;
                d = c + 1;
                setXY4(&flash, screen[s].vx, screen[s].vy, screen[b].vx, screen[b].vy,
                    screen[c].vx, screen[c].vy, screen[d].vx, screen[d].vy);
                setXY4(&quad, screen[s].vx, screen[s].vy, screen[b].vx, screen[b].vy,
                    screen[c].vx, screen[c].vy, screen[d].vx, screen[d].vy);
                depth = AverageZ4(depths[s], depths[b], depths[c], depths[d]);
                flag = (flags[s] | flags[b] | flags[c] | flags[d]) & 0x20;
                SetSemiTrans(&quad, 1);
                if (t < config->flash) {
                    s = (config->flash - t) * 255 / config->flash;
                    setRGB0(&flash, s, s, s);
                    if (depth >= 0 && flag == 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)depth, 1);
                    }
                }
                if (t >= config->flash + config->push + config->hold) {
                    s = (config->flash * 2 + config->push + config->hold - t) * 255 / config->flash;
                    setRGB0(&flash, s, s, s);
                    if (depth >= 0 && flag == 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, (u16)depth, 1);
                    }
                    SetSemiTrans(&quad, 0);
                }
                if (*o < 4) {
                    s = 0;
                } else if (*o < 8) {
                    s = 32;
                } else {
                    s = 64;
                }
                setUV4(&quad, (*o & 3) << 5, s, ((*o & 3) << 5) + 31, s,
                    (*o & 3) << 5, s + 31, ((*o & 3) << 5) + 31, s + 31);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                o++;
            }
        }
        if (t >= config->flash && t < config->flash + config->push) {
            work->offset.vz += (direction * 1350 - work->offset.vz) / (config->push - t + config->flash);
        } else if (t >= config->flash + config->push && t < config->flash + config->push + config->hold) {
            work->offset.vz += (direction * (config->size * 4 + 1350) - work->offset.vz) /
                (config->hold - t + config->flash + config->push);
        }
    }
    t -= config->flash * 2 + config->push + config->hold;
    if (t >= 0 && t < config->fly) {
        o = order.cell;
        setVector(&vertices[0], 0, -config->size / 2, -config->size / 2);
        setVector(&vertices[3], 0, config->size / 2, config->size / 2);
        setVector(&vertices[1], 0, vertices[0].vy, vertices[3].vz);
        setVector(&vertices[2], 0, vertices[3].vy, vertices[0].vz);
        point = work->positions;
        GsSetLsMatrix(&base);
        GsGetLwUnit((GsCOORDUNIT *)Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        target.vx = matrix.t[0];
        target.vy = matrix.t[1];
        target.vz = matrix.t[2];
        target.vx -= work->offset.vx;
        target.vy -= work->offset.vy;
        target.vz -= work->offset.vz + direction * 450;
        for (j = 0; j < 2; j++) {
            for (i = 0; i < 4; i++) {
                GsSetLsMatrix(&base);
                s = (t + i) << 7;
                setVector(&rotation, (t + i) << 8, 0x400, s);
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = point->vz;
                position.vx += work->offset.vx;
                position.vy += work->offset.vy;
                position.vz += work->offset.vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (*o < 4) {
                    s = 0;
                } else if (*o < 8) {
                    s = 32;
                } else {
                    s = 64;
                }
                setUV4(&quad, (*o & 3) << 5, s, ((*o & 3) << 5) + 31, s,
                    (*o & 3) << 5, s + 31, ((*o & 3) << 5) + 31, s + 31);
                SetSemiTrans(&quad, 0);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                point->vx += (target.vx - point->vx) / (config->fly - t);
                point->vy += (target.vy - point->vy) / (config->fly - t);
                point->vz += (target.vz - point->vz) / (config->fly - t);
                o++;
                point++;
            }
        }
    }
    t = work->elapsed - config->grow;
    t -= config->fly + config->spacing * 8;
    t -= config->flash + config->push;
    if (t >= 0 && t < config->ring) {
        band.tpage = work->texture[1] >> 16;
        band.clut = work->texture[1];
        SetSemiTrans(&band, 1);
        setUV4(&band, 0, 128, 31, 128, 0, 255, 31, 255);
        setVector(&position, 0, -350, direction * 450);
        s = (t << 12) / config->ring;
        sb = &screen[9];
        db = &depths[9];
        fb = &flags[9];
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, s, s, s);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        sa = screen;
        da = depths;
        fa = flags;
        RotTransPersN(work->rings, sa, da, projection, fa, 18);
        for (i = 0; i < 8; i++) {
            setXY4(&band, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
            s = csin(i * 224 + 128);
            b = csin(i * 224 + 352);
            red = config->ring_red * s / 4096 * (config->ring - t) / config->ring;
            green = config->ring_green * s / 4096 * (config->ring - t) / config->ring;
            blue = config->ring_blue * s / 4096 * (config->ring - t) / config->ring;
            setRGB0(&band, red, green, blue);
            setRGB2(&band, red, green, blue);
            red = config->ring_red * b / 4096 * (config->ring - t) / config->ring;
            green = config->ring_green * b / 4096 * (config->ring - t) / config->ring;
            blue = config->ring_blue * b / 4096 * (config->ring - t) / config->ring;
            setRGB1(&band, red, green, blue);
            setRGB3(&band, red, green, blue);
            depth = AverageZ4(da[0], da[1], db[0], db[1]);
            flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&band, ot, (u16)depth);
            }
            sa++;
            sb++;
            da++;
            db++;
            fa++;
            fb++;
        }
        setVector(&rotation, 0, 0, 0x800);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->rings, screen, depths, projection, flags, 18);
        sa = screen;
        sb = &screen[9];
        da = depths;
        db = &depths[9];
        fa = flags;
        fb = &flags[9];
        for (i = 0; i < 8; i++) {
            setXY4(&band, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
            s = csin(i * 224 + 128);
            b = csin(i * 224 + 352);
            red = config->ring_red * s / 4096 * (config->ring - t) / config->ring;
            green = config->ring_green * s / 4096 * (config->ring - t) / config->ring;
            blue = config->ring_blue * s / 4096 * (config->ring - t) / config->ring;
            setRGB0(&band, red, green, blue);
            setRGB2(&band, red, green, blue);
            red = config->ring_red * b / 4096 * (config->ring - t) / config->ring;
            green = config->ring_green * b / 4096 * (config->ring - t) / config->ring;
            blue = config->ring_blue * b / 4096 * (config->ring - t) / config->ring;
            setRGB1(&band, red, green, blue);
            setRGB3(&band, red, green, blue);
            depth = AverageZ4(da[0], da[1], db[0], db[1]);
            flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                GsSortPoly(&band, ot, (u16)depth);
            }
            sa++;
            sb++;
            da++;
            db++;
            fa++;
            fb++;
        }
    }
    t = work->elapsed - config->grow;
    t -= config->fly + config->spacing * 8;
    t -= config->flash + config->push;
    if (t >= 0 && t < config->ring) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        red = (config->ring - t) * 255 / config->ring;
        setRGB0(&flash, red, red, red);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    t = work->elapsed - config->grow;
    t -= config->fly + config->spacing * 8;
    t -= config->flash * 2 + config->push + config->hold;
    t -= config->fly;
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (t < -(config->fly + config->flash + config->hold)) {
        return 0;
    }
    if (t >= 0) {
        return 2;
    }
    if (t < -config->fly) {
        if (work->completed == 0) {
            work->completed++;
            return 1;
        }
    }
    return 0;
}
