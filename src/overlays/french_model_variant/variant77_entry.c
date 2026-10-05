#include "../../types.h"
#include "variant77_entry.h"

#define CLAMP(value, lower, upper) \
    ((value) < (lower) ? (lower) : ((value) > (upper) ? (upper) : (value)))

const VECTOR D_8013BC18 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model77State *work;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    POLY_F3 triangle;
    POLY_FT4 quad;
    SVECTOR origin;
    SVECTOR vertices[4];
    MATRIX base;
    MATRIX matrix;
    PSXLONG interpolation;
    PSXLONG flag;
    s32 direction;
    Model77Config *config;
    SVECTOR *velocity;
    s32 i, time, a, b;
    s32 left, right, step;
    PSXLONG depth, first_depth;
    s32 left_y, right_y, end, x;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013BC18;
    work = (Model77State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013BC44[command];
        work->elapsed = 0;
        velocity = work->velocities;
        for (i = 0; i < 32; velocity++, i++) {
            a = rand() % 4096;
            velocity->vx = config->speed * ccos(a) / 4096;
            velocity->vy = config->speed * csin(a) / 4096;
            velocity->vz = direction * config->speed;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BC28[i]);
        }
        work->completed = 0;
        return 0;
    }
    config = work->config;
    SetPolyF3(&triangle);
    SetPolyFT4(&quad);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setRGB0(&quad, config->particle_r, config->particle_g, config->particle_b);
    SetSemiTrans(&quad, 1);
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    setVector(&origin, matrix.t[0], matrix.t[1], matrix.t[2]);
    if (work->elapsed < config->delay) {
        setVector(&work->position, origin.vx, origin.vy, origin.vz);
        work->elapsed += Model_GetFrameStep();
        PopMatrix();
        return 0;
    }
    if (work->elapsed < config->travel) {
        setVector(&vertices[0], work->position.vx, work->position.vy, work->position.vz);
        vertices[0].vx *= -1;
        vertices[0].vy *= -1;
        vertices[0].vz *= -1;
        vertices[0].vy -= 350;
        vertices[0].vz += direction * 450;
        i = (config->travel - work->elapsed) / 2 + 1;
        vertices[0].vx /= i;
        vertices[0].vy /= i;
        vertices[0].vz /= i;
        work->position.vx += vertices[0].vx;
        work->position.vy += vertices[0].vy;
        work->position.vz += vertices[0].vz;
    }
    GsSetLsMatrix(&base);
    first_depth = RotTransPers(&origin, (PSXLONG *)&triangle.x0,
                              &interpolation, &flag);
    depth = RotTransPers(&work->position, (PSXLONG *)&triangle.x1,
                        &interpolation, &flag);
    if (first_depth < depth) {
        depth = first_depth;
    }
    if (triangle.x0 < triangle.x1) {
        left = triangle.x0;
        right = triangle.x1;
        left_y = triangle.y0 << 12;
        right_y = triangle.y1 << 12;
        step = 16;
    } else {
        left = triangle.x1;
        right = triangle.x0;
        left_y = triangle.y1 << 12;
        right_y = triangle.y0 << 12;
        step = -16;
    }
    setRGB0(&triangle, config->line_r, config->line_g, config->line_b);
    if (work->elapsed >= config->travel) {
        if (triangle.x0 < triangle.x1) {
            a = left + (right - left) * (work->elapsed - config->travel) / config->duration;
            end = right;
        } else {
            a = left;
            end = right + (a - right) * (work->elapsed - config->travel) / config->duration;
        }
    } else {
        a = left;
        end = right;
    }
    a = CLAMP(a, -16, 336);
    end = CLAMP(end, -16, 336);
    for (x = a; x < end; x += 2) {
        s32 distance = x - left;

        a = csin((distance << 12) / (right - left));
        b = left_y + (right_y - left_y) * distance / (right - left)
            + config->amplitude * a * ccos(work->elapsed * 24) / 4096;
        i = config->width * csin((distance << 11) / (right - left));
        setXY3(&triangle, x, (b - i) / 4096, x + step, b / 4096, x, (b + i) / 4096);
        func_8005B260((u32 *)&triangle, func_80058F10(), (u16)depth, 3);
    }
    velocity = work->velocities;
    for (i = 0; i < 32; i++) {
        time = work->elapsed - (config->travel + i);
        if (time >= 0 && time < config->duration) {
            setVector(&position, work->position.vx, work->position.vy, work->position.vz);
            position.vx += velocity->vx * time / config->duration;
            position.vy += velocity->vy * time / config->duration;
            position.vz += velocity->vz * time / config->duration;
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&vertices[0], -config->size / 2, -config->size / 2, 0);
            setVector(&vertices[1], config->size / 2, -config->size / 2, 0);
            setVector(&vertices[2], -config->size / 2, config->size / 2, 0);
            setVector(&vertices[3], config->size / 2, config->size / 2, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                &interpolation, &flag);
            a = time * 8 / config->duration;
            setUV4(&quad, a % 4 * 32, a / 4 * 32, a % 4 * 32 + 31, a / 4 * 32,
                   a % 4 * 32, a / 4 * 32 + 31, a % 4 * 32 + 31, a / 4 * 32 + 31);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, func_80058F10(), (u16)depth);
            }
            velocity++;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->travel) {
        return 0;
    }
    if (work->elapsed < config->travel + config->duration + 32) {
        return 4;
    }
    {
        u8 result;

        if (work->completed != 0) {
            result = 2;
        } else {
            work->completed++;
            result = 1;
        }
        return result;
    }
}
