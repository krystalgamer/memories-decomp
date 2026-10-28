#include "../../types.h"
#include "image5996_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Image5996State *state;
    Image5996Config *config;
    MATRIX light;
    MATRIX matrix;
    POLY_G3 triangle;
    POLY_FT4 quad;
    SVECTOR rotation;
    SVECTOR translation;
    VECTOR scale;
    SVECTOR v0, v1, v2;
    Image5996Projection projected;
    u16 depths[66];
    u16 interpolation[66];
    u16 flags[66];
    PSXLONG flag;
    PSXLONG p;
    GsOT *ot;
    s32 side;
    s32 i;
    s32 depth;
    s32 elapsed;
    s32 size;
    s32 red, green, blue;
    s32 endpoint_flag;
    SVECTOR *angle;
    SVECTOR *velocity;
    DVECTOR *first;
    DVECTOR *second;
    u16 *first_depth;
    u16 *second_depth;
    u16 *pair_flags;

    memset(&rotation, 0, 8);
    memset(&translation, 0, 8);
    scale = D_8013BF44;
    state = (Image5996State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = state->config = &D_8013BFA8[command];
        angle = state->rotations;
        velocity = state->velocities;
        for (i = 0; i < config->triangle_count; angle++, velocity++, i++) {
            setVector(angle, rand() % 4096,
                      rand() % 4096, rand() % 4096);
            setVector(velocity, rand() % 32,
                      rand() % 32, rand() % 32);
            velocity->vz = 32 - velocity->vx;
        }
        angle = state->rings;
        for (i = 0; i <= 32; angle++, i++) {
            setVector(angle,
                      config->outer_radius * rsin(i * 128) / 4096,
                      config->outer_radius * rcos(i * 128) / 4096,
                      -side * config->inner_radius);
        }
        for (i = 0; i <= 32; angle++, i++) {
            setVector(angle,
                      config->inner_radius * rsin(i * 128) / 4096,
                      config->inner_radius * rcos(i * 128) / 4096, 0);
        }
        state->notified = 0;
        for (i = 0; i < 3; i++) {
            state->textures[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &D_8013BF54[i]);
        }
        state->elapsed = -config->delay;
        state->tick = 0;
        return 0;
    }

    config = state->config;
    ot = func_80058F10();
    if (state->elapsed < 0) {
        state->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyG3(&triangle);
    setRGB0(&triangle, 0, 0, 0);
    setRGB1(&triangle, 0, 0, 0);
    SetPolyFT4(&quad);
    setRGB0(&quad, config->quad_rgb[0], config->quad_rgb[1],
            config->quad_rgb[2]);
    GsSetLsMatrix(&light);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(),
                                    config->part), &matrix);
    setVector(&translation, matrix.t[0], matrix.t[1], matrix.t[2]);

    if (state->elapsed < config->release_at) {
        Model_SetSlotPlaybackDirection(0, 0);
        Model_SetSlotPlaybackDirection(1, 0);
        Model_SetSlotAmbient(2, -1024, -1024, -1024);
        angle = state->rotations;
        velocity = state->velocities;
        setVector(&v2, 0, 0, 0);
        for (i = 0; i < config->triangle_count; angle++, velocity++, i++) {
            GsSetLsMatrix(&light);
            copyVector(&rotation, velocity);
            rotation.vx *= state->elapsed;
            rotation.vy *= state->elapsed;
            rotation.vz *= state->elapsed;
            addVector(&rotation, angle);
            RotTrans(&translation, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&light, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setVector(&v0, config->triangle_width, config->triangle_height, 0);
            setVector(&v1, -config->triangle_width, config->triangle_height, 0);
            depth = RotAverage3(&v0, &v1, &v2, (PSXLONG *)&triangle.x0,
                               (PSXLONG *)&triangle.x1, (PSXLONG *)&triangle.x2,
                               &p, &flag);
            setRGB2(&triangle, config->triangle_rgb[0],
                    config->triangle_rgb[1], config->triangle_rgb[2]);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
            }
            setVector(&v0, config->triangle_width / 2,
                      config->triangle_height / 2, 0);
            setVector(&v1, -config->triangle_width / 2,
                      config->triangle_height / 2, 0);
            depth = RotAverage3(&v0, &v1, &v2, (PSXLONG *)&triangle.x0,
                               (PSXLONG *)&triangle.x1, (PSXLONG *)&triangle.x2,
                               &p, &flag);
            setRGB2(&triangle, 255, 255, 255);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
            }
        }
    } else {
        Model_SetSlotPlaybackDirection(0, 1);
        Model_SetSlotPlaybackDirection(1, 1);
    }

    if (state->elapsed < config->burst_at) {
        quad.tpage = state->textures[0] >> 16;
        quad.clut = state->textures[0];
        SetSemiTrans(&quad, 1);
        setVector(&v0, -config->quad_extent / 4, -config->quad_extent / 4, 0);
        setVector(&v1, config->quad_extent / 4, -config->quad_extent / 4, 0);
        setVector(&v2, -config->quad_extent / 4, config->quad_extent / 4, 0);
        setVector(&projected.fourth, config->quad_extent / 4,
                  config->quad_extent / 4, 0);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&light);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(),
                                        config->part), &matrix);
        setVector(&translation, matrix.t[0], matrix.t[1], matrix.t[2]);
        size = (state->tick & 1) * 512 + 4096;
        setRGB0(&quad, 128, 128, 128);
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&light);
        RotTrans(&translation, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&v0, &v1, &v2, &projected.fourth,
                           (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                           (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag) - 30;
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }

    elapsed = state->elapsed - config->burst_at;
    if (elapsed >= 0 && elapsed < config->burst_duration) {
        {
            quad.tpage = state->textures[0] >> 16;
            quad.clut = state->textures[0];
            SetSemiTrans(&quad, 1);
            setVector(&v0, -config->quad_extent, -config->quad_extent, 0);
            setVector(&v1, config->quad_extent, -config->quad_extent, 0);
            setVector(&v2, -config->quad_extent, config->quad_extent, 0);
            setVector(&projected.fourth, config->quad_extent,
                      config->quad_extent, 0);
            setVector(&rotation, 0, 0, 0);
            setVector(&translation, 0, -350, side * 450);
            size = elapsed * 4096 / config->burst_duration + 2048;
            setVector(&scale, size, size, size);
            red = config->quad_rgb[0] * (config->burst_duration - elapsed) /
                  config->burst_duration;
            green = config->quad_rgb[1] * (config->burst_duration - elapsed) /
                    config->burst_duration;
            blue = config->quad_rgb[2] * (config->burst_duration - elapsed) /
                   config->burst_duration;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&light);
            RotTrans(&translation, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&v0, &v1, &v2, &projected.fourth,
                               (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                               (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &p, &flag);
            setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        {
            quad.tpage = state->textures[1] >> 16;
            quad.clut = state->textures[1];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
            GsSetLsMatrix(&light);
            RotTrans(&translation, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&light, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(state->rings, projected.screens, depths, interpolation,
                          flags, 66);
            first = projected.screens;
            second = &projected.screens[33];
            first_depth = depths;
            second_depth = &depths[33];
            pair_flags = flags;
            for (i = 0; i < 32; first++, second++,
                 first_depth++, second_depth++, pair_flags++, i++) {
                setXY4(&quad, first[0].vx, first[0].vy,
                       first[1].vx, first[1].vy,
                       second[0].vx, second[0].vy,
                       second[1].vx, second[1].vy);
                depth = AverageZ4(first_depth[0], first_depth[1],
                                  second_depth[0], second_depth[1]);
                endpoint_flag = pair_flags[0];
                flag = (pair_flags[1] | endpoint_flag) & 32;
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    PopMatrix();
    state->elapsed += Model_GetFrameStep();
    state->tick++;
    if (state->elapsed >= config->release_at) {
        if (state->elapsed < config->burst_at) {
            Model_SetSlotAmbient(2, 2048, 2048, 2048);
        } else if (state->elapsed > config->burst_at + config->burst_duration) {
            return 2;
        } else if (!state->notified) {
            state->notified++;
            return 1;
        }
    }
    return 0;
}
