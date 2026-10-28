#include "../../types.h"
#include "variant106_entry.h"

/* Header 106's entry: a fan of triangles twists open over the slot, then a
 * banded dome rises in rings, spins and walks to the opponent while sprites
 * bob around its rim and fade out, and a flash flares beneath it. */

/* Places target over the joint, sliding it across to the opponent's slot once
 * the dome has grown; vertices[0] receives the opponent's position. */
#define MODEL106_PLACE(target)                                                              \
    if (time < config->grow_end - config->dome_delay) {                                    \
        target.vx = matrix.t[0];                                                           \
        target.vy = 0;                                                                     \
        target.vz = matrix.t[2];                                                           \
    } else {                                                                               \
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);       \
        if (time < config->grow_end - config->dome_delay + config->move_duration) {        \
            target.vx = matrix.t[0] + (vertices[0].vx - matrix.t[0]) *                     \
                (time - config->grow_end + config->dome_delay) / config->move_duration;    \
            target.vy = 0;                                                                 \
            target.vz = matrix.t[2] + (vertices[0].vz - matrix.t[2]) *                     \
                (time - config->grow_end + config->dome_delay) / config->move_duration;    \
        } else {                                                                           \
            target.vx = vertices[0].vx;                                                    \
            target.vy = 0;                                                                 \
            target.vz = vertices[0].vz;                                                    \
        }                                                                                  \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model106State *work;
    MATRIX base, matrix;
    POLY_GT4 band;
    POLY_FT4 quad;
    POLY_G3 fan;
    DVECTOR screen[32];
    u16 depths[32], projection[32], flags[32];
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    SVECTOR center;
    PSXLONG flag, interpolation;
    Model106Config *config;
    GsOT *ot;
    s32 time;
    SVECTOR *point;
    SVECTOR *ring;
    s32 i, j, k, n, a, b, c, value;
    u8 red, green, blue;
    PSXLONG depth;

    work = (Model106State *)context;
    Model_GetActiveSlotIndex();
    if (command >= 0) {
        config = work->config = &D_8013D1C8[command];
        point = work->dome;
        for (j = 0; j < 8; j++) {
            for (i = 0; i < 16; i++) {
                point->vx = (config->outer_radius - (config->outer_radius - config->inner_radius) *
                    ccos(j << 7) / 4096) * ccos(i << 8) / 4096;
                point->vy = -config->height * j / 8;
                point->vz = (config->outer_radius - (config->outer_radius - config->inner_radius) *
                    ccos(j << 7) / 4096) * csin(i << 8) / 4096;
                point++;
            }
        }
        for (i = 0; i < 4; i++) {
            work->colours[i].r = work->colours[7 - i].r = work->colours[8 + i].r = work->colours[15 - i].r =
                (config->base_red - config->rim_red) * i / 4;
            work->colours[i].g = work->colours[7 - i].g = work->colours[8 + i].g = work->colours[15 - i].g =
                (config->base_green - config->rim_green) * i / 4;
            work->colours[i].b = work->colours[7 - i].b = work->colours[8 + i].b = work->colours[15 - i].b =
                (config->base_blue - config->rim_blue) * i / 4;
        }
        point = work->ring;
        for (i = 0; i < 8; i++) {
            point->vx = config->ring_radius * i / 8 * ccos(i << 9) / 4096;
            point->vy = 0;
            point->vz = config->ring_radius * i / 8 * csin(i << 9) / 4096;
            point++;
        }
        point = work->fan;
        setVector(&point[0], 0, 0, 0);
        setVector(&point[1], 0, config->fan_long, 0);
        setVector(&point[2], config->fan_short, 0, 0);
        setVector(&point[3], 0, -config->fan_long, 0);
        setVector(&point[4], -config->fan_short, 0, 0);
        setVector(&point[5], 0, config->fan_short, 0);
        setVector(&point[6], config->fan_long, 0, 0);
        setVector(&point[7], 0, -config->fan_short, 0);
        setVector(&point[8], -config->fan_long, 0, 0);
        point = &work->fan[1];
        for (i = 0; i < 8; i++) {
            setVector(&point[8], point->vx, point->vy, point->vz);
            point[8].vx /= 2;
            point[8].vy /= 2;
            point[8].vz /= 2;
            point++;
        }
        for (i = 0; i < 16; i++) {
            work->frame[i] = rand() % 8;
        }
        work->elapsed = -config->delay;
        work->completed = 0;
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D174[i]);
        }
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    SetPolyGT4(&band);
    SetPolyFT4(&quad);
    SetPolyG3(&fan);
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    time = work->elapsed;
    if (time >= 0 && time < config->fan_duration) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->fan_joint), &matrix);
        position.vx = matrix.t[0];
        position.vy = matrix.t[1];
        position.vz = matrix.t[2];
        a = (time << 12) / config->fan_duration;
        setVector(&rotation, 0, 0, a / 3);
        setVector(&scale, a, a, a);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->fan, screen, depths, projection, flags, 17);
        red = config->fan_red * (config->fan_duration - time) / config->fan_duration;
        green = config->fan_green * (config->fan_duration - time) / config->fan_duration;
        blue = config->fan_blue * (config->fan_duration - time) / config->fan_duration;
        a = (config->fan_duration - time) * 255 / config->fan_duration;
        setRGB1(&fan, 0, 0, 0);
        setRGB2(&fan, 0, 0, 0);
        for (i = 1; i < 5; i++) {
            setRGB0(&fan, red, green, blue);
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i].vx, screen[i].vy,
                screen[i % 4 + 1].vx, screen[i % 4 + 1].vy);
            depth = AverageZ3(depths[0], depths[i], depths[i % 4 + 1]);
            flag = (flags[0] | flags[i] | flags[i % 4 + 1]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 4].vx, screen[i + 4].vy,
                screen[i % 4 + 5].vx, screen[i % 4 + 5].vy);
            depth = AverageZ3(depths[0], depths[i + 4], depths[i % 4 + 5]);
            flag = (flags[0] | flags[i + 4] | flags[i % 4 + 5]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
            setRGB0(&fan, a, a, a);
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 8].vx, screen[i + 8].vy,
                screen[i % 4 + 9].vx, screen[i % 4 + 9].vy);
            depth = AverageZ3(depths[0], depths[i + 8], depths[i % 4 + 9]);
            flag = (flags[0] | flags[i + 8] | flags[i % 4 + 9]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
            setXY3(&fan, screen[0].vx, screen[0].vy, screen[i + 12].vx, screen[i + 12].vy,
                screen[i % 4 + 13].vx, screen[i % 4 + 13].vy);
            depth = AverageZ3(depths[0], depths[i + 12], depths[i % 4 + 13]);
            flag = (flags[0] | flags[i + 12] | flags[i % 4 + 13]) & 0x20;
            if (depth >= 0 && flag == 0) {
                Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1);
            }
        }
    }
    time = work->elapsed - config->dome_delay;
    if (time >= 0) {
        band.tpage = work->texture[0] >> 16;
        band.clut = work->texture[0];
        point = work->dome;
        ring = work->ring;
        setVector(&rotation, 0, 4096 - time * config->spin_speed % 4096, 0);
        setVector(&scale, 4096, 4096, 4096);
        if (time < config->grow_end - config->dome_delay) {
            scale.vx = scale.vy = scale.vz = (time << 12) / (config->grow_end - config->dome_delay);
        } else if (config->fade_start - config->dome_delay < time) {
            scale.vx = scale.vz = ((time - config->fade_start + config->dome_delay) << 13) /
                (config->fade_end - config->fade_start) + 4096;
        }
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joint), &matrix);
        MODEL106_PLACE(position)
        position.vx += ring->vx;
        position.vy += ring->vy;
        position.vz += ring->vz;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(point, &screen[16], &depths[16], &projection[16], &flags[16], 16);
        point += 16;
        for (i = 1; i < 8; i++) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joint), &matrix);
            MODEL106_PLACE(position)
            position.vx += ring->vx;
            position.vy += ring->vy;
            position.vz += ring->vz;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(point, screen, depths, projection, flags, 16);
            SetSemiTrans(&band, 1);
            for (j = 0; j < 16; j++) {
                /* a, b and c index the two rings first and the colour columns
                 * afterwards; retail keeps j + 1 in its own register. */
                a = j + 16;
                n = j + 1;
                b = n % 16;
                c = n % 16 + 16;
                setXY4(&band, screen[a].vx, screen[a].vy, screen[c].vx, screen[c].vy,
                    screen[j].vx, screen[j].vy, screen[b].vx, screen[b].vy);
                depth = AverageZ4(depths[a], depths[c], depths[j], depths[b]);
                flag = (flags[a] | flags[c] | flags[j] | flags[b]) & 0x20;
                a = (i - 1 + j) % 16;
                b = (i + j) % 16;
                c = (i + n) % 16;
                setRGB0(&band, work->colours[a].r, work->colours[a].g, work->colours[a].b);
                setRGB1(&band, work->colours[b].r, work->colours[b].g, work->colours[b].b);
                setRGB2(&band, work->colours[b].r, work->colours[b].g, work->colours[b].b);
                setRGB3(&band, work->colours[c].r, work->colours[c].g, work->colours[c].b);
                setUV4(&band, a * 16 % 128, (i - 1) * 16 % 128, b * 16 % 128, (i - 1) * 16 % 128,
                    b * 16 % 128, i * 16 % 128, c * 16 % 128, i * 16 % 128);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&band, ot, (u16)depth);
                }
            }
            for (j = 0; j < 16; j++) {
                screen[j + 16] = screen[j];
            }
            point += 16;
            ring++;
        }
        point = work->ring;
        for (i = 0; i < 8; i++) {
            point->vx = config->ring_radius * i / 8 *
                ccos((i << 9) + config->ring_speed * work->elapsed % 4096) / 4096;
            point->vy = 0;
            point->vz = config->ring_radius * i / 8 *
                csin((i << 9) + config->ring_speed * work->elapsed % 4096) / 4096;
            point++;
        }
        if (config->fade_start - config->dome_delay < time && time < config->fade_end - config->dome_delay) {
            for (i = 0; i < 16; i++) {
                work->colours[i].r -= work->colours[i].r / (config->fade_end - time - config->dome_delay) * Model_GetFrameStep();
                work->colours[i].g -= work->colours[i].g / (config->fade_end - time - config->dome_delay) * Model_GetFrameStep();
                work->colours[i].b -= work->colours[i].b / (config->fade_end - time - config->dome_delay) * Model_GetFrameStep();
            }
        }
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joint), &matrix);
        point = work->dome;
        MODEL106_PLACE(center)
        setVector(&vertices[0], -config->sprite_size / 2, -config->sprite_size / 2, 0);
        setVector(&vertices[1], config->sprite_size / 2, -config->sprite_size / 2, 0);
        setVector(&vertices[2], -config->sprite_size / 2, config->sprite_size / 2, 0);
        setVector(&vertices[3], config->sprite_size / 2, config->sprite_size / 2, 0);
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setSemiTrans(&quad, 1);
        quad.tpage = work->texture[1] >> 16;
        quad.clut = work->texture[1];
        for (i = 0; i < 16; i++) {
            if (time <= config->fade_start - config->dome_delay || work->frame[i] != 0) {
                position.vx = center.vx;
                position.vy = center.vy;
                position.vz = center.vz;
                position.vx += work->ring[0].vx;
                position.vy += work->ring[0].vy;
                position.vz += work->ring[0].vz;
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                position.vy -= config->sprite_size * (7 - work->frame[i]) / 24;
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                    (PSXLONG *)&quad.x3, &interpolation, &flag);
                setUV4(&quad, work->frame[i] << 5, 128, (work->frame[i] << 5) + 31, 128,
                    work->frame[i] << 5, 159, (work->frame[i] << 5) + 31, 159);
                k = 8 - work->frame[i];
                red = config->sprite_red * k / 8;
                green = config->sprite_green * k / 8;
                blue = config->sprite_blue * k / 8;
                setRGB0(&quad, red, green, blue);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                work->frame[i] = (work->frame[i] + 15) % 16;
            }
            point++;
        }
    }
    time = work->elapsed - config->flash_delay;
    if (time >= 0 && time < config->grow_end) {
        setVector(&vertices[0], -config->flash_size, 0, -config->flash_size);
        setVector(&vertices[1], config->flash_size, 0, -config->flash_size);
        setVector(&vertices[2], -config->flash_size, 0, config->flash_size);
        setVector(&vertices[3], config->flash_size, 0, config->flash_size);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->joint), &matrix);
        position.vx = matrix.t[0];
        position.vy = 0;
        position.vz = matrix.t[2];
        /* a doubles as the fan scale/grey and this spin, matching retail's register. */
        value = (time << 7) / (config->dome_delay - config->flash_delay);
        a = value * value;
        setVector(&rotation, 0, a, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
            (PSXLONG *)&quad.x3, &interpolation, &flag);
        SetSemiTrans(&quad, 1);
        quad.tpage = work->texture[2] >> 16;
        quad.clut = work->texture[2];
        setUV4(&quad, 128, 0, 191, 0, 128, 63, 191, 63);
        if (time < config->dome_delay - config->flash_delay) {
            red = config->flash_red * time / (config->dome_delay - config->flash_delay);
            green = config->flash_green * time / (config->dome_delay - config->flash_delay);
            blue = config->flash_blue * time / (config->dome_delay - config->flash_delay);
        } else {
            red = config->flash_red * (config->grow_end - time) / (config->grow_end - config->dome_delay + config->flash_delay);
            green = config->flash_green * (config->grow_end - time) / (config->grow_end - config->dome_delay + config->flash_delay);
            blue = config->flash_blue * (config->grow_end - time) / (config->grow_end - config->dome_delay + config->flash_delay);
        }
        setRGB0(&quad, red, green, blue);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->grow_end + config->move_duration) {
        return 0;
    }
    if (config->fade_end < work->elapsed && work->completed != 0) {
        return 2;
    }
    if (config->fade_start < work->elapsed) {
        if (work->completed == 0) {
            work->completed = 1;
            return 1;
        }
        return 0;
    }
    return 4;
}
