#include "../../types.h"
#include "primary62.h"

s32 func_8013A004(u8 *context, s32 command)
{
    ModelPrimary62State *work;
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
    s32 direction;
    GsOT *ot;
    ModelPrimary62Config *config;
    s32 i;
    s32 angle0;
    s32 angle1;
    s32 radius;
    s32 red, green, blue;
    SVECTOR *point;
    u8 *frame;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013A628;
    work = (ModelPrimary62State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013A750[command];
        point = work->points;
        for (i = 0; i < config->count; point++, i++) {
            angle0 = rand() % 4096;
            angle1 = rand() % 4096;
            radius = rand() % config->radius;
            point->vx = radius * ccos(angle0) / 4096 * ccos(angle1) / 4096;
            point->vy = radius * ccos(angle0) / 4096 * csin(angle1) / 4096;
            point->vz = -direction * radius;
            work->frames[i] = (rand() >> 8) % 8;
        }
        work->texture = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013A638);
        work->delay = -config->delay;
        return 0;
    }

    config = work->config;
    ot = func_80058F10();
    if (work->delay < 0) {
        work->delay += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyFT4(&quad);
    quad.tpage = work->texture >> 16;
    quad.clut = work->texture;
    color = *(CVECTOR *)func_80059520(Model_GetActiveSlotIndex());
    red = config->r * color.r / 128;
    green = config->g * color.g / 128;
    blue = config->b * color.b / 128;
    quad.r0 = red;
    quad.g0 = green;
    quad.b0 = blue;
    corners[0].vx = -config->size;
    corners[0].vy = -config->size;
    corners[0].vz = 0;
    corners[1].vx = config->size;
    corners[1].vy = -config->size;
    corners[1].vz = 0;
    corners[2].vx = -config->size;
    corners[2].vy = config->size;
    corners[2].vz = 0;
    corners[3].vx = config->size;
    corners[3].vy = config->size;
    corners[3].vz = 0;
    point = work->points;
    frame = work->frames;
    for (i = 0; i < config->count; point++, frame++, i++) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        position.vx += point->vx;
        position.vy += point->vy;
        position.vz += point->vz;
        position.vy -= *frame * config->travel / 8;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, (PSXLONG *)&flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                            (PSXLONG *)&quad.x3, (PSXLONG *)&interpolation, (PSXLONG *)&flag);
        quad.u0 = *frame << 5;
        quad.v0 = 224;
        quad.u1 = (*frame << 5) + 31;
        quad.v1 = 224;
        quad.u2 = *frame << 5;
        quad.v2 = 255;
        quad.u3 = (*frame << 5) + 31;
        quad.v3 = 255;
        SetSemiTrans(&quad, 1);
        GsSortPoly(&quad, ot, depth & 0xFFFF);
        *frame = (*frame + 1) % 8;
    }
    PopMatrix();
    return 0;
}
