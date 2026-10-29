#include "../../types.h"
#include "variant173us_entry.h"

/* The entry scatters a row of textured cubes from the active slot. Each cube
 * is drawn as up to four spinning copies spread along the x axis while it
 * flies towards a point in front of the other slot, after which every copy
 * left in its mask bursts into a fading ring of light. A negative command
 * draws a frame; any other command sets the work area up. */

static const VECTOR D_8013C2D0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_GT4 glow;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    DVECTOR screen[34];
    u16 depths[34], interpolation[34], flags[34];
    PSXLONG flag, parameter, depth;
    Model173UsState *work;
    Model173UsConfig *config;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, k, time, value;
    u8 red, green, blue;
    SVECTOR *point, *inner;
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa, *fb;
    SVECTOR *G32 *face;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C2D0;
    work = (Model173UsState *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C37C[command % 100];
        point = work->corners;
        for (i = -1; i < 2; i += 2) {
            for (j = -1; j < 2; j += 2) {
                for (k = -1; k < 2; point++, k += 2) {
                    point->vx = j * config->size;
                    point->vy = i * config->size;
                    point->vz = k * config->size;
                }
            }
        }
        point = work->corners;
        face = work->faces;
        face[0] = &point[0];
        face[1] = &point[1];
        face[2] = &point[2];
        face[3] = &point[3];
        face[4] = &point[1];
        face[5] = &point[5];
        face[6] = &point[3];
        face[7] = &point[7];
        face[8] = &point[5];
        face[9] = &point[4];
        face[10] = &point[7];
        face[11] = &point[6];
        face[12] = &point[4];
        face[13] = &point[0];
        face[14] = &point[6];
        face[15] = &point[2];
        face[16] = &point[1];
        face[17] = &point[0];
        face[18] = &point[5];
        face[19] = &point[4];
        face[20] = &point[2];
        face[21] = &point[3];
        face[22] = &point[6];
        face[23] = &point[7];
        for (i = 0; i < config->count; i++) {
            work->masks[i] = ((1 << rand() % 6) & 0xF) + ((1 << rand() % 4) & 0xF);
        }
        point = work->ring;
        inner = &work->ring[17];
        for (i = 0; i < 17; i++) {
            point->vx = config->outer * ccos(i << 8) / 4096;
            point->vy = config->outer * csin(i << 8) / 4096;
            point->vz = -direction * (config->outer - config->inner) / 3;
            inner->vx = config->inner * ccos(i << 8) / 4096;
            inner->vy = config->inner * csin(i << 8) / 4096;
            inner->vz = 0;
            point++;
            inner++;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C2F0[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model173UsState), sizeof(Model173UsState));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyGT4(&glow);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    setVector(&scale, 4096, 4096, 4096);
    point = work->cubes;
    for (i = 0; i < config->count; point++, i++) {
        time = work->elapsed - config->delay - i * config->stagger;
        if (time < 0) {
            Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)point);
            point->vy = 0;
        } else if (time < config->flight) {
            for (j = 0; j < 4; j++) {
                copyVector(&position, point);
                switch (j) {
                case 0:
                    position.vx += direction * (config->size * 3);
                    setVector(&rotation, -time << 6, direction * 1024, 0);
                    break;
                case 1:
                    position.vx += direction * config->size;
                    setVector(&rotation, -time << 6, direction * 2048, 0);
                    break;
                case 2:
                    position.vx -= direction * config->size;
                    setVector(&rotation, -time << 6, 0, 0);
                    break;
                case 3:
                    position.vx -= direction * (config->size * 3);
                    setVector(&rotation, -time << 6, direction * 3072, 0);
                    break;
                }
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrixZYX(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                face = work->faces;
                if ((work->masks[i] >> j) & 1) {
                    quad.tpage = work->texture[2] >> 16;
                    quad.clut = work->texture[2];
                    SetSemiTrans(&quad, 0);
                    setUV4(&quad, 32, 0, 63, 0, 32, 31, 63, 31);
                    red = config->red;
                    green = config->green;
                    blue = config->blue;
                    setRGB0(&quad, red, green, blue);
                    for (k = 0; k < 4; face += 4, k++) {
                        value = RotAverageNclip4(face[0], face[1], face[2], face[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &depth, &flag);
                        if (depth >= 0 && flag >= 0 && value > 0) {
                            GsSortPoly(&quad, ot, (u16)depth);
                        }
                    }
                    quad.tpage = work->texture[4] >> 16;
                    quad.clut = work->texture[4];
                    setUV4(&quad, 96, 0, 127, 0, 96, 31, 127, 31);
                    for (k = 4; k < 6; face += 4, k++) {
                        value = RotAverageNclip4(face[0], face[1], face[2], face[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &depth, &flag);
                        if (depth >= 0 && flag >= 0 && value > 0) {
                            GsSortPoly(&quad, ot, (u16)depth);
                        }
                    }
                } else {
                    quad.tpage = work->texture[4] >> 16;
                    quad.clut = work->texture[4];
                    SetSemiTrans(&quad, 1);
                    setUV4(&quad, 96, 0, 127, 0, 96, 31, 127, 31);
                    red = config->red * (config->flight - time) / config->flight;
                    green = config->green * (config->flight - time) / config->flight;
                    blue = config->blue * (config->flight - time) / config->flight;
                    setRGB0(&quad, red, green, blue);
                    for (k = 0; k < 6; face += 4, k++) {
                        value = RotAverageNclip4(face[0], face[1], face[2], face[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &depth, &flag);
                        if (depth >= 0 && flag >= 0 && value > 0) {
                            GsSortPoly(&quad, ot, (u16)depth);
                        }
                    }
                }
            }
            point->vx += -point->vx / (config->flight - time) * step;
            point->vy += (-350 - point->vy) / (config->flight - time) * step;
            point->vz += (direction * 450 - point->vz) / (config->flight - time) * step;
        }
    }
    glow.tpage = work->texture[0] >> 16;
    glow.clut = work->texture[0];
    SetSemiTrans(&glow, 1);
    setUV4(&glow, 0, 32, 31, 32, 0, 159, 31, 159);
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->delay - i * config->stagger - config->flight;
        if (time >= 0 && time < config->ring) {
            red = config->outer_red * (config->ring - time) / config->ring;
            green = config->outer_green * (config->ring - time) / config->ring;
            blue = config->outer_blue * (config->ring - time) / config->ring;
            setRGB0(&glow, red, green, blue);
            setRGB1(&glow, red, green, blue);
            red = config->inner_red * (config->ring - time) / config->ring;
            green = config->inner_green * (config->ring - time) / config->ring;
            blue = config->inner_blue * (config->ring - time) / config->ring;
            setRGB2(&glow, red, green, blue);
            setRGB3(&glow, red, green, blue);
            value = (time << 12) / config->ring;
            setVector(&scale, value, value, value);
            for (j = 0; j < 4; j++) {
                if ((work->masks[i] >> j) & 1) {
                    setVector(&position, 0, -350, direction * 450);
                    switch (j) {
                    case 0:
                        position.vx = direction * (config->size * 3);
                        break;
                    case 1:
                        position.vx = direction * config->size;
                        break;
                    case 2:
                        position.vx = -(direction * config->size);
                        break;
                    case 3:
                        position.vx = -(direction * (config->size * 3));
                        break;
                    }
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrixZYX(&rotation, &matrix);
                    MulMatrix2(&base, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    RotTransPersN(work->ring, screen, depths, interpolation, flags, 34);
                    sa = screen;
                    sb = &screen[17];
                    da = depths;
                    db = &depths[17];
                    fa = flags;
                    fb = &flags[17];
                    for (k = 0; k < 16; k++) {
                        setXY4(&glow, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
                        depth = AverageZ4(da[0], da[1], db[0], db[1]);
                        flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
                        if (depth >= 0 && !flag) {
                            GsSortPoly(&glow, ot, (u16)depth);
                        }
                        sa++;
                        sb++;
                        da++;
                        db++;
                        fa++;
                        fb++;
                    }
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->delay - config->flight;
    if (time >= 0) {
        time -= config->stagger * config->count;
        if (time < 0) {
            return 4;
        }
        if (time >= config->ring) {
            return 2;
        }
        if (!work->completed) {
            work->completed++;
            return 1;
        }
    }
    return 0;
}
