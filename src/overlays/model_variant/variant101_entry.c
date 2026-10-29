#include "../../types.h"
#include "variant101_entry.h"

/* Header 101's entry: textured rings trail behind the configured joints and
 * fade out, then flashes and bursts of sparks spin open in front of the
 * model. The return value reports the end (2), 4 while the bursts run, 1 on
 * the first frame after them or 0 before the first burst starts. */

static const VECTOR D_8013C2B8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    GsBOXF box;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, parameter;
    Model101State *work;
    s32 side;
    GsOT *ot;
    Model101Config *config;
    s32 i, j;
    s32 time;
    s32 depth;
    s32 frame;
    SVECTOR *point, *other;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C2B8;
    work = (Model101State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C31C[command];
        point = work->points;
        for (i = 0; i < 128; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            point++;
        }
        point = work->sparks;
        for (i = 0; i < 64; i++, point++) {
            point->vx = rand() % config->spread * 2 - config->size;
            point->vy = rand() % config->spread * 2 - config->size;
            point->vz = rand() % config->spread * 2 - config->size;
        }
        point = work->rotations;
        for (i = 0; i < config->burst_count; i++) {
            point->vx = rand() % 4096;
            point->vy = rand() % 4096;
            point->vz = rand() % 4096;
            point++;
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C2C8[i]);
        }
        work->completed = 0;
        work->elapsed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < config->delay) {
        PushMatrix();
        point = work->points;
        for (i = 0; i < config->joint_count; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joints[i]), &matrix);
            for (j = 0; j < config->ring_count; j++, point++) {
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
            }
        }
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    PushMatrix();
    SetPolyFT4(&quad);
    box.attribute = 0x50000000;
    box.w = box.h = 1;
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->end - config->delay) {
        if (time < config->end - config->delay - config->ring_fade) {
            quad.r0 = config->ring_red;
            quad.g0 = config->ring_green;
            quad.b0 = config->ring_blue;
        } else {
            red = config->ring_red * (config->end - config->delay - time) / config->ring_fade;
            green = config->ring_green * (config->end - config->delay - time) / config->ring_fade;
            blue = config->ring_blue * (config->end - config->delay - time) / config->ring_fade;
            quad.r0 = red;
            quad.g0 = green;
            quad.b0 = blue;
        }
        SetSemiTrans(&quad, 1);
        for (i = 0; i < config->joint_count; i++) {
            frame = config->width + config->width * (config->joint_count - i) / config->joint_count;
            point = &work->points[i * config->ring_count];
            vertices[0].vx = 0;
            vertices[0].vz = 0;
            vertices[1].vx = 0;
            vertices[1].vz = 0;
            vertices[0].vy = -frame;
            vertices[1].vy = frame;
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joints[i]), &matrix);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            position.vx = matrix.t[0];
            position.vy = matrix.t[1];
            position.vz = matrix.t[2];
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotTransPers(&vertices[0], (PSXLONG *)&quad.x0, &parameter, &flag);
            depth += RotTransPers(&vertices[1], (PSXLONG *)&quad.x2, &parameter, &flag);
            depth /= 2;
            quad.clut = work->texture[0];
            for (j = 0; j < config->ring_count; j++) {
                position.vx = point[(work->index + j) % config->ring_count].vx;
                position.vy = point[(work->index + j) % config->ring_count].vy;
                position.vz = point[(work->index + j) % config->ring_count].vz;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth += RotTransPers(&vertices[0], (PSXLONG *)&quad.x1, &parameter, &flag);
                depth += RotTransPers(&vertices[1], (PSXLONG *)&quad.x3, &parameter, &flag);
                setUV4(&quad, j * 128 / config->ring_count + 32, 0,
                    j * 128 / config->ring_count + 31 + 128 / config->ring_count, 0,
                    j * 128 / config->ring_count + 32, 31,
                    j * 128 / config->ring_count + 31 + 128 / config->ring_count, 31);
                depth /= 3;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, depth);
                }
                quad.x0 = quad.x1;
                quad.y0 = quad.y1;
                quad.x2 = quad.x3;
                quad.y2 = quad.y3;
            }
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joints[i]), &matrix);
            point[(work->index + config->ring_count - 1) % config->ring_count].vx = matrix.t[0];
            point[(work->index + config->ring_count - 1) % config->ring_count].vy = matrix.t[1];
            point[(work->index + config->ring_count - 1) % config->ring_count].vz = matrix.t[2];
        }
        work->index = (work->index + config->ring_count - 1) % config->ring_count;
    }
    SetSemiTrans(&quad, 1);
    setVector(&vertices[0], -config->size / 2, -config->size / 2, 0);
    setVector(&vertices[1], config->size / 2, -config->size / 2, 0);
    setVector(&vertices[2], -config->size / 2, config->size / 2, 0);
    setVector(&vertices[3], config->size / 2, config->size / 2, 0);
    quad.clut = work->texture[2];
    for (i = 0; i < config->burst_count; i++) {
        time = work->elapsed - config->start[i];
        if (time >= 0 && time < config->flash) {
            setVector(&position, 0, -350, side * 450);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &flag);
            frame = time * 3 / config->flash * 64;
            setUV4(&quad, frame, 48, frame + 63, 48,
                frame, 111, frame + 63, 111);
            red = config->flash_red * (config->flash - time) / config->flash;
            green = config->flash_green * (config->flash - time) / config->flash;
            blue = config->flash_blue * (config->flash - time) / config->flash;
            setRGB0(&quad, red, green, blue);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
    }
    point = work->rotations;
    for (i = 0; i < config->burst_count; point++, i++) {
        time = work->elapsed - config->start[i];
        if (time >= 0 && time < config->burst) {
            setVector(&position, 0, -350, side * 450);
            rotation.vx = point->vx;
            rotation.vy = point->vy;
            rotation.vz = point->vz;
            frame = (time << 12) / config->burst;
            scale.vx = frame;
            scale.vy = frame;
            scale.vz = frame;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            box.r = config->spark_red * (config->burst - time) / config->burst;
            box.g = config->spark_green * (config->burst - time) / config->burst;
            box.b = config->spark_blue * (config->burst - time) / config->burst;
            for (j = 0, other = work->sparks; j < 64; j++, other++) {
                depth = RotTransPers(other, (PSXLONG *)&box.x, &parameter, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortBoxFill(&box, ot, depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    time = work->elapsed;
    if (time < config->start[0]) {
        return 0;
    }
    if (config->start[config->burst_count - 1] + config->burst < time) {
        if (work->completed) {
            return 2;
        }
        work->completed++;
        return 1;
    }
    return 4;
}
