#include "../../types.h"
#include "variant93_entry.h"

static const VECTOR D_8013E40C = {4096, 4096, 4096, 0};

/* Header 93's entry: a shaded dome of 137 points spins over the command's
 * part and fades, three lightning bolts reach from the slot towards a part
 * vertex, the slot ambient dims while a textured flash grows over that vertex
 * and a sixteen-ray star bursts from it, then eighty sparks rise in five
 * groups and a textured wave sweeps through twice. The camera is stepped
 * through seven presets as the command timeline passes each step time. */
s32 func_8013B004(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_FT3 tri;
    POLY_G3 bolt;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[5];
    DVECTOR screen[137];
    u16 depths[137], projection[137], flags[137];
    PSXLONG flag, interpolation;
    Model93State *work;
    s32 side;
    GsOT *ot;
    s32 time;
    u8 red, green, blue;
    Model93Config *config;
    SVECTOR *point;
    s32 i, j, shade, value, other, dx, dy, dz;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013E40C;
    work = (Model93State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013E454[command];
        point = work->dome;
        setVector(point, 0, 0, side * config->radius);
        point = &work->dome[1];
        for (i = 1; i < 5; i++) {
            for (j = 0; j < 17; j++) {
                point->vx = config->radius * csin(i * 256) / 4096 * ccos(j * 256) / 4096;
                point->vy = config->radius * csin(i * 256) / 4096 * csin(j * 256) / 4096;
                point->vz = side * config->radius * ccos(i * 256) / 4096;
                point++;
            }
        }
        for (i = 1; i < 5; i++) {
            for (j = 0; j < 17; j++) {
                point->vx = config->radius * (4 - i) / 4 * ccos(j * 256) / 4096;
                point->vy = config->radius * (4 - i) / 4 * csin(j * 256) / 4096;
                point->vz = -side * config->depth * i / 4;
                point++;
            }
        }
        point = work->bolts;
        for (i = 0; i < 6; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
            point->pad = 0;
            point++;
        }
        point = work->star;
        setVector(&point[0], 0, 0, 0);
        setVector(&point[1], 0, config->star_long, 0);
        setVector(&point[2], config->star_short, 0, 0);
        setVector(&point[3], 0, -config->star_long, 0);
        setVector(&point[4], -config->star_short, 0, 0);
        setVector(&point[5], 0, config->star_short, 0);
        setVector(&point[6], config->star_long, 0, 0);
        setVector(&point[7], 0, -config->star_short, 0);
        setVector(&point[8], -config->star_long, 0, 0);
        point = &work->star[1];
        for (i = 0; i < 8; i++) {
            setVector(&point[8], point->vx, point->vy, point->vz);
            point[8].vx /= 2;
            point[8].vy /= 2;
            point[8].vz /= 2;
            point++;
        }
        point = work->sparks.flat;
        for (i = 0; i < 16; i++) {
            value = side * config->spark_spread * i / 16;
            setVector(&point[0], 0, 0, value * 2);
            setVector(&point[16], -value / 2, 0, value / 2);
            setVector(&point[32], value / 2, 0, value / 2);
            setVector(&point[48], -value / 4, 0, -value / 4);
            setVector(&point[64], value / 4, 0, -value / 4);
            point++;
        }
        point = work->wave;
        for (i = 0; i < 9; i++) {
            point->vx = config->wave_radius * csin(i * 256) / 4096;
            point->vy = 0;
            point->vz = config->wave_length * (i - 4) / 8;
            point++;
            point->vx = (config->wave_radius + config->wave_width) * csin(i * 256) / 4096;
            point->vy = -config->wave_height;
            point->vz = config->wave_length * (i - 4) / 8;
            point++;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013E438[i]);
        }
        work->elapsed = -(s16)config->delay;
        setVector(&vertices[0], 0, 0, 0);
        Model_QueueCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 1, &vertices[0], (s16)((s16)config->delay / 3));
        work->camera_step = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    time = work->elapsed + (s16)config->delay;
    setVector(&vertices[0], 0, 0, 0);
    switch (work->camera_step) {
    case 0:
        if (time >= config->camera_time[0]) {
            Model_QueueCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 3, &vertices[0], config->dome_duration);
            work->camera_step++;
        }
        break;
    case 1:
        if (time >= config->camera_time[1]) {
            Model_RequestCameraMoveToSlots(0, Model_GetActiveSlotIndex(), Model_GetActiveSlotIndex(), 0);
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 2, &vertices[0]);
            work->camera_step++;
        }
        break;
    case 2:
        if (time >= config->camera_time[2]) {
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 3, &vertices[0]);
            work->camera_step++;
        }
        break;
    case 3:
        if (time >= config->camera_time[3]) {
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 2, &vertices[0]);
            work->camera_step++;
        }
        break;
    case 4:
        if (time >= config->camera_time[4]) {
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 3, &vertices[0]);
            work->camera_step++;
        }
        break;
    case 5:
        if (time >= config->camera_time[5]) {
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[1]);
            Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&vertices[2]);
            setVector(&vertices[0], 0, -120, (vertices[1].vz - vertices[2].vz) / 2);
            Model_QueueCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 4, &vertices[0], config->flash_duration);
            work->camera_step++;
        }
        break;
    case 6:
        if (time >= config->camera_time[6]) {
            setVector(&vertices[0], 0, 0, 0);
            Model_QueueCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 2, &vertices[0], 0);
            work->camera_step++;
        }
        break;
    }
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    GsSetLsMatrix(&base);
    SetPolyFT4(&quad);
    SetPolyFT3(&tri);
    SetPolyG3(&bolt);
    time = work->elapsed;
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    tri.tpage = work->texture[0] >> 16;
    tri.clut = work->texture[0];
    if (time >= 0 && time < config->dome_duration) {
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        rotation.vx = 0;
        rotation.vy = 0;
        setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);
        rotation.vz = time * 3 * 1024 / config->dome_duration;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->dome, screen, depths, projection, flags, 137);
        SetSemiTrans(&quad, 1);
        SetSemiTrans(&tri, 1);
        {
            s32 r = config->dome_red * (config->dome_duration - time) / config->dome_duration;
            s32 g = config->dome_green * (config->dome_duration - time) / config->dome_duration;
            s32 b = config->dome_blue * (config->dome_duration - time) / config->dome_duration;
            setRGB0(&tri, r, g, b);
        }
        for (i = 1; i < 17; i++) {
            setXY3(&tri, screen[0].vx, screen[0].vy, screen[i].vx, screen[i].vy, screen[i + 1].vx, screen[i + 1].vy);
            setUV3(&tri, (i - 1) * 16, 0, (i - 1) * 16, 31, i * 16 - 1, 31);
            depth = AverageZ3(depths[0], depths[i], depths[i + 1]);
            flag = (flags[0] | flags[i] | flags[i + 1]) & 0x20;
            if (depth >= 0 && !flag) {
                GsSortPoly(&tri, ot, (u16)depth);
            }
        }
        /* value and other index the current and next dome rings. */
        value = 1;
        other = 18;
        for (i = 0; i < 4; i++) {
            {
                s32 r = config->dome_red * (config->dome_duration - time) / config->dome_duration * (4 - i / 2) / 4;
                s32 g = config->dome_green * (config->dome_duration - time) / config->dome_duration * (4 - i / 2) / 4;
                s32 b = config->dome_blue * (config->dome_duration - time) / config->dome_duration * (4 - i / 2) / 4;
                setRGB0(&quad, r, g, b);
            }
            for (j = 0; j < 16; j++) {
                setXY4(&quad, screen[value].vx, screen[value].vy, screen[value + 1].vx, screen[value + 1].vy,
                    screen[other].vx, screen[other].vy, screen[other + 1].vx, screen[other + 1].vy);
                setUV4(&quad, j * 8, 32 - time * 4 % 32, j * 8 + 7, 32 - time * 4 % 32,
                    j * 8, 63 - time * 4 % 32, j * 8 + 7, 63 - time * 4 % 32);
                depth = AverageZ4(depths[value], depths[value + 1], depths[other], depths[other + 1]);
                flag = (flags[value] | flags[value + 1] | flags[other] | flags[other + 1]) & 0x20;
                if (depth >= 0 && !flag) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                value++;
                other++;
            }
            value++;
            other++;
        }
        for (i = 0; i < 3; i++) {
            red = config->dome_red * (config->dome_duration - time) / config->dome_duration * (3 - i) / 4;
            green = config->dome_green * (config->dome_duration - time) / config->dome_duration * (3 - i) / 4;
            blue = config->dome_blue * (config->dome_duration - time) / config->dome_duration * (3 - i) / 4;
            setRGB0(&quad, red, green, blue);
            for (j = 0; j < 16; j++) {
                setRGB0(&quad, red, green, blue);
                setXY4(&quad, screen[value].vx, screen[value].vy, screen[value + 1].vx, screen[value + 1].vy,
                    screen[other].vx, screen[other].vy, screen[other + 1].vx, screen[other + 1].vy);
                setUV4(&quad, j * 8, 32 - time * 4 % 32, j * 8 + 7, 32 - time * 4 % 32,
                    j * 8, 63 - time * 4 % 32, j * 8 + 7, 63 - time * 4 % 32);
                depth = AverageZ4(depths[value], depths[value + 1], depths[other], depths[other + 1]);
                flag = (flags[value] | flags[value + 1] | flags[other] | flags[other + 1]) & 0x20;
                if (depth >= 0 && !flag) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
                value++;
                other++;
            }
            value++;
            other++;
        }
    }
    SetSemiTrans(&bolt, 1);
    /* value picks the bolt's end-point pair; other is its growth time. */
    for (i = 0; i < 3; i++) {
        other = value = 0;
        time = -1;
        if (i == 0 && work->elapsed >= config->bolt_start[0] && work->elapsed < config->bolt_start[0] + config->bolt_duration) {
            time = work->elapsed - config->bolt_start[0];
            other = 10;
        }
        if (i == 1 && work->elapsed >= config->bolt_start[1] && work->elapsed < config->bolt_start[1] + config->bolt_duration) {
            time = work->elapsed - config->bolt_start[1];
            value = 1;
            other = 10;
        }
        if (i == 2 && work->elapsed >= config->bolt_start[2] && work->elapsed < config->bolt_start[2] + config->bolt_duration) {
            time = work->elapsed - config->bolt_start[2];
            value = 2;
            other = 8;
        }
        if (time < 0) {
            continue;
        }
        point = &work->bolts[value * 2];
        if (point->pad == 0) {
            GsSetLsMatrix(&base);
            Model_TransformPartVertex(Model_GetActiveSlotIndex(), config->bolt_part, 316, &scale);
            point->vx = scale.vx;
            point->vy = scale.vy;
            point->vz = scale.vz;
            point->pad = 1;
        }
        point++;
        if (time < other) {
            GsSetLsMatrix(&base);
            Model_TransformPartVertex(Model_GetActiveSlotIndex(), config->bolt_part, 316, &scale);
            point->vx = scale.vx;
            point->vy = scale.vy;
            point->vz = scale.vz;
            continue;
        }
        GsSetLsMatrix(&base);
        setVector(&vertices[0], point[-1].vx, point[-1].vy, point[-1].vz);
        setVector(&vertices[1], vertices[0].vx, vertices[0].vy, vertices[0].vz);
        setVector(&vertices[2], vertices[0].vx, vertices[0].vy, vertices[0].vz);
        setVector(&vertices[4], vertices[0].vx, vertices[0].vy, vertices[0].vz);
        setVector(&vertices[3], point->vx, point->vy, point->vz);
        dx = vertices[3].vx - vertices[0].vx;
        dy = vertices[3].vy - vertices[0].vy;
        dz = vertices[3].vz - vertices[0].vz;
        vertices[4].vx += dx / 2;
        vertices[4].vy += dy / 2;
        vertices[1].vz += dz / 2;
        vertices[2].vz += dz / 2;
        vertices[4].vz += dz / 2;
        vertices[1].vy += dy / 2 - dx / 10;
        vertices[2].vy += dy / 2 + dx / 10;
        vertices[1].vx += dx / 2 + dy / 10;
        vertices[2].vx += dx / 2 - dy / 10;
        RotTransPers(&vertices[4], (PSXLONG *)&bolt.x0, &interpolation, &flag);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
            &interpolation, &flag);
        shade = (config->bolt_duration - time) * 255 / config->bolt_duration;
        setRGB0(&bolt, shade, shade, shade);
        setRGB1(&bolt, 0, 0, 0);
        setRGB2(&bolt, 0, 0, 0);
        if (depth >= 0 && flag >= 0) {
            bolt.x1 = quad.x0;
            bolt.y1 = quad.y0;
            bolt.x2 = quad.x1;
            bolt.y2 = quad.y1;
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            bolt.x1 = quad.x3;
            bolt.y1 = quad.y3;
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            bolt.x2 = quad.x2;
            bolt.y2 = quad.y2;
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            bolt.x1 = quad.x0;
            bolt.y1 = quad.y0;
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            {
                s32 r = config->bolt_red * (config->bolt_duration - time) / config->bolt_duration;
                s32 g = config->bolt_green * (config->bolt_duration - time) / config->bolt_duration;
                s32 b = config->bolt_blue * (config->bolt_duration - time) / config->bolt_duration;
                bolt.r0 = r;
                bolt.x1 = bolt.x0 + (quad.x0 - bolt.x0) * 2;
                bolt.y1 = bolt.y0 + (quad.y0 - bolt.y0) * 2;
                bolt.x2 = bolt.x0 + (quad.x1 - bolt.x0) * 2;
                bolt.y2 = bolt.y0 + (quad.y1 - bolt.y0) * 2;
                bolt.g0 = g;
                bolt.b0 = b;
            }
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            bolt.x1 = bolt.x0 + (quad.x3 - bolt.x0) * 2;
            bolt.y1 = bolt.y0 + (quad.y3 - bolt.y0) * 2;
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            bolt.x2 = bolt.x0 + (quad.x2 - bolt.x0) * 2;
            bolt.y2 = bolt.y0 + (quad.y2 - bolt.y0) * 2;
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            bolt.x1 = bolt.x0 + (quad.x0 - bolt.x0) * 2;
            bolt.y1 = bolt.y0 + (quad.y0 - bolt.y0) * 2;
            Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
        }
    }
    if (work->elapsed >= config->fade_start && work->elapsed < config->fade_start + config->fade_in_duration) {
        time = work->elapsed - config->fade_start;
        value = 2048 - (time << 12) / config->fade_in_duration;
        Model_SetSlotAmbient(2, value, value, value);
    }
    if (work->elapsed >= config->flash_start && work->elapsed < config->spark_start) {
        Model_SetSlotAmbient(2, -2048, -2048, -2048);
    }
    time = work->elapsed - config->fade_start;
    if (time >= 0 && time < config->flash_start - config->fade_start + config->flash_duration) {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setUV4(&quad, 32, 160, 95, 160, 32, 223, 95, 223);
        setVector(&vertices[0], -config->flash_size, -config->flash_size, 0);
        setVector(&vertices[1], config->flash_size, -config->flash_size, 0);
        setVector(&vertices[2], -config->flash_size, config->flash_size, 0);
        setVector(&vertices[3], config->flash_size, config->flash_size, 0);
        GsSetLsMatrix(&base);
        Model_TransformPartVertex(Model_GetActiveSlotIndex(), config->bolt_part, 316, &scale);
        setVector(&position, scale.vx, scale.vy, scale.vz);
        if (time < config->flash_start - config->fade_start) {
            GsSetLsMatrix(&base);
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->bolt_part), &matrix);
            position.vx = matrix.t[0] + (position.vx - matrix.t[0]) * time / (config->flash_start - config->fade_start);
            position.vy = matrix.t[1] + (position.vy - matrix.t[1]) * time / (config->flash_start - config->fade_start);
            position.vz = matrix.t[2] + (position.vz - matrix.t[2]) * time / (config->flash_start - config->fade_start);
        }
        setVector(&rotation, 0, 0, 0);
        if (time < config->flash_start - config->fade_start) {
            value = (time << 12) / (config->flash_start - config->fade_start) + ((time % 4 / 2) << 9);
            setVector(&scale, value, value, value);
            setRGB0(&quad, config->flash_red, config->flash_green, config->flash_blue);
        } else {
            value = (time - config->flash_start + config->fade_start) * 10240 / config->flash_duration + 4096;
            setVector(&scale, value, value, value);
            {
                s32 fade = (config->flash_start - config->fade_start + config->flash_duration - time) * 2;
                s32 r = config->flash_red * fade / config->flash_duration;
                s32 g = config->flash_green * fade / config->flash_duration;
                s32 b = config->flash_blue * fade / config->flash_duration;
                setRGB0(&quad, r, g, b);
            }
        }
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
            &interpolation, &flag);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }
    if (work->elapsed >= config->flash_start && work->elapsed < config->flash_start + config->flash_duration) {
        time = work->elapsed - config->flash_start;
        Model_SetSlotPlaybackDirection(0, 0);
        Model_SetSlotPlaybackDirection(1, 0);
        GsSetLsMatrix(&base);
        Model_TransformPartVertex(Model_GetActiveSlotIndex(), config->bolt_part, 316, &scale);
        setVector(&position, scale.vx, scale.vy, scale.vz);
        value = (time << 12) / config->flash_duration;
        rotation.vx = 0;
        rotation.vy = 0;
        setVector(&scale, value, value, value);
        rotation.vz = value;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->star, screen, depths, projection, flags, 17);
        red = config->star_red * (config->flash_duration - time) / config->flash_duration;
        green = config->star_green * (config->flash_duration - time) / config->flash_duration;
        blue = config->star_blue * (config->flash_duration - time) / config->flash_duration;
        value = (config->flash_duration - time) * 255 / config->flash_duration;
        setRGB1(&bolt, 0, 0, 0);
        setRGB2(&bolt, 0, 0, 0);
        for (i = 1; i < 5; i++) {
            setRGB0(&bolt, red, green, blue);
            bolt.x0 = screen[0].vx;
            bolt.y0 = screen[0].vy;
            bolt.x1 = screen[i].vx;
            bolt.y1 = screen[i].vy;
            bolt.x2 = screen[i % 4 + 1].vx;
            bolt.y2 = screen[i % 4 + 1].vy;
            depth = AverageZ3(depths[0], depths[i], depths[i % 4 + 2]);
            flag = (flags[0] | flags[i] | flags[i % 4 + 2]) & 0x20;
            if (depth >= 0 && !flag) {
                Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            }
            bolt.x0 = screen[0].vx;
            bolt.y0 = screen[0].vy;
            bolt.x1 = screen[i + 4].vx;
            bolt.y1 = screen[i + 4].vy;
            bolt.x2 = screen[i % 4 + 5].vx;
            bolt.y2 = screen[i % 4 + 5].vy;
            depth = AverageZ3(depths[0], depths[i + 4], depths[i % 4 + 5]);
            flag = (flags[0] | flags[i + 4] | flags[i % 4 + 5]) & 0x20;
            if (depth >= 0 && !flag) {
                Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            }
            setRGB0(&bolt, value, value, value);
            bolt.x0 = screen[0].vx;
            bolt.y0 = screen[0].vy;
            bolt.x1 = screen[i + 8].vx;
            bolt.y1 = screen[i + 8].vy;
            bolt.x2 = screen[i % 4 + 9].vx;
            bolt.y2 = screen[i % 4 + 9].vy;
            depth = AverageZ3(depths[0], depths[i + 8], depths[i % 4 + 9]);
            flag = (flags[0] | flags[i + 8] | flags[i % 4 + 9]) & 0x20;
            if (depth >= 0 && !flag) {
                Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            }
            bolt.x0 = screen[0].vx;
            bolt.y0 = screen[0].vy;
            bolt.x1 = screen[i + 12].vx;
            bolt.y1 = screen[i + 12].vy;
            bolt.x2 = screen[i % 4 + 13].vx;
            bolt.y2 = screen[i % 4 + 13].vy;
            depth = AverageZ3(depths[0], depths[i + 12], depths[i % 4 + 13]);
            flag = (flags[0] | flags[i + 12] | flags[i % 4 + 13]) & 0x20;
            if (depth >= 0 && !flag) {
                Graphics_SubmitDrawModePacket((u32 *)&bolt, ot, (u16)depth, 1);
            }
        }
    }
    time = work->elapsed - config->flash_start - config->flash_duration;
    if (time >= 0 && time < config->fade_out_duration) {
        Model_SetSlotPlaybackDirection(0, 1);
        Model_SetSlotPlaybackDirection(1, 1);
        value = (time << 12) / config->fade_out_duration - 2048;
        Model_SetSlotAmbient(2, value, value, value);
    }
    if (work->elapsed >= config->spark_start && work->elapsed <= config->spark_start + config->fade_out_duration + 16) {
        setVector(&vertices[0], -config->spark_size / 4, -config->spark_size, 0);
        setVector(&vertices[1], config->spark_size / 4, -config->spark_size, 0);
        setVector(&vertices[2], -config->spark_size / 4, 0, 0);
        setVector(&vertices[3], config->spark_size / 4, 0, 0);
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setUV4(&quad, 0, 160, 31, 160, 0, 255, 31, 255);
        SetSemiTrans(&quad, 1);
        point = work->sparks.flat;
        for (i = 0; i < 80; i++, point++) {
            time = work->elapsed - config->spark_start - i % 16;
            if (time >= 0 && time < config->fade_out_duration) {
                {
                    s32 r = config->spark_red * (config->fade_out_duration - time) / config->fade_out_duration;
                    s32 g = config->spark_green * (config->fade_out_duration - time) / config->fade_out_duration;
                    s32 b = config->spark_blue * (config->fade_out_duration - time) / config->fade_out_duration;
                    setVector(&position, 0, 0, side * 450);
                    setRGB0(&quad, r, g, b);
                }
                position.vx += point->vx;
                position.vy += point->vy;
                position.vz += point->vz;
                setVector(&rotation, 0, 0, 0);
                value = ((config->fade_out_duration - time) << 12) / config->fade_out_duration;
                switch (i / 16) {
                case 1:
                    value /= 2;
                    break;
                case 2:
                    value /= 2;
                    break;
                case 3:
                    value /= 4;
                    break;
                case 4:
                    value /= 4;
                    break;
                }
                setVector(&scale, value, value, value);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotTransPers4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    if (work->elapsed >= config->wave_start && work->elapsed < config->wave_start + config->wave_period * 2) {
        time = (work->elapsed - config->wave_start) % config->wave_period;
        {
            s32 r = config->wave_red * (config->wave_period - time) / config->wave_period;
            s32 g = config->wave_green * (config->wave_period - time) / config->wave_period;
            s32 b = config->wave_blue * (config->wave_period - time) / config->wave_period;
            setRGB0(&quad, r, g, b);
        }
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        setSemiTrans(&quad, 1);
        GsSetLsMatrix(&base);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->bolt_part), &matrix);
        position.vx = config->wave_shift * time / config->wave_period;
        position.vy = config->wave_height / 2 - 350;
        position.vz = side * (config->wave_length / 2 + 450);
        setVector(&scale, 4096, 4096, 4096);
        scale.vy = (time << 12) / config->wave_period;
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->wave, screen, depths, projection, flags, 18);
        for (i = 0; i < 8; i++) {
            setXY4(&quad, screen[i * 2 + 1].vx, screen[i * 2 + 1].vy, screen[i * 2 + 3].vx, screen[i * 2 + 3].vy,
                screen[i * 2].vx, screen[i * 2].vy, screen[(i + 1) * 2].vx, screen[(i + 1) * 2].vy);
            setUV4(&quad, i * 16, 0, i * 16 + 15, 0, i * 16, 31, i * 16 + 15, 31);
            depth = AverageZ4(depths[i * 2], depths[i * 2 + 2], depths[i * 2 + 1], depths[i * 2 + 3]);
            flag = (flags[i * 2] | flags[i * 2 + 1] | flags[i * 2 + 2] | flags[i * 2 + 3]) & 0x20;
            if (depth >= 0 && !flag) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        position.vx = -config->wave_shift * time / config->wave_period;
        rotation.vy = 2048;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotTransPersN(work->wave, screen, depths, projection, flags, 18);
        for (i = 0; i < 8; i++) {
            setXY4(&quad, screen[i * 2 + 1].vx, screen[i * 2 + 1].vy, screen[i * 2 + 3].vx, screen[i * 2 + 3].vy,
                screen[i * 2].vx, screen[i * 2].vy, screen[(i + 1) * 2].vx, screen[(i + 1) * 2].vy);
            setUV4(&quad, i * 16, 0, i * 16 + 15, 0, i * 16, 31, i * 16 + 15, 31);
            depth = AverageZ4(depths[i * 2], depths[i * 2 + 2], depths[i * 2 + 1], depths[i * 2 + 3]);
            flag = (flags[i * 2] | flags[i * 2 + 1] | flags[i * 2 + 2] | flags[i * 2 + 3]) & 0x20;
            if (depth >= 0 && !flag) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->bolt_start[0]) {
        return 0;
    }
    if (work->elapsed > config->wave_start + config->wave_period * 3) {
        Model_SetSlotAmbient(2, 2048, 2048, 2048);
        return 2;
    }
    if (work->elapsed > config->fade_start && work->elapsed < config->spark_start) {
        return 0;
    }
    if (work->elapsed > config->spark_start) {
        return 1;
    }
    return 4;
}
