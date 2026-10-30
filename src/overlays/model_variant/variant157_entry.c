#include "../../types.h"
#include "variant157_entry.h"
#include "../../game/screen_projection.h"

/* A textured streak flies in an arc from a joint of the active model to the
 * other slot, growing as it goes and trailed by ten fading copies, then a
 * 3x3 grid of animated burst quads plays where it lands. A negative command
 * draws a frame; any other command sets the work area up. */

static const VECTOR D_8013C45C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model157UsState *work;
    Model157UsConfig *config;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flat;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin, target;
    PSXLONG flag, interpolation;
    GsOT *ot;
    s32 step;
    SVECTOR *spin, *path;
    SVECTOR *G32 (*corner)[4];
    s32 i, k, x, y, time, index, phase, angle, value, remain;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C45C;
    work = (Model157UsState *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013C540[command % 100];
        path = work->path;
        spin = work->spin;
        index = 0;
        for (k = 0; k < 256; k++) {
            time = index * 0x555 + k;
            phase = time / 256 * 3 / 2;
            y = index * 6 + 512;
            path->vx = direction * (config->radius * csin(phase + 341) / 4096 - config->radius * csin(341) / 4096);
            path->vy = -direction * (config->radius * csin(y) / 4096 - config->radius * csin(512) / 4096);
            path->vz = 0;
            if ((u32)k >> 31 >> 1) {
                k = time;
            }
            x = 683 - phase;
            y = 512 - index * 6;
            x = x < -683 ? -683 : x > 683 ? 683 : x;
            y = y < -512 ? -512 : y > 512 ? 512 : y;
            path++;
            spin->vx = y;
            spin->vy = x;
            spin->vz = 0;
            spin++;
            index++;
        }
        path = work->grid;
        for (k = -1; k < 2; k++) {
            for (i = -1; i < 2; i++) {
                path->vx = i * config->spacing;
                path->vy = k * config->spacing;
                path->vz = 0;
                path++;
            }
        }
        path = work->grid;
        corner = work->quads;
        corner[0][0] = &path[0];
        corner[0][1] = &path[1];
        corner[0][2] = &path[3];
        corner[0][3] = &path[4];
        corner[1][0] = &path[2];
        corner[1][1] = &path[5];
        corner[1][2] = &path[1];
        corner[1][3] = &path[4];
        corner[2][0] = &path[8];
        corner[2][1] = &path[7];
        corner[2][2] = &path[5];
        corner[2][3] = &path[4];
        corner[3][0] = &path[6];
        corner[3][1] = &path[3];
        corner[3][2] = &path[7];
        corner[3][3] = &path[4];
        work->unknown_10aa = 0;
        for (k = 0; k < 7; k++) {
            work->texture[k] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C47C[k]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->announced = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model157UsState), sizeof(Model157UsState));
