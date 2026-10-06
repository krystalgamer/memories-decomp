#include "../../types.h"
#include "variant107_entry.h"

static const VECTOR D_8013C188 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model107State *work;
    Model107Config *config;
    MATRIX base, matrix;
    POLY_GT4 quad;
    POLY_G4 unused_quad;
    GsGLINE line;
    SVECTOR rotation, position;
    VECTOR scale;
    DISPENV display;
    DVECTOR projected[51];
    u16 depths[51], interpolation[51], flags[51];
    PSXLONG flag, depth;
    s32 direction;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j, age, return_age, step, burst_age, value;
    s32 inner;
    s32 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C188;
    work = (Model107State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    value = 0;
    if (command >= 0) {
        config = work->config = &D_8013C198[command];
        point = work->ring;
        for (i = 0; i < 17; point++, i++) {
            point->vx = config->radius * ccos(i << 8) / 4096;
            point->vy = config->radius * csin(i << 8) / 4096;
            point->vz = 0;
        }
        for (i = 0; i < 17; point++, i++) {
            point->vx = (config->radius - config->thickness) * ccos(i << 8) / 4096;
            point->vy = (config->radius - config->thickness) * csin(i << 8) / 4096;
            point->vz = direction * config->near_z / config->duration;
        }
        for (i = 0; i < 17; point++, i++) {
            point->vx = (config->radius - config->thickness * 2) * ccos(i << 8) / 4096;
            point->vy = (config->radius - config->thickness * 2) * csin(i << 8) / 4096;
            point->vz = 0;
        }
        point = work->positions;
        for (i = 0; i < config->count; i++) {
            setVector(point, 0, 0, 0);
        }
        work->completed = 0;
        work->elapsed = -config->delay;
        work->started = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyGT4(&quad);
    SetPolyG4(&unused_quad);
    line.attribute = 0x50000000;
    if (work->elapsed >= 0 && !work->started) {
        work->started++;
        func_8005F7B0(30, config->count * config->stagger / 2);
    }
    point = work->positions;
    for (i = 0; i < config->count; point++, i++) {
        age = work->elapsed - config->stagger * i;
        if (age < 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            setVector(point, matrix.t[0], matrix.t[1], matrix.t[2]);
        }
        {
            s32 double_duration = config->duration * 2;
            return_age = age - config->duration -
                config->near_z * double_duration / (450 - config->near_z);
        }
        burst_age = return_age - config->duration;
        if (config->duration < age && return_age < 0) {
            point->vz = direction * config->near_z;
        }
        if (burst_age >= 0 && burst_age < config->duration) {
            s32 factor;
            setVector(&position, 0, -350, direction * 450);
            factor = (burst_age << 12) / config->duration;
            setVector(&rotation, (i * 3 % 7 - 3) * 192, (i * 4 % 7 - 3) * 192, 0);
            value = 3 * factor;
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&rotation, 0, 0, 0);
            RotTransPersN(work->ring, projected, depths, interpolation, flags, 34);
            for (j = 0; j < 16; j++) {
                value = j + 17;
                line.x0 = projected[j].vx;
                line.y0 = projected[j].vy;
                line.x1 = projected[value].vx;
                line.y1 = projected[value].vy;
                depth = AverageZ4(depths[j], depths[value], depths[j], depths[value]);
                flag = (flags[j] | flags[value]) & 0x20;
                red = config->line_red * (config->duration - burst_age) / config->duration;
                green = config->line_green * (config->duration - burst_age) / config->duration;
                blue = config->line_blue * (config->duration - burst_age) / config->duration;
                setRGB0(&line, 0, 0, 0);
                setRGB1(&line, red, green, blue);
                if (depth >= 0 && !flag) {
                    GsSortGLine(&line, ot, depth & 0xffff);
                }
            }
        }
        copyVector(&position, point);
        if (age >= 0 && age < config->duration) {
            value = (age << 12) / config->duration;
            point->vz += (-direction * config->near_z - point->vz) /
                (config->duration - age) * step;
        }
        if (return_age >= 0 && return_age < config->duration) {
            value = (4096 - config->minimum_scale) * (config->duration - return_age) /
                config->duration + config->minimum_scale;
            point->vx += -point->vx / (config->duration - return_age) * step;
            point->vy += (-350 - point->vy) / (config->duration - return_age) * step;
            point->vz += (direction * 450 - point->vz) / (config->duration - return_age) * step;
        }
        if (burst_age >= 0 && burst_age < config->duration) {
            value = (4096 - config->minimum_scale) * burst_age /
                config->duration + config->minimum_scale;
            point->vx += -point->vx / (config->duration - burst_age) * step;
            point->vy += (-350 - point->vy) / (config->duration - burst_age) * step;
            point->vz += (direction * (450 + config->extra_z) - point->vz) /
                (config->duration - burst_age) * step;
        }
        setVector(&scale, value, value, value);
        if ((age >= 0 && age < config->duration) ||
            (return_age >= 0 && return_age < config->duration) ||
            (burst_age >= 0 && burst_age < config->duration)) {
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, projected, depths, interpolation, flags, 51);
            GetDispEnv(&display);
            for (j = 0; j < 16; j++) {
                s32 adjacent;
                value = j + 17;
                inner = j + 34;
                quad.tpage = GetTPage(2, 1,
                    projected[value].vx + display.disp.x,
                    projected[value].vy + display.disp.y);
                quad.u0 = projected[value].vx % 64;
                quad.v0 = projected[value].vy % 256;
                adjacent = value + 1;
                quad.u1 = projected[adjacent].vx % 256;
                quad.v1 = projected[adjacent].vy % 256;
                quad.u2 = projected[inner].vx % 64;
                quad.v2 = projected[inner].vy % 256;
                quad.u3 = projected[j + 35].vx % 64;
                quad.v3 = projected[j + 35].vy % 256;
                setXY4(&quad, projected[j].vx, projected[j].vy,
                    projected[j + 1].vx, projected[j + 1].vy,
                    projected[value].vx, projected[value].vy,
                    projected[adjacent].vx, projected[adjacent].vy);
                depth = AverageZ4(depths[j], depths[j + 1], depths[value], depths[adjacent]);
                flag = (flags[j] | flags[j + 1] | flags[value] | flags[adjacent]) & 0x20;
                setRGB0(&quad, 255, 255, 255);
                setRGB1(&quad, 255, 255, 255);
                setRGB2(&quad, config->red, config->green, config->blue);
                setRGB3(&quad, config->red, config->green, config->blue);
                setRGB3(&unused_quad, 0, 0, 0);
                if (depth >= 0 && !flag) {
                    GsSortPoly(&quad, ot, depth & 0xffff);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    {
        s32 double_duration = config->duration * 2;
        return_age = work->elapsed - config->duration -
            config->near_z * double_duration / (450 - config->near_z);
    }
    burst_age = return_age - config->duration;
    if (burst_age < 0) {
        return 0;
    } else {
        if (burst_age >= config->duration + config->count * config->stagger) {
            if (work->completed) {
                return 2;
            } else {
                work->completed++;
                return 1;
            }
        } else {
            return 4;
        }
    }
}
