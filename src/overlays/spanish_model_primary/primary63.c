#include "../../types.h"
#include "primary63.h"

s32 func_8013A004(u8 *context, s32 command)
{
    ModelPrimary63State *work;
    MATRIX base;
    MATRIX matrix;
    GsBOXF box;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    DVECTOR projected[48];
    u16 depths[48];
    u16 interpolation[48];
    u16 flags[48];
    CVECTOR color;
    s32 depth;
    s32 flag;
    GsOT *ot;
    ModelPrimary63Config *config;
    SVECTOR *point;
    SVECTOR *velocity;
    s32 i, j;
    s32 a, b, radius;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013AA08;
    work = (ModelPrimary63State *)context;
    Model_GetActiveSlotIndex();
    if (command >= 0) {
        config = work->config = &D_8013AA18[command];
        point = work->points[0];
        velocity = work->velocities[0];
        for (i = 0; i < 16; point++, velocity++, i++) {
            a = rand() % 4096;
            b = rand() % 4096;
            radius = config->radius / config->period;
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            velocity->vx = radius * ccos(a) / 4096 * ccos(b) / 4096;
            velocity->vy = radius * ccos(a) / 4096 * csin(b) / 4096;
            velocity->vz = radius * csin(a) / 4096;
        }
        for (i = 0; i < 16; point++, velocity++, i++) {
            a = rand() % 4096;
            b = rand() % 4096;
            radius = config->radius / config->period;
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            velocity->vx = radius * ccos(a) / 4096 * ccos(b) / 4096;
            velocity->vy = radius * ccos(a) / 4096 * csin(b) / 4096;
            velocity->vz = radius * csin(a) / 4096;
        }
        for (i = 0; i < 16; point++, velocity++, i++) {
            a = rand() % 4096;
            b = rand() % 4096;
            radius = config->other_radius / config->period;
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            velocity->vx = radius * ccos(a) / 4096 * ccos(b) / 4096;
            velocity->vy = radius * ccos(a) / 4096 * csin(b) / 4096;
            velocity->vz = radius * csin(a) / 4096;
        }
        work->frame = 0;
        return 0;
    }

    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    PushMatrix();
    box.attribute = 0x50000000;
    box.w = 1;
    box.h = 1;
    color = *(CVECTOR *)func_80059520(Model_GetActiveSlotIndex());
    b = Model_GetSlotAnimationIndex(Model_GetActiveSlotIndex());
    radius = Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex());
    if (!(b == 4 && radius > 140 && radius < 239)
        && !(b == 6 && radius >= 47)) {
        for (i = 0; i < 3; i++) {
            if (b == 3) {
                if (radius >= 50 && i == 1) {
                    continue;
                }
                if (radius >= 118 && i == 0) {
                    continue;
                }
            }
            point = work->points[i];
            velocity = work->velocities[i];
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->parts[i]),
                        &matrix);
            position.vx = matrix.t[0];
            position.vy = matrix.t[1];
            position.vz = matrix.t[2];
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, (PSXLONG *)&flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(point, projected, depths, interpolation, flags, 16);
            for (j = 0; j < 16; point++, velocity++, j++) {
                box.x = projected[j].vx;
                box.y = projected[j].vy;
                a = config->period - (j + work->frame) % config->period;
                box.r = config->r * a / config->period * color.r / 128;
                box.g = config->g * a / config->period * color.g / 128;
                box.b = config->b * a / config->period * color.b / 128;
                GsSortBoxFill(&box, ot, depths[j] >> 2);
                a = (work->frame + j) % config->period;
                point->vx = velocity->vx;
                point->vy = velocity->vy;
                point->vz = velocity->vz;
                point->vx *= a;
                point->vy *= a;
                point->vz *= a;
            }
        }
    }
    PopMatrix();
    work->frame += Model_GetFrameStep();
    return 0;
}
