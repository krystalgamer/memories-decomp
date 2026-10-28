#include "../../types.h"
#include "variant108_entry.h"

/* Header 108's entry: gouraud lines stream from eight joints towards the
 * anchor joint, two ribbon trails follow joint segments, the trail tips
 * fall away as fading quads and textured sparks flash along each trail.
 * The return value reports the end (2), 4 while the trails run, 1 during
 * the fade or 0 before the trails start. */

static const VECTOR D_8013C51C = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_G4 ribbon;
    POLY_FT4 quad;
    GsGLINE line;
    SVECTOR head, tail;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG parameter, flag;
    Model108State *work;
    s32 side;
    GsOT *ot;
    Model108Config *config;
    s32 i, j;
    s32 time;
    s32 depth;
    s32 count;
    s32 frame;
    SVECTOR *point, *other;
    u8 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C51C;
    work = (Model108State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    Model_GetFrameStep();
    if (command >= 0) {
        work->config = &D_8013C564[command];
        work->anchor.vx = 0;
        work->anchor.vy = 0;
        work->anchor.vz = 0;
        for (i = 0; i < 2; i++) {
            work->index[i] = 0;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C52C[i]);
        }
        work->elapsed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    SetPolyG4(&ribbon);
    SetPolyFT4(&quad);
    line.attribute = 0x50000000;
    line.r1 = 0;
    line.g1 = 0;
    line.b1 = 0;
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    GsSetLsMatrix(&base);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joints[0]), &matrix);
    head.vx = matrix.t[0];
    head.vy = matrix.t[1];
    head.vz = matrix.t[2];
    if (work->elapsed < config->delay) {
        work->anchor.vx = head.vx;
        work->anchor.vy = head.vy;
        work->anchor.vz = head.vz;
        point = work->trails[0].inner;
        for (i = 0; i < 2; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->trail_joints[i]), &matrix);
            for (j = 0; j < 4; j++, point++) {
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
            }
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->trail_joints[i] + 1), &matrix);
            for (j = 0; j < 4; j++, point++) {
                point->vx = matrix.t[0];
                point->vy = matrix.t[1];
                point->vz = matrix.t[2];
            }
        }
        work->elapsed += Model_GetFrameStep();
        PopMatrix();
        return 0;
    }
    time = work->elapsed - config->delay;
    if (time >= 0 && time < config->approach) {
        vertices[2].vx = (work->anchor.vx - head.vx) * 4;
        vertices[2].vy = (work->anchor.vy - head.vy) * 4;
        vertices[2].vz = (work->anchor.vz - head.vz) * 4;
        for (i = 0; i < 8; i++) {
            red = config->line_red * (config->approach - time) / config->approach;
            green = config->line_green * (config->approach - time) / config->approach;
            blue = config->line_blue * (config->approach - time) / config->approach;
            line.r0 = red;
            line.g0 = green;
            line.b0 = blue;
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joints[i]), &matrix);
            vertices[0].vx = matrix.t[0];
            vertices[0].vy = matrix.t[1];
            vertices[0].vz = matrix.t[2];
            vertices[1].vx = matrix.t[0];
            vertices[1].vy = matrix.t[1];
            vertices[1].vz = matrix.t[2];
            vertices[1].vx += vertices[2].vx;
            vertices[1].vy += vertices[2].vy;
            vertices[1].vz += vertices[2].vz;
            GsSetLsMatrix(&base);
            depth = RotTransPers(&vertices[0], (PSXLONG *)&line.x0, &parameter, &flag);
            depth += RotTransPers(&vertices[1], (PSXLONG *)&line.x1, &parameter, &flag);
            depth /= 2;
            if (depth >= 0 && flag >= 0) {
                GsSortGLine(&line, ot, depth);
            }
        }
        work->anchor.vx = head.vx;
        work->anchor.vy = head.vy;
        work->anchor.vz = head.vz;
    }
    for (i = 0; i < 2; i++) {
        time = work->elapsed - config->start[i];
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->trail_joints[i]), &matrix);
        head.vx = matrix.t[0];
        head.vy = matrix.t[1];
        head.vz = matrix.t[2];
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->trail_joints[i] + 1), &matrix);
        tail.vx = matrix.t[0];
        tail.vy = matrix.t[1];
        tail.vz = matrix.t[2];
        if (time >= 0 && time < config->duration) {
            GsSetLsMatrix(&base);
            depth = RotTransPers(&head, (PSXLONG *)&ribbon.x0, &parameter, &flag);
            depth += RotTransPers(&tail, (PSXLONG *)&ribbon.x1, &parameter, &flag);
            point = work->trails[i].inner;
            other = work->trails[i].outer;
            for (j = 0; j < 4; j++) {
                vertices[0].vx = point[(work->index[i] + j) % 4].vx;
                vertices[0].vy = point[(work->index[i] + j) % 4].vy;
                vertices[0].vz = point[(work->index[i] + j) % 4].vz;
                vertices[1].vx = other[(work->index[i] + j) % 4].vx;
                vertices[1].vy = other[(work->index[i] + j) % 4].vy;
                vertices[1].vz = other[(work->index[i] + j) % 4].vz;
                vertices[0].vx = vertices[1].vx + (vertices[0].vx - vertices[1].vx) * (4 - j) / 4;
                vertices[0].vy = vertices[1].vy + (vertices[0].vy - vertices[1].vy) * (4 - j) / 4;
                vertices[0].vz = vertices[1].vz + (vertices[0].vz - vertices[1].vz) * (4 - j) / 4;
                depth += RotTransPers(&vertices[0], (PSXLONG *)&ribbon.x2, &parameter, &flag);
                depth += RotTransPers(&other[(work->index[i] + j) % 4], (PSXLONG *)&ribbon.x3, &parameter, &flag);
                depth /= 4;
                red = config->trail_red * (4 - j) / 4;
                green = config->trail_green * (4 - j) / 4;
                blue = config->trail_blue * (4 - j) / 4;
                setRGB0(&ribbon, red >> 1, green >> 1, blue >> 1);
                setRGB1(&ribbon, red, green, blue);
                red = config->trail_red * (3 - j) / 4;
                green = config->trail_green * (3 - j) / 4;
                blue = config->trail_blue * (3 - j) / 4;
                setRGB2(&ribbon, red >> 1, green >> 1, blue >> 1);
                setRGB3(&ribbon, red, green, blue);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ribbon, ot, (u16)depth, 1);
                }
                depth /= 2;
                ribbon.x0 = ribbon.x2;
                ribbon.y0 = ribbon.y2;
                ribbon.x1 = ribbon.x3;
                ribbon.y1 = ribbon.y3;
            }
            work->index[i] = (work->index[i] + 3) % 4;
            point[work->index[i]].vx = head.vx;
            point[work->index[i]].vy = head.vy;
            point[work->index[i]].vz = head.vz;
            other[work->index[i]].vx = tail.vx;
            other[work->index[i]].vy = tail.vy;
            other[work->index[i]].vz = tail.vz;
        } else {
            point = work->trails[i].inner;
            other = work->trails[i].outer;
            for (j = 0; j < 4; j++, point++, other++) {
                point->vx = head.vx;
                point->vy = head.vy;
                point->vz = head.vz;
                other->vx = tail.vx;
                other->vy = tail.vy;
                other->vz = tail.vz;
            }
        }
    }
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 127, 63, 127);
    SetSemiTrans(&quad, 1);
    for (i = 0; i < 2; i++) {
        time = work->elapsed - config->start[i];
        if (time >= 0 && time < config->duration) {
            point = &work->trails[i].outer[work->index[i]];
            other = &work->trails[i].outer[(work->index[i] + 3) % 4];
            setVector(&work->quads[i][0], other->vx, other->vy, other->vz);
            setVector(&work->quads[i][1], point->vx, point->vy, point->vz);
            setVector(&work->quads[i][2], other->vx,
                other->vy - side * (point->vz - other->vz) / 2,
                other->vz + side * (point->vy - other->vy) / 2);
            setVector(&work->quads[i][3], point->vx,
                point->vy - side * (point->vz - other->vz) / 2,
                point->vz + side * (point->vy - other->vy) / 2);
        }
        time = work->elapsed - config->start[i] - config->duration;
        if (time >= 0 && time < config->fade) {
            point = work->quads[i];
            vertices[0].vx = 0;
            if (point[2].vz != point[0].vz) {
                vertices[0].vy = -config->rise * time / config->fade * (point[2].vy - point[0].vy)
                    / (point[2].vz - point[0].vz);
            } else {
                vertices[0].vy = 0;
            }
            vertices[0].vz = side * config->rise * time / config->fade;
            vertices[1].vx = vertices[0].vx;
            vertices[1].vy = vertices[0].vy;
            vertices[2].vx = vertices[0].vx;
            vertices[2].vy = vertices[0].vy;
            vertices[3].vx = vertices[0].vx;
            vertices[3].vy = vertices[0].vy;
            vertices[1].vz = vertices[0].vz;
            vertices[2].vz = vertices[0].vz;
            vertices[3].vz = vertices[0].vz;
            vertices[0].vx += point[0].vx;
            vertices[0].vy += point[0].vy;
            vertices[0].vz += point[0].vz;
            vertices[2].vx += point[1].vx;
            vertices[2].vy += point[1].vy;
            vertices[2].vz += point[1].vz;
            vertices[1].vx += point[2].vx;
            vertices[1].vy += point[2].vy;
            vertices[1].vz += point[2].vz;
            vertices[3].vx += point[3].vx;
            vertices[3].vy += point[3].vy;
            vertices[3].vz += point[3].vz;
            GsSetLsMatrix(&base);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &parameter, &flag);
            red = config->fade_red * (config->fade - time) / config->fade;
            green = config->fade_green * (config->fade - time) / config->fade;
            blue = config->fade_blue * (config->fade - time) / config->fade;
            setRGB0(&quad, red, green, blue);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, depth);
            }
        }
    }
    setVector(&vertices[0], -config->size / 2, -config->size / 2, 0);
    setVector(&vertices[1], config->size / 2, -config->size / 2, 0);
    setVector(&vertices[2], -config->size / 2, config->size / 2, 0);
    setVector(&vertices[3], config->size / 2, config->size / 2, 0);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setRGB0(&quad, config->spark_red, config->spark_green, config->spark_blue);
    for (i = 0; i < 2; i++) {
        time = work->elapsed - config->start[i];
        if (time >= 0 && time < config->duration + 16) {
            count = config->duration < 2 ? config->duration / 2 : 4;
            for (j = 0; j < count; j++) {
                point = &work->trails[i].inner[(work->index[i] + j) % 4];
                position.vx = point->vx;
                position.vy = point->vy;
                position.vz = side * 450;
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
                frame = j + time / 2;
                if ((u32)frame < 8 && flag >= 0 && depth >= 0) {
                    setUV4(&quad, frame * 32, 128, frame * 32 + 31, 128,
                        frame * 32, 159, frame * 32 + 31, 159);
                    GsSortPoly(&quad, ot, depth);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->start[0]) {
        return 0;
    }
    if (config->start[1] + config->duration + config->fade < work->elapsed) {
        return 2;
    }
    if (work->elapsed < config->start[1] + config->duration) {
        return 4;
    }
    return 1;
}
