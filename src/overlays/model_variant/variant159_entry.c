#include "../../types.h"
#include "variant159_entry.h"
#include "../../game/screen_projection.h"

/* Header 159's entry: a rainbow curtain of five rows rises out of the
 * floor, waves and slides towards the other slot, then eight glowing
 * three-sided prisms grow along its edge. A negative command draws a frame;
 * any other command builds the curtain and prints the work-area size. */

static const VECTOR D_8013CD74 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    POLY_G4 band;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[12];
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    PSXLONG flag, interpolation;
    Model159State *work;
    s32 direction;
    GsOT *ot;
    s32 step;
    Model159Config *config;
    CVECTOR *prev;
    DVECTOR *sa, *sb;
    u16 *da, *db, *fa, *fb;
    SVECTOR *point;
    SVECTOR *vertex;
    s32 j;
    s32 time;
    CVECTOR *cur;
    s32 i, a, b, fade, depth, lower;
    s32 level;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CD74;
    work = (Model159State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013CDB0[command % 100];
        point = work->edge;
        for (i = 0; i < 17; i++) {
            point->vx = -direction * (config->width * (i << 1) / 16 - config->width);
            point->vy = 0;
            point->vz = config->wave * csin(i * 512) / 4096;
            point++;
        }
        prev = work->colors[0];
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 17; j++) {
                a = ((j + i * 4) % 17 << 12) / 17;
                prev->r = csin(a) / 32 + 127;
                prev->g = csin(a + 0x555) / 32 + 127;
                prev->b = csin(a + 0xAAA) / 32 + 127;
                prev->r = prev->r * csin(j * 128) / 4096;
                prev->g = prev->g * csin(j * 128) / 4096;
                prev->b = prev->b * csin(j * 128) / 4096;
                if (j == 0) {
                    prev->r = prev->g = prev->b = 0;
                }
                prev++;
            }
        }
        work->depth = -direction * 225;
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CD94[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model159State), sizeof(Model159State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetPolyG4(&band);
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->rise + config->hold + config->fall) {
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&position, 0, 0, work->depth);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->edge, screen, depths, projection, flags, 17);
        prev = work->colors[0];
        cur = work->colors[1];
        for (i = 1; i < 5; i++) {
            a = i % 2 * 17;
            b = (i + 1) % 2 * 17;
            setVector(&position, 0, -config->height * i / 4, work->depth);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->edge, &screen[a], &depths[a], &projection[a], &flags[a], 17);
            sb = &screen[b];
            sa = &screen[a];
            db = &depths[b];
            da = &depths[a];
            fb = &flags[b];
            fa = &flags[a];
            fade = config->rise / 16;
            if (fade < 1) {
                fade = 1;
            }
            lower = i - 1;
            for (j = 0; j < 16; j++) {
                setXY4(&band, sa[0].vx, sa[0].vy, sa[1].vx, sa[1].vy, sb[0].vx, sb[0].vy, sb[1].vx, sb[1].vy);
                if (time < config->rise) {
                    a = time - config->rise * j / 16;
                    b = time - config->rise * (j + 1) / 16;
                    /* Clamped through a temporary; a ternary folds to a MIN and reorders the branch. */
                    if (a >= 0) {
                        if (a <= fade) {
                            level = a;
                        } else {
                            level = fade;
                        }
                    } else {
                        level = 0;
                    }
                    a = level;
                    if (b >= 0) {
                        if (b <= fade) {
                            level = b;
                        } else {
                            level = fade;
                        }
                    } else {
                        level = 0;
                    }
                    b = level;
                    band.r0 = cur->r * i / 4 * a / fade;
                    band.g0 = cur->g * i / 4 * a / fade;
                    band.b0 = cur->b * i / 4 * a / fade;
                    band.r2 = prev->r * lower / 4 * a / fade;
                    band.g2 = prev->g * lower / 4 * a / fade;
                    band.b2 = prev->b * lower / 4 * a / fade;
                    prev++;
                    cur++;
                    band.r1 = cur->r * i / 4 * b / fade;
                    band.g1 = cur->g * i / 4 * b / fade;
                    band.b1 = cur->b * i / 4 * b / fade;
                    band.r3 = prev->r * lower / 4 * b / fade;
                    band.g3 = prev->g * lower / 4 * b / fade;
                    band.b3 = prev->b * lower / 4 * b / fade;
                } else if (time < config->rise + config->hold) {
                    band.r0 = cur->r * i / 4;
                    band.g0 = cur->g * i / 4;
                    band.b0 = cur->b * i / 4;
                    band.r2 = prev->r * lower / 4;
                    band.g2 = prev->g * lower / 4;
                    band.b2 = prev->b * lower / 4;
                    prev++;
                    cur++;
                    band.r1 = cur->r * i / 4;
                    band.g1 = cur->g * i / 4;
                    band.b1 = cur->b * i / 4;
                    band.r3 = prev->r * lower / 4;
                    band.g3 = prev->g * lower / 4;
                    band.b3 = prev->b * lower / 4;
                } else if (time < config->rise + config->hold + config->fall) {
                    a = config->rise + config->hold + config->fall - time;
                    band.r0 = cur->r * i / 4 * a / config->fall;
                    band.g0 = cur->g * i / 4 * a / config->fall;
                    band.b0 = cur->b * i / 4 * a / config->fall;
                    band.r2 = prev->r * lower / 4 * a / config->fall;
                    band.g2 = prev->g * lower / 4 * a / config->fall;
                    band.b2 = prev->b * lower / 4 * a / config->fall;
                    prev++;
                    cur++;
                    band.r1 = cur->r * i / 4 * a / config->fall;
                    band.g1 = cur->g * i / 4 * a / config->fall;
                    band.b1 = cur->b * i / 4 * a / config->fall;
                    band.r3 = prev->r * lower / 4 * a / config->fall;
                    band.g3 = prev->g * lower / 4 * a / config->fall;
                    band.b3 = prev->b * lower / 4 * a / config->fall;
                }
                depth = AverageZ4(db[0], db[1], da[0], da[1]);
                flag = (fb[0] | fb[1] | fa[0] | fa[1]) & 0x20;
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&band, ot, (u16)depth, 1);
                }
                sb++;
                sa++;
                db++;
                da++;
                fb++;
                fa++;
            }
            prev++;
            cur++;
        }
        point = work->edge;
        for (i = 0; i < 17; i++) {
            point->vz = config->wave * csin(i * 512 + time * config->speed) / 4096;
            point++;
        }
        a = time - config->rise;
        if (a >= 0 && a < config->hold + config->fall) {
            work->depth += direction * 675 / config->hold * step;
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 0, 0, 31, 0, 0, 127, 31, 127);
    setVector(&vertices[0], -config->size / 2, -config->size * 6, 0);
    setVector(&vertices[1], config->size / 2, -config->size * 6, 0);
    setVector(&vertices[2], -config->size / 2, -config->size * 2, 0);
    setVector(&vertices[3], config->size / 2, -config->size * 2, 0);
    setVector(&vertices[4], -config->size, -config->size * 4, -direction * config->size);
    setVector(&vertices[5], -config->size / 2, -config->size * 6, 0);
    setVector(&vertices[6], -config->size, 0, -direction * config->size);
    setVector(&vertices[7], -config->size / 2, -config->size * 2, 0);
    setVector(&vertices[8], config->size / 2, -config->size * 6, 0);
    setVector(&vertices[9], config->size, -config->size * 4, -direction * config->size);
    setVector(&vertices[10], config->size / 2, -config->size * 2, 0);
    setVector(&vertices[11], config->size, 0, -direction * config->size);
    point = work->sprites;
    for (i = 0; i < 8; point++, i++) {
        time = work->elapsed - config->delay - config->rise - config->hold - i * config->fall / 8;
        if (time < 0) {
            setVector(point, 0, -350, work->depth);
        } else if (time < config->sprite) {
            a = (time << 12) / config->sprite;
            setVector(&scale, a, a, a);
            red = config->red + (config->mid_red - config->red) * i / 8;
            red = red + (config->end_red - red) * time / config->sprite;
            red = red * (config->sprite - time) / config->sprite;
            green = config->green + (config->mid_green - config->green) * i / 8;
            green = green + (config->end_green - green) * time / config->sprite;
            green = green * (config->sprite - time) / config->sprite;
            blue = config->blue + (config->mid_blue - config->blue) * i / 8;
            blue = blue + (config->end_blue - blue) * time / config->sprite;
            blue = blue * (config->sprite - time) / config->sprite;
            setRGB0(&quad, red, green, blue);
            copyVector(&position, point);
            setVector(&rotation, 0, 0, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            vertex = vertices;
            for (j = 0; j < 3; j++) {
                depth = RotAverage4(&vertex[0], &vertex[1], &vertex[2], &vertex[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                vertex += 4;
            }
            setVector(&rotation, 0, 0, 2048);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            vertex = vertices;
            for (j = 0; j < 3; j++) {
                depth = RotAverage4(&vertex[0], &vertex[1], &vertex[2], &vertex[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                vertex += 4;
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->delay - config->rise - config->hold;
    if (time < 0) {
        return 0;
    }
    time -= config->fall + config->sprite;
    if (time > 0) {
        if (work->completed) {
            return 2;
        }
        work->completed++;
        return 1;
    }
    return 4;
}
