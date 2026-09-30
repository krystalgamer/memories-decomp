#include "../../types.h"
#include "variant87_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Family87State *work;
    MATRIX base;
    MATRIX matrix;
    POLY_G3 triangle;
    GsBOXF box;
    DVECTOR projected[26];
    u16 depths[26];
    u16 interpolation[26];
    u16 flags[26];
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    s32 flag;
    GsOT *ot;
    Family87Config *config;
    SVECTOR *point;
    s32 direction;
    s32 i, j;
    s32 size;
    s32 depth;
    s32 end;
    s32 frame;
    s32 start;
    s32 result;

    memset(&rotation, 0, 8);
    work = (Family87State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BA1C[command];
        point = work->points;
        point->vx = (config->radius * 2 / 3) * ccos(2560) / 4096;
        point->vy = (config->radius * 2 / 3) * csin(2560) / 4096;
        point->vz = 0;
        point++;
        for (i = 0; i < 25; point++, i++) {
            point->vx = config->radius * ccos(i * 4096 / 24) / 4096;
            point->vy = config->radius * csin(i * 4096 / 24) / 4096;
            point->vz = 0;
        }
        point = (SVECTOR *)work->destination_storage;
        for (i = 0; i < config->count; point++, i++) {
            point->vx = rand() % (config->spread * 2) - config->spread;
            point->vy = rand() % (config->spread * 2) - (u32)(config->spread + 350);
            size = rand();
            depth = config->spread;
            size %= depth * 2;
            depth -= 450;
            size -= depth;
            point->vz = direction * size;
        }
        work->frame = -config->delay;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    SetPolyG3(&triangle);
    box.w = box.h = 1;
    box.attribute = 0x50000000;
    setRGB0(&triangle, config->inner[0], config->inner[1], config->inner[2]);
    setRGB1(&triangle, config->outer[0], config->outer[1], config->outer[2]);
    setRGB2(&triangle, config->outer[0], config->outer[1], config->outer[2]);
    if (work->frame < 0) {
        work->frame += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    point = (SVECTOR *)work->destination_storage;
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    for (i = 0; i < config->count; point++, i++) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        start = i * config->interval;
        frame = work->frame;
        if (frame <= start) {
            scale.vz = scale.vy = scale.vx = 0;
        } else if (frame < (end = start + config->growth + config->hold)) {
            position.vx = matrix.t[0] + (point->vx - matrix.t[0]) *
                (work->frame - i * config->interval) / (config->growth + config->hold);
            position.vy = matrix.t[1] + (point->vy - matrix.t[1]) *
                (work->frame - i * config->interval) / (config->growth + config->hold);
            position.vz = matrix.t[2] + (point->vz - matrix.t[2]) *
                (work->frame - i * config->interval) / (config->growth + config->hold);
            size = ((work->frame - i * config->interval) << 12) / config->growth;
            size = size < 0 ? 0 : size > 4096 ? 4096 : size;
            scale.vz = scale.vy = scale.vx = size;
        } else if (frame < (end += config->fade)) {
            position.vx = point->vx;
            position.vy = point->vy;
            position.vz = point->vz;
            end -= frame;
            box.r = config->inner[0] * end / config->fade;
            box.g = config->inner[1] * end / config->fade;
            box.b = config->inner[2] * end / config->fade;
            size = 4096 +
                ((work->frame - i * config->interval - config->growth - config->hold) << 10) / config->fade;
            scale.vz = scale.vy = scale.vx = size;
        }
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, (PSXLONG *)&flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->points, projected, depths, interpolation, flags, 26);
        for (j = 1; j < 25; j++) {
            if (work->frame < i * config->interval + config->growth + config->hold) {
                setXY3(&triangle, projected[0].vx, projected[0].vy,
                       projected[j].vx, projected[j].vy, projected[j + 1].vx, projected[j + 1].vy);
                depth = AverageZ3(depths[0], depths[j], depths[j + 1]);
                func_8005B260((u32 *)&triangle, ot, depth & 0xFFFF, 1);
            } else if (work->frame < i * config->interval + config->growth + config->hold + config->fade) {
                box.x = projected[j].vx;
                box.y = projected[j].vy;
                GsSortBoxFill(&box, ot, depths[0] >> 2);
            }
        }
    }
    PopMatrix();
    work->frame += Model_GetFrameStep();
    if (work->frame < config->growth + config->hold) {
        return 0;
    }
    if (work->frame <= config->interval * config->count + config->growth + config->hold + config->fade) {
        return 4;
    }
    if (work->completed) {
        result = 2;
    } else {
        work->completed++;
        result = 1;
    }
    return result;
}
