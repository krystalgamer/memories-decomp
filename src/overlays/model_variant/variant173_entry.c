#include "../../types.h"
#include "variant173_entry.h"
#include "../../game/screen_projection.h"

/* Header 173's entry: each particle starts at the active slot's position and
 * drifts to a point in front of it, drawn as a row of four spinning cubes
 * whose faces are textured where the particle's random mask bit is set and
 * fade out otherwise. Once a particle arrives, every masked cube fires an
 * expanding double-ring beam from that point. A negative command draws a
 * frame; any other command sets the work area up and prints its size. */

static const VECTOR D_8013C2D0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_GT4 beam;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    DVECTOR screen[34];
    u16 depths[34], interpolation[34], flags[34];
    PSXLONG flag, p, otz;
    Model173State *work;
    s32 direction;
    GsOT *ot;
    s32 step;
    s32 i, j, k, t, a, b;
    u8 red, green, blue;
    s32 nclip;
    Model173Config *config;
    SVECTOR *point, *other;
    SVECTOR *G32 *face;
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa, *fb;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C2D0;
    work = (Model173State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->corners;
        config = work->config = &D_8013C37C[command % 100];
        for (i = -1; i < 2; i += 2) {
            for (j = -1; j < 2; j += 2) {
                for (k = -1; k < 2; k += 2) {
                    point->vx = j * config->size;
                    point->vy = i * config->size;
                    point->vz = k * config->size;
                    point++;
                }
            }
        }
        point = work->corners;
        face = work->faces;
        face[0] = point;
        face[1] = &work->corners[1];
        face[2] = &work->corners[2];
        face[3] = &work->corners[3];
        face[4] = &work->corners[1];
        face[5] = &work->corners[5];
        face[6] = &work->corners[3];
        face[7] = &work->corners[7];
        face[8] = &work->corners[5];
        face[9] = &work->corners[4];
        face[10] = &work->corners[7];
        face[11] = &work->corners[6];
        face[12] = &work->corners[4];
        face[13] = point;
        face[14] = &work->corners[6];
        face[15] = &work->corners[2];
        face[16] = &work->corners[1];
        face[17] = point;
        face[18] = &work->corners[5];
        face[19] = &work->corners[4];
        face[20] = &work->corners[2];
        face[21] = &work->corners[3];
        face[22] = &work->corners[6];
        face[23] = &work->corners[7];
        for (i = 0; i < config->count; i++) {
            a = rand();
            b = rand();
            a %= 6;
            a = 1 << a;
            a &= 0xF;
            work->masks[i] = a + ((1 << (b % 4)) & 0xF);
        }
        point = work->ring;
        other = &work->ring[17];
        for (i = 0; i < 17; i++) {
            point->vx = config->outer * ccos(i << 8) / 4096;
            point->vy = config->outer * csin(i << 8) / 4096;
            point->vz = -direction * (config->outer - config->inner) / 3;
            other->vx = config->inner * ccos(i << 8) / 4096;
            other->vy = config->inner * csin(i << 8) / 4096;
            other->vz = 0;
            point++;
            other++;
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C2F0[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model173State), sizeof(Model173State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyGT4(&beam);
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    setVector(&scale, 4096, 4096, 4096);
    point = work->positions;
    for (i = 0; i < config->count; i++) {
        t = work->elapsed - config->delay - i * config->spacing;
        if (t < 0) {
            Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)point);
            point->vy = 0;
        } else if (t < config->duration) {
            for (j = 0; j < 4; j++) {
                setVector(&position, point->vx, point->vy, point->vz);
                switch (j) {
                case 0:
                    position.vx += direction * (config->size * 3);
                    setVector(&rotation, -t * 64, direction * 1024, 0);
                    break;
                case 1:
                    position.vx += direction * config->size;
                    setVector(&rotation, -t * 64, direction * 2048, 0);
                    break;
                case 2:
                    position.vx -= direction * config->size;
                    setVector(&rotation, -t * 64, 0, 0);
                    break;
                case 3:
                    position.vx -= direction * (config->size * 3);
                    setVector(&rotation, -t * 64, direction * 3072, 0);
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
                    for (k = 0; k < 4; k++) {
                        nclip = RotAverageNclip4(face[0], face[1], face[2], face[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &otz, &flag);
                        if (otz >= 0 && flag >= 0 && nclip > 0) {
                            GsSortPoly(&quad, ot, (u16)otz);
                        }
                        face += 4;
                    }
                    quad.tpage = work->texture[4] >> 16;
                    quad.clut = work->texture[4];
                    setUV4(&quad, 96, 0, 127, 0, 96, 31, 127, 31);
                    for (k = 4; k < 6; k++) {
                        nclip = RotAverageNclip4(face[0], face[1], face[2], face[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &otz, &flag);
                        if (otz >= 0 && flag >= 0 && nclip > 0) {
                            GsSortPoly(&quad, ot, (u16)otz);
                        }
                        face += 4;
                    }
                } else {
                    quad.tpage = work->texture[4] >> 16;
                    quad.clut = work->texture[4];
                    SetSemiTrans(&quad, 1);
                    setUV4(&quad, 96, 0, 127, 0, 96, 31, 127, 31);
                    red = config->red * (config->duration - t) / config->duration;
                    green = config->green * (config->duration - t) / config->duration;
                    blue = config->blue * (config->duration - t) / config->duration;
                    setRGB0(&quad, red, green, blue);
                    for (k = 0; k < 6; k++) {
                        nclip = RotAverageNclip4(face[0], face[1], face[2], face[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &otz, &flag);
                        if (otz >= 0 && flag >= 0 && nclip > 0) {
                            GsSortPoly(&quad, ot, (u16)otz);
                        }
                        face += 4;
                    }
                }
            }
            point->vx += -point->vx / (config->duration - t) * step;
            point->vy += (-350 - point->vy) / (config->duration - t) * step;
            point->vz += (direction * 450 - point->vz) / (config->duration - t) * step;
        }
        point++;
    }
    beam.tpage = work->texture[0] >> 16;
    beam.clut = work->texture[0];
    SetSemiTrans(&beam, 1);
    setUV4(&beam, 0, 32, 31, 32, 0, 159, 31, 159);
    setVector(&rotation, 0, 0, 0);
    for (i = 0; i < config->count; i++) {
        t = work->elapsed - config->delay - i * config->spacing - config->duration;
        if (t >= 0 && t < config->fade) {
            red = config->start_red * (config->fade - t) / config->fade;
            green = config->start_green * (config->fade - t) / config->fade;
            blue = config->start_blue * (config->fade - t) / config->fade;
            beam.r0 = red;
            beam.r1 = red;
            beam.g0 = green;
            beam.g1 = green;
            beam.b0 = blue;
            beam.b1 = blue;
            red = config->end_red * (config->fade - t) / config->fade;
            green = config->end_green * (config->fade - t) / config->fade;
            blue = config->end_blue * (config->fade - t) / config->fade;
            beam.r2 = red;
            beam.r3 = red;
            beam.g2 = green;
            beam.g3 = green;
            beam.b2 = blue;
            beam.b3 = blue;
            nclip = (t << 12) / config->fade;
            setVector(&scale, nclip, nclip, nclip);
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
                        setXY4(&beam, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
                        otz = AverageZ4(da[0], da[1], db[0], db[1]);
                        flag = (fa[0] | fa[1] | fb[0] | fb[1]) & 0x20;
                        if (otz >= 0 && !flag) {
                            GsSortPoly(&beam, ot, (u16)otz);
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
    t = work->elapsed - config->delay - config->duration;
    if (t < 0) {
        return 0;
    }
    t -= config->spacing * config->count;
    if (t < 0) {
        return 4;
    }
    if (t >= config->fade) {
        return 2;
    }
    if (work->completed) {
        return 0;
    }
    work->completed++;
    return 1;
}