#endif
        work->repeat = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flat);
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->duration) {
        x = time * 256 / config->duration;
        path = &work->path[x];
        spin = &work->spin[x];
        setVector(&vertices[0], -config->width, 0, direction * config->width / 8);
        setVector(&vertices[1], config->width, 0, direction * config->width / 8);
        setVector(&vertices[2], -config->width, 0, -direction * config->width * 7 / 8);
        setVector(&vertices[3], config->width, 0, -direction * config->width * 7 / 8);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        origin.vx = matrix.t[0];
        origin.vy = matrix.t[1];
        origin.vz = matrix.t[2];
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&target);
        target.vy = -350;
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 0, 0, 127, 0, 0, 63, 127, 63);
        x = (time << 12) / config->duration;
        scale.vx = x;
        scale.vy = x;
        scale.vz = x;
        red = config->head_r;
        green = config->head_g;
        blue = config->head_b;
        if (time >= config->duration * 2 / 3) {
            x = time - config->duration * 2 / 3;
            y = (s16)(config->duration / 3);
            red = red * (y - x) / y;
            green = green * (y - x) / y;
            blue = blue * (y - x) / y;
        }
        setRGB0(&quad, red, green, blue);
        for (k = 0; k < 4; k++) {
            position.vx = path->vx;
            position.vy = path->vy;
            position.vz = path->vz;
            rotation.vx = spin->vx;
            rotation.vy = spin->vy;
            rotation.vz = spin->vz;
            if (k & 1) {
                position.vx = -position.vx;
                rotation.vy = -rotation.vy;
            }
            if (k / 2) {
                position.vy = -position.vy;
                rotation.vx = -rotation.vx;
            }
            position.vx += origin.vx + (target.vx - origin.vx) * time / config->duration / 2 * 3;
            position.vy += origin.vy + (target.vy - origin.vy) * time / config->duration / 2 * 3;
            position.vz = origin.vz + (target.vz - origin.vz) * time / config->duration / 2 * 3;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            x = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (x >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, x);
            }
        }
    }
    setVector(&vertices[0], -config->width, 0, direction * config->width / 8);
    setVector(&vertices[1], config->width, 0, direction * config->width / 8);
    setVector(&vertices[2], -config->width, 0, -direction * config->width * 7 / 8);
    setVector(&vertices[3], config->width, 0, -direction * config->width * 7 / 8);
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    origin.vx = matrix.t[0];
    origin.vy = matrix.t[1];
    origin.vz = matrix.t[2];
    Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&target);
    target.vy = -350;
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    setUV4(&quad, 0, 0, 127, 0, 0, 63, 127, 63);
    for (i = 1; i < 11; i++) {
        time = work->elapsed - config->delay - i * 2;
        if (time >= 0 && time < config->duration) {
            x = time * 256 / config->duration;
            path = &work->path[x];
            spin = &work->spin[x];
            x = (time << 12) / config->duration;
            scale.vx = x;
            scale.vy = x;
            scale.vz = x;
            red = config->trail_r * (11 - i) / 10;
            green = config->trail_g * (11 - i) / 10;
            blue = config->trail_b * (11 - i) / 10;
            if (time >= config->duration * 2 / 3) {
                x = time - config->duration * 2 / 3;
                y = (s16)(config->duration / 3);
                red = red * (y - x) / y;
                green = green * (y - x) / y;
                blue = blue * (y - x) / y;
            }
            setRGB0(&quad, red, green, blue);
            for (k = 0; k < 4; k++) {
                position.vx = path->vx;
                position.vy = path->vy;
                position.vz = path->vz;
                rotation.vx = spin->vx;
                rotation.vy = spin->vy;
                rotation.vz = spin->vz;
                if (k & 1) {
                    position.vx = -position.vx;
                    rotation.vy = -rotation.vy;
                }
                if (k / 2) {
                    position.vy = -position.vy;
                    rotation.vx = -rotation.vx;
                }
                position.vx += origin.vx + (target.vx - origin.vx) * time / config->duration / 2 * 3;
                position.vy += origin.vy + (target.vy - origin.vy) * time / config->duration / 2 * 3;
                position.vz = origin.vz + (target.vz - origin.vz) * time / config->duration / 2 * 3;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                x = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (x >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, x);
                }
            }
        }
    }
    time = work->elapsed - config->delay - config->duration * 2 / 3;
    if (time >= 0 && time < config->burst_duration) {
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        SetSemiTrans(&quad, 1);
        x = time * 6 / config->burst_duration;
        value = x % 4 * 32;
        remain = x / 4 * 32 + 64;
        setUV4(&quad, value, remain, value + 31, remain, value, remain + 31, value + 31, remain + 31);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&target);
        position.vy = target.vy = -350;
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        position.vx = target.vx;
        position.vz = target.vz;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        red = config->burst_r;
        green = config->burst_g;
        blue = config->burst_b;
        setRGB0(&quad, red, green, blue);
        corner = work->quads;
        for (k = 0; k < 4; corner++, k++) {
            x = RotAverage4((*corner)[0], (*corner)[1], (*corner)[2], (*corner)[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (x >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, x);
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->delay - config->duration * 2 / 3;
    if (time >= 0) {
        angle = time - 5;
        time = angle - (s16)(config->duration / 3);
        if (time > 0) {
            work->repeat++;
            if (work->repeat < config->repeats) {
                work->elapsed = config->delay - 2;
                work->announced = 0;
                return 5;
            }
            return 2;
        }
        if (work->announced == 0) {
            work->announced++;
            if (work->repeat < config->repeats - 1) {
                return 4;
            }
            return 1;
        }
        return 0;
    }
    return 0;
}
