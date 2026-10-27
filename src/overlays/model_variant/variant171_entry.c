#include "../../types.h"
#include "variant171_entry.h"
#include "../../game/screen_projection.h"

/* Header 171's entry: groups of points spread through a sphere in front of the
 * other slot play an eight-frame texture as wobbling quads that converge on
 * one of the slot's parts while their colour rises and fades, then a fixed
 * burst frame fades out at each point's starting position. */

static const VECTOR D_8013BF34 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model171State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR origin;
    PSXLONG flag, interpolation;
    GsOT *ot;
    Model171Config *config;
    s32 step;
    SVECTOR *point, *start;
    s32 i, j, a, b, radius, time, remain;
    PSXLONG depth;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BF34;
    work = (Model171State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        point = work->positions;
        start = work->starts;
        config = work->config = &D_8013C0DC[command % 100];
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);
        vertices[0].vy = -350;
        for (i = 0; i <= 512; i++) {
            a = rand() % 4096;
            b = rand() % 2048;
            radius = rand() % config->radius;
            point->vx = radius * csin(a) / 4096 * csin(b) / 4096;
            point->vy = radius * ccos(b) / 4096;
            point->vz = radius * ccos(a) / 4096 * csin(b) / 4096;
            point->vx += vertices[0].vx;
            point->vy += vertices[0].vy;
            point->vz += vertices[0].vz;
            start->vx = point->vx;
            start->vy = point->vy;
            start->vz = point->vz;
            point++;
            start++;
        }
        for (i = 0; i < 14; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013BF54[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model171State), sizeof(Model171State));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    SetSemiTrans(&quad, 1);
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    setVector(&origin, matrix.t[0], matrix.t[1], matrix.t[2]);
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], config->half_size, -config->half_size, 0);
    setVector(&vertices[2], -config->half_size, config->half_size, 0);
    setVector(&vertices[3], config->half_size, config->half_size, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->positions;
    for (i = 0; i < config->total / config->group; i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time >= 0 && time < config->gather_duration) {
            b = config->total / config->group * config->spacing + config->gather_duration;
            a = b - work->elapsed + config->delay;
            red = config->red * time / config->gather_duration * a / b;
            green = config->green * time / config->gather_duration * a / b;
            blue = config->blue * time / config->gather_duration * a / b;
            setRGB0(&quad, red, green, blue);
            a = time * 8 / config->gather_duration;
            setUV4(&quad, a % 4 * 64, a / 4 * 64, a % 4 * 64 + 63, a / 4 * 64,
                   a % 4 * 64, a / 4 * 64 + 63, a % 4 * 64 + 63, a / 4 * 64 + 63);
            for (j = 0; j < config->group; j++) {
                setVector(&position, point->vx, point->vy, point->vz);
                position.vy -= (point->vy - origin.vy) / 2 * csin((time << 11) / config->gather_duration) / 4096;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                point->vx += (origin.vx - point->vx) / (config->gather_duration - time) * step;
                point->vy += (origin.vy - point->vy) / (config->gather_duration - time) * step;
                point->vz += (origin.vz - point->vz) / (config->gather_duration - time) * step;
                point++;
            }
        } else {
            point += config->group;
        }
    }
    quad.tpage = work->texture[8] >> 16;
    quad.clut = work->texture[8];
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[1], config->burst_half_size, -config->burst_half_size, 0);
    setVector(&vertices[2], -config->burst_half_size, config->burst_half_size, 0);
    setVector(&vertices[3], config->burst_half_size, config->burst_half_size, 0);
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    point = work->starts;
    for (i = 0; i < config->total / config->group; i++) {
        time = work->elapsed - config->delay - i * config->spacing;
        if (time >= 0 && time < config->burst_duration) {
            remain = config->burst_duration - time;
            red = config->burst_red * remain / config->burst_duration;
            green = config->burst_green * remain / config->burst_duration;
            blue = config->burst_blue * remain / config->burst_duration;
            setRGB0(&quad, red, green, blue);
            setUV4(&quad, 64, 128, 95, 128, 64, 159, 95, 159);
            for (j = 0; j < config->group; j++) {
                setVector(&position, point->vx, point->vy, point->vz);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                point++;
            }
        } else {
            point += config->group;
        }
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->delay;
    if (time < 0) {
        return 0;
    }
    time -= config->spacing * config->total / config->group + config->gather_duration;
    if (time < 0) {
        return 4;
    }
    if (work->completed != 0) {
        return 2;
    }
    work->completed++;
    return 1;
}
