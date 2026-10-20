#include "../../types.h"
#include "variant782_entry.h"

static const VECTOR D_8013C4B0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model782State *work;
    s32 slot, direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    u16 projection[64], flags[64];
    PSXLONG flag, interpolation;
    s32 step;
    GsOT *ot;
    SVECTOR *point, *other;
    DVECTOR *screen;
    s32 i, j, k, z;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C4B0;
    work = (Model782State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C54C[command];
        point = work->debris_positions;
        Model_CopySlotU16Values(slot ^ 1, (u16 *)&work->origin);
        other = work->debris_velocities;
        for (i = 0; i < work->config->debris_count; point++, other++, i++) {
            k = rand() % 4096 - rand() % 4096;
            j = -(rand() % 4096);
            z = rand() % 4096 - rand() % 4096;
            point->vx = work->config->debris_spread * k / 4096;
            point->vy = (work->config->debris_spread / 2) * j / 4096;
            point->vz = direction * work->config->debris_spread * z / 4096;
            other->vx = work->config->debris_speed * k / 4096;
            other->vy = work->config->debris_speed * j / 4096;
            other->vz = direction * work->config->debris_speed * z / 4096;
            point->vx += work->origin.vx;
            point->vz += work->origin.vz;
        }
        point = work->dot_positions;
        other = work->dot_velocities;
        screen = work->screen;
        for (i = 0; i < work->config->dot_count; point++, other++, screen++, i++) {
            setVector(point, 0, 0, 0);
            k = rand() % 4096;
            j = rand() % 4096;
            other->vx = work->config->dot_speed * ccos(k) / 4096 * ccos(j) / 4096;
            other->vy = work->config->dot_speed * csin(k) / 4096 * ccos(j) / 4096;
            other->vz = work->config->dot_speed * csin(k) / 4096 * ccos(j) / 4096;
            screen->vx = screen->vy = 0;
            work->depths[i] = 0;
        }
        work->level = 2048;
        work->animation = 0;
        work->elapsed = 0;
        work->phase = 0;
        work->growth_age = 0;
        work->shade = 128;
        for (i = 0; i < 5; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013C4C0[i]);
        }
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step * 64;
        } else {
            work->level = -512;
        }
    } else if (work->phase > 0) {
        if (work->level < 2048) {
            work->level += step * 64;
        } else {
            work->level = 2048;
            work->phase = 2;
        }
    }
    func_800595C8(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->initial_delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    box.w = box.h = 1;
    box.r = 128;
    box.g = 128;
    box.b = 128;
    point = work->dot_positions;
    screen = work->screen;
    setVector(&position, 0, -350, direction * 450);
    setVector(&scale, 4096, 4096, 4096);
    setVector(&rotation, 0, 0, 0);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    box.attribute = 0x40000000;
    flag = 0;
    for (i = 0; i < work->config->dot_count; point++, screen++, i++) {
        if (point->vy < 350) {
            box.x = screen->vx;
            box.y = screen->vy;
            if (work->depths[i]) {
                GsSortBoxFill(&box, ot, work->depths[i] / 4);
            }
        }
    }
    point = work->dot_positions;
    screen = work->screen;
    RotTransPersN(point, screen, work->depths, projection, flags, work->config->dot_count);
    box.attribute = 0;
    flag = 0;
    for (i = 0; i < work->config->dot_count; point++, screen++, i++) {
        if (point->vy < 350) {
            box.x = screen->vx;
            box.y = screen->vy;
            if (work->depths[i]) {
                GsSortBoxFill(&box, ot, work->depths[i] / 4);
            }
            flag = 1;
        }
    }
    gteMIMefunc(work->dot_positions, work->dot_velocities, work->config->dot_count, 2048);
    other = work->dot_velocities;
    for (i = 0; i < work->config->dot_count; other++, i++) {
        other->vy += (350 - other->vy) / work->config->dot_speed / 2;
    }
    SetPolyFT4(&quad);
    if (work->animation < 8) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setRGB0(&quad, 128, 128, 128);
        setVector(&vertices[0], -work->config->debris_half_size, -work->config->debris_half_size, 0);
        setVector(&vertices[1], work->config->debris_half_size, -work->config->debris_half_size, 0);
        setVector(&vertices[2], -work->config->debris_half_size, work->config->debris_half_size, 0);
        setVector(&vertices[3], work->config->debris_half_size, work->config->debris_half_size, 0);
        point = work->debris_positions;
        other = work->debris_velocities;
        for (i = 0; i < work->config->debris_count; i++) {
            point[i].vx += other[i].vx;
            point[i].vy += other[i].vy;
            point[i].vz += other[i].vz;
            GsSetLsMatrix(&base);
            RotTrans(&point[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
            setUV4(&quad, work->animation * 32, 96, work->animation * 32 + 31, 96,
                work->animation * 32, 127, work->animation * 32 + 31, 127);
            if (depth >= 0) {
                func_8005B260((u32 *)&quad, ot, (u16)depth, 3);
            }
        }
    }
    setRGB0(&quad, work->shade, work->shade, work->shade);
    quad.tpage = work->texture[3] >> 16;
    quad.clut = work->texture[3];
    setVector(&position, 0, -350, direction * 450);
    position.vy = 0;
    position.vx = work->config->x_offset;
    setVector(&scale, work->growth_age * work->config->growth_rate / 2 + 4096,
        work->growth_age * work->config->growth_rate + 4096,
        work->growth_age * work->config->growth_rate / 2 + 4096);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setVector(&vertices[0], -work->config->half_width, -work->config->height, 0);
    setVector(&vertices[1], work->config->half_width, -work->config->height, 0);
    setVector(&vertices[2], -work->config->half_width, 0, 0);
    setVector(&vertices[3], work->config->half_width, 0, 0);
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
    setUV4(&quad, 64, 0, 87, 0, 64, 95, 87, 95);
    if (depth >= 0) {
        func_8005B260((u32 *)&quad, ot, (u16)depth, 3);
    }
    quad.tpage = work->texture[4] >> 16;
    quad.clut = work->texture[4];
    setVector(&scale, work->growth_age * work->config->growth_rate + 8192,
        work->growth_age * work->config->growth_rate + 8192,
        work->growth_age * work->config->growth_rate + 8192);
    setVector(&vertices[0], 0, -work->config->side_height, direction * -work->config->half_width);
    setVector(&vertices[1], 0, -work->config->side_height, direction * work->config->side_end_z);
    setVector(&vertices[2], 0, 0, direction * -work->config->half_width);
    setVector(&vertices[3], 0, 0, direction * work->config->side_end_z);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    if (depth >= 0) {
        func_8005B260((u32 *)&quad, ot, (u16)depth, 3);
    }
    position.vx -= 2 * work->config->x_offset;
    quad.tpage = work->texture[3] >> 16;
    quad.clut = work->texture[3];
    setVector(&scale, work->growth_age * work->config->growth_rate / 2 + 4096,
        work->growth_age * work->config->growth_rate + 4096,
        work->growth_age * work->config->growth_rate / 2 + 4096);
    setVector(&vertices[0], -work->config->half_width, -work->config->height, 0);
    setVector(&vertices[1], work->config->half_width, -work->config->height, 0);
    setVector(&vertices[2], -work->config->half_width, 0, 0);
    setVector(&vertices[3], work->config->half_width, 0, 0);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
    setUV4(&quad, 64, 0, 87, 0, 64, 95, 87, 95);
    if (depth >= 0) {
        func_8005B260((u32 *)&quad, ot, (u16)depth, 3);
    }
    quad.tpage = work->texture[4] >> 16;
    quad.clut = work->texture[4];
    setVector(&scale, work->growth_age * work->config->growth_rate + 8192,
        work->growth_age * work->config->growth_rate + 8192,
        work->growth_age * work->config->growth_rate + 8192);
    setVector(&vertices[0], 0, -work->config->side_height, direction * -work->config->half_width);
    setVector(&vertices[1], 0, -work->config->side_height, direction * work->config->side_end_z);
    setVector(&vertices[2], 0, 0, direction * -work->config->half_width);
    setVector(&vertices[3], 0, 0, direction * work->config->side_end_z);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    if (depth >= 0) {
        func_8005B260((u32 *)&quad, ot, (u16)depth, 3);
    }
    work->growth_age++;
    PopMatrix();
    if (work->elapsed < work->config->burst_end) {
        func_8005F7B0(32, 8);
        work->elapsed += step;
        return 1;
    }
    work->animation++;
    if (work->shade >= 16) {
        work->shade -= step * 6;
    } else {
        work->shade = 0;
    }
    work->phase = 1;
    if (work->elapsed < work->config->duration) {
        work->elapsed += step;
        return 0;
    }
    return 2;
}
