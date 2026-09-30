#include "../../types.h"
#include "primary64.h"

s32 func_8013A004(u8 *context, s32 command)
{
    ModelPrimary64State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_FT4 quad;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR corners[4];
    CVECTOR color;
    s32 depth;
    s32 flag;
    s32 interpolation;
    GsOT *ot;
    ModelPrimary64Config *config;
    SVECTOR *point;
    u8 *part;
    s16 *width;
    s16 *height;
    s16 *jitter;
    s32 i;
    s32 red, green, blue;

    memset(&rotation, 0, 8);
    scale = D_8013A994;
    work = (ModelPrimary64State *)context;
    Model_GetActiveSlotIndex();
    if (command >= 0) {
        config = work->config = &D_8013AA30[command];
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        PushMatrix();
        point = work->points;
        part = config->parts;
        for (i = 0; i < 12; point++, part++, i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), *part), &matrix);
            point->vx = matrix.t[0];
            point->vy = matrix.t[1];
            point->vz = matrix.t[2];
        }
        PopMatrix();
        for (i = 0; i < 5; i++) {
            work->textures[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013A9A4[i]);
        }
        work->frame = 0;
        return 0;
    }

    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyFT4(&quad);
    quad.tpage = work->textures[1] >> 16;
    quad.clut = work->textures[1];
    SetSemiTrans(&quad, 1);
    quad.u0 = (work->frame / 2 % 8) << 5;
    quad.v0 = 0;
    quad.u1 = ((work->frame / 2 % 8) << 5) + 31;
    quad.v1 = 0;
    quad.u2 = (work->frame / 2 % 8) << 5;
    quad.v2 = 63;
    quad.u3 = ((work->frame / 2 % 8) << 5) + 31;
    quad.v3 = 63;
    PushMatrix();
    color = *(CVECTOR *)func_80059520(Model_GetActiveSlotIndex());
    red = config->r * color.r / 128;
    green = config->g * color.g / 128;
    blue = config->b * color.b / 128;
    quad.r0 = red;
    quad.g0 = green;
    quad.b0 = blue;
    point = work->points;
    part = config->parts;
    width = config->widths;
    height = config->heights;
    jitter = config->jitter;
    for (i = 0; i < config->count; point++, part++, width++, height++, jitter++, i++) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), *part), &matrix);
        position.vx = point->vx;
        position.vy = point->vy;
        position.vz = point->vz;
        position.vx -= matrix.t[0];
        position.vy -= matrix.t[1];
        position.vz -= matrix.t[2];
        position.vx -= rand() % *jitter;
        position.vy -= rand() % *jitter;
        position.vz -= rand() % *jitter;
        position.vx += rand() % *jitter;
        position.vy += rand() % *jitter;
        position.vz += rand() % *jitter;
        position.vx /= -config->divisor;
        position.vy /= -config->divisor;
        position.vz /= -config->divisor;
        point->vx += position.vx;
        point->vy += position.vy;
        point->vz += position.vz;
        corners[0].vx = -*width;
        corners[0].vy = -*height - 150;
        corners[0].vz = 0;
        corners[1].vx = *width;
        corners[1].vy = -*height - 150;
        corners[1].vz = 0;
        corners[2].vx = -*width;
        corners[2].vy = *height - 150;
        corners[2].vz = 0;
        corners[3].vx = *width;
        corners[3].vy = *height - 150;
        corners[3].vz = 0;
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, (PSXLONG *)&flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPers(&corners[2], (PSXLONG *)&quad.x2, (PSXLONG *)&interpolation, (PSXLONG *)&flag);
        RotTransPers(&corners[3], (PSXLONG *)&quad.x3, (PSXLONG *)&interpolation, (PSXLONG *)&flag);
        position.vx = point->vx;
        position.vy = point->vy;
        position.vz = point->vz;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, (PSXLONG *)&flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotTransPers(&corners[0], (PSXLONG *)&quad.x0, (PSXLONG *)&interpolation, (PSXLONG *)&flag);
        depth += RotTransPers(&corners[1], (PSXLONG *)&quad.x1, (PSXLONG *)&interpolation, (PSXLONG *)&flag);
        depth /= 2;
        GsSortPoly(&quad, ot, depth & 0xFFFF);
    }
    work->frame += Model_GetFrameStep();
    PopMatrix();
    return 0;
}
