#include "../../types.h"
#include "variant64_entry.h"

/* Header 64's entry: an outer and an inner ring of strips rise around a model
 * part with a fan of rays, the rings fading out in turn after the fade start;
 * the first frames burst the rays outwards and queue a camera shake. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model64State *work;
    Model64Config *config;
    MATRIX base, matrix;
    POLY_FT4 quad;
    SVECTOR position, rotation;
    VECTOR scale;
    DVECTOR projected[66];
    u16 depths[66];
    u16 interpolations[66];
    u16 flags[66];
    SVECTOR vertices[4];
    PSXLONG trans_flag, interpolation, flag;
    GsOT *ot;
    s32 direction;
    SVECTOR *p, *q;
    s32 i, a, r, s, t, z;
    u8 red, green, blue;

    work = (Model64State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C1D4[command];
        p = work->outer;
        q = work->inner;
        for (i = 0; i < 33; i++) {
            a = i << 7;
            p->vx = config->outer_bottom * ccos(a) / 4096;
            p->vy = 0;
            p->vz = config->outer_bottom * csin(a) / 4096;
            q->vx = config->inner_bottom * ccos(a) / 4096;
            q->vy = 0;
            q->vz = config->inner_bottom * csin(a) / 4096;
            p++;
            q++;
            p->vx = config->outer_top * ccos(a) / 4096;
            p->vy = -config->outer_height;
            p->vz = config->outer_top * csin(a) / 4096;
            q->vx = config->inner_top * ccos(a) / 4096;
            q->vy = 0;
            q->vz = config->inner_top * csin(a) / 4096;
            p++;
            q++;
        }
        p = work->rays;
        for (i = 0; i < 8; i++) {
            a = i << 9;
            p->vx = config->ray_radius * ccos(a) / 4096;
            p->vy = 0;
            p->vz = config->ray_radius * csin(a) / 4096;
            p++;
        }
        work->texture = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, D_8013C1B8);
        work->stage = 0;
        work->elapsed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < config->start) {
        PushMatrix();
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        work->center.vx = matrix.t[0];
        work->center.vy = 0;
        work->center.vz = matrix.t[2];
        r = direction * 450 - matrix.t[2];
        p = work->bursts;
        for (i = 0; i < 8; i++) {
            a = i * 64 - 256;
            p->vx = r * csin(a) / 4096;
            p->vy = 0;
            p->vz = r * ccos(a) / 4096;
            p++;
        }
        PopMatrix();
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    quad.tpage = work->texture >> 16;
    quad.clut = work->texture;
    setUV4(&quad, 0, 0, 127, 0, 0, 31, 127, 31);
    if (work->stage == 0) {
        work->stage++;
        Model_QueueCameraShake(100, 20);
    }
    if (work->elapsed < config->inner_end) {
        GsSetLsMatrix(&base);
        RotTrans(&work->center, (VECTOR *)matrix.t, &trans_flag);
        setVector(&rotation, 0, 0, 0);
        s = (work->elapsed - config->start << 14) / (config->inner_end - config->start);
        setVector(&scale, s, s, s);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->inner, projected, depths, interpolations, flags, 66);
        if (work->elapsed < config->fade_start) {
            setRGB0(&quad, config->inner_red, config->inner_green, config->inner_blue);
        } else {
            r = config->inner_end - work->elapsed;
            t = config->inner_end - config->fade_start;
            red = config->inner_red * r / t;
            green = config->inner_green * r / t;
            blue = config->inner_blue * r / t;
            setRGB0(&quad, red, green, blue);
        }
        SetSemiTrans(&quad, 1);
        for (i = 0; i < 32; i++) {
            quad.x0 = projected[i * 2].vx;
            quad.y0 = projected[i * 2].vy;
            quad.x1 = projected[(i + 1) * 2].vx;
            quad.y1 = projected[(i + 1) * 2].vy;
            quad.x2 = projected[i * 2 + 1].vx;
            quad.y2 = projected[i * 2 + 1].vy;
            quad.x3 = projected[i * 2 + 3].vx;
            quad.y3 = projected[i * 2 + 3].vy;
            z = AverageZ4(depths[i * 2], depths[i * 2 + 2], depths[i * 2 + 1], depths[i * 2 + 3]);
            flag = (flags[i * 2] | flags[i * 2 + 2] | flags[i * 2 + 1] | flags[i * 2 + 3]) & 0x20;
            if (z >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, z);
            }
        }
    }
    if (work->elapsed < config->outer_end) {
        GsSetLsMatrix(&base);
        RotTrans(&work->center, (VECTOR *)matrix.t, &trans_flag);
        setVector(&rotation, 0, 0, 0);
        s = (work->elapsed - config->start << 14) / (config->outer_end - config->start);
        setVector(&scale, s, s, s);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->outer, projected, depths, interpolations, flags, 66);
        if (work->elapsed < config->fade_start) {
            setRGB0(&quad, config->red, config->green, config->blue);
        } else {
            r = config->outer_end - work->elapsed;
            a = config->outer_end - config->fade_start;
            red = config->red * r / a;
            green = config->green * r / a;
            blue = config->blue * r / a;
            setRGB0(&quad, red, green, blue);
        }
        SetSemiTrans(&quad, 1);
        for (i = 0; i < 32; i++) {
            quad.x0 = projected[i * 2].vx;
            quad.y0 = projected[i * 2].vy;
            quad.x1 = projected[(i + 1) * 2].vx;
            quad.y1 = projected[(i + 1) * 2].vy;
            quad.x2 = projected[i * 2 + 1].vx;
            quad.y2 = projected[i * 2 + 1].vy;
            quad.x3 = projected[i * 2 + 3].vx;
            quad.y3 = projected[i * 2 + 3].vy;
            z = AverageZ4(depths[i * 2], depths[i * 2 + 2], depths[i * 2 + 1], depths[i * 2 + 3]);
            flag = (flags[i * 2] | flags[i * 2 + 2] | flags[i * 2 + 1] | flags[i * 2 + 3]) & 0x20;
            if (z >= 0 && flag == 0) {
                GsSortPoly(&quad, ot, z);
            }
        }
    }
    setVector(&vertices[0], -config->ray_width / 2, -config->ray_height, 0);
    setVector(&vertices[1], config->ray_width / 2, -config->ray_height, 0);
    setVector(&vertices[2], -config->ray_width / 2, 0, 0);
    setVector(&vertices[3], config->ray_width / 2, 0, 0);
    if (work->elapsed < config->start + config->fade) {
        setSemiTrans(&quad, 1);
        r = config->start + config->fade - work->elapsed;
        red = config->red * r / config->fade;
        green = config->green * r / config->fade;
        blue = config->blue * r / config->fade;
        setRGB0(&quad, red, green, blue);
        p = work->rays;
        for (i = 0; i < 8; i++) {
            GsSetLsMatrix(&base);
            position.vx = work->center.vx;
            position.vy = work->center.vy;
            position.vz = work->center.vz;
            position.vx += p->vx;
            position.vy += p->vy;
            position.vz += p->vz;
            setVector(&rotation, 0, 0, 0);
            s = (work->elapsed - config->start << 14) / (config->outer_end - config->start);
            setVector(&scale, s, s, s);
            RotTrans(&position, (VECTOR *)matrix.t, &trans_flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            z = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                            &interpolation, &flag);
            if (z >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, z);
            }
            p++;
        }
    }
    t = work->elapsed - config->fade_start;
    if (t >= 0 && t < config->fade) {
        if (work->stage == 1) {
            work->stage++;
            Model_QueueCameraShake(100, 20);
        }
        setSemiTrans(&quad, 1);
        r = config->fade - t;
        red = config->red * r / config->fade;
        green = config->green * r / config->fade;
        blue = config->blue * r / config->fade;
        setRGB0(&quad, red, green, blue);
        p = work->bursts;
        for (i = 0; i < 8; i++) {
            GsSetLsMatrix(&base);
            position.vx = work->center.vx;
            position.vy = work->center.vy;
            position.vz = work->center.vz;
            position.vx += p->vx;
            position.vy += p->vy;
            position.vz += p->vz;
            setVector(&rotation, 0, 0, 0);
            s = (t << 14) / config->fade;
            setVector(&scale, s, s, s);
            RotTrans(&position, (VECTOR *)matrix.t, &trans_flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            z = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                            &interpolation, &flag);
            if (z >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, z);
            }
            p++;
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->fade_start) {
        return 0;
    }
    if (work->elapsed > config->finish) {
        return 2;
    }
    return 1;
}
