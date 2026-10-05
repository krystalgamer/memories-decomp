#include "../../types.h"
#include "variant76_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    POLY_F3 triangle;
    MATRIX base;
    MATRIX matrix;
    VECTOR scale;
    SVECTOR origin;
    PSXLONG flag;
    PSXLONG projection;
    PSXLONG depth;
    s32 side;
    SVECTOR *velocity;
    SVECTOR *rotation;
    GsOT *ot;
    u8 red, green, blue;
    u8 step;
    Model76State *work = (Model76State *)context;
    Model76Config *config;
    SVECTOR *point;
    Model76Face *face;
    s32 i, j;
    s32 clip;

    side = Model_GetActiveSlotIndex() * 2 - 1;
    ot = func_80058F10();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = work->config = &D_8013BB98[command];
        point = work->positions;
        velocity = work->velocities;
        rotation = work->rotations;
        j = 0;
        for (i = 0; i < config->count; velocity++, i++) {
            setVector(point, 0, 0, 0);
            rotation->vx = rand() % (config->angle_x_range * 2) - config->angle_x_range + j;
            rotation->vy = rand() % (config->angle_y_range * 2) - config->angle_y_range + j;
            rotation->vz = rand() % 4096;
            point++;
            rotation++;
            setVector(velocity, 0, 0, 0);
            work->life[i] = 0;
        }
        setVector(&work->vertices[0], 0, 0, side * config->size);
        setVector(&work->vertices[1], 0, -config->size * 50 / 1732, 0);
        setVector(&work->vertices[2], -config->size / 20, config->size * 50 / 3464, 0);
        setVector(&work->vertices[3], config->size / 20, config->size * 50 / 3464, 0);
        work->faces[0].a = &work->vertices[0];
        work->faces[0].b = &work->vertices[1];
        work->faces[0].c = &work->vertices[3];
        work->faces[1].a = &work->vertices[0];
        work->faces[1].b = &work->vertices[2];
        work->faces[1].c = &work->vertices[1];
        work->faces[2].a = &work->vertices[0];
        work->faces[2].b = &work->vertices[3];
        work->faces[2].c = &work->vertices[2];
        work->faces[3].a = &work->vertices[1];
        work->faces[3].b = &work->vertices[2];
        work->faces[3].c = &work->vertices[3];
        for (i = 0; i < config->count; i++) {
            work->life[i] = 0;
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < config->delay) {
        PushMatrix();
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        setVector(&work->positions[0], matrix.t[0], matrix.t[1], matrix.t[2]);
        j = (side * 450 - work->positions[0].vz) / config->lifetime;
        work->velocities[0].vx = j * csin(work->rotations[0].vy) / 4096;
        work->velocities[0].vy = -j * csin(work->rotations[0].vx) / 4096;
        work->velocities[0].vz = j * ccos(work->rotations[0].vy) / 4096;
        work->life[0] = config->lifetime;
        PopMatrix();
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    SetPolyF3(&triangle);
    velocity = work->velocities;
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    setVector(&origin, matrix.t[0], matrix.t[1], matrix.t[2]);
    point = work->positions;
    rotation = work->rotations;
    for (i = 0; i < config->count; point++, velocity++, rotation++, i++) {
        if (work->elapsed <= config->delay + i * config->interval) {
            setVector(point, origin.vx, origin.vy, origin.vz);
            j = (side * 450 - origin.vz) / config->lifetime * 3 / 2;
            velocity->vx = j * csin(rotation->vy) / 4096;
            velocity->vy = -j * csin(rotation->vx) / 4096;
            velocity->vz = j * ccos(rotation->vy) / 4096;
            work->life[i] = config->lifetime;
        } else if (work->life[i] > 0) {
            GsSetLsMatrix(&base);
            setVector(&scale, 4096, 4096, 4096);
            RotTrans(point, (VECTOR *)matrix.t, &flag);
            RotMatrix(rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (j = 0; j < step; j++) {
                point->vx += velocity->vx;
                point->vy += velocity->vy;
                point->vz += velocity->vz;
                rotation->vz = (rotation->vz + 4096 / config->lifetime) % 4096;
            }
            if (work->life[i] > config->fade) {
                SetSemiTrans(&triangle, 0);
                red = config->color[0];
                green = config->color[1];
                blue = config->color[2];
            } else {
                SetSemiTrans(&triangle, 1);
                red = config->color[0] * work->life[i] / config->fade;
                green = config->color[1] * work->life[i] / config->fade;
                blue = config->color[2] * work->life[i] / config->fade;
            }
            for (j = 0; j < 4; j++) {
                face = (Model76Face *)((u8 *)work +
                    (j * sizeof(Model76Face) + (u32)&((Model76State *)0)->faces));
                clip = RotAverageNclip3(face->a, face->b, face->c,
                    (PSXLONG *)&triangle.x0, (PSXLONG *)&triangle.x1,
                    (PSXLONG *)&triangle.x2, &projection, &depth, &flag);
                setRGB0(&triangle, red * (4 - j) / 4, green * (4 - j) / 4, blue * (4 - j) / 4);
                if (clip > 0 && depth >= 0 && flag >= 0) {
                    if (work->life[i] > config->fade) {
                        GsSortPoly(&triangle, ot, (u16)depth);
                    } else {
                        func_8005B260((u32 *)&triangle, ot, (u16)depth, 1);
                    }
                }
            }
            work->life[i] -= step;
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (work->elapsed < config->delay + config->lifetime) {
        return 0;
    }
    if (work->elapsed >= config->delay + config->interval * config->count + config->lifetime) {
        if (work->completed != 0) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 4;
}
