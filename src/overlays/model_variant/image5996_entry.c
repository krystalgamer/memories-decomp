#include "../../types.h"
#include "image5996_entry.h"

s32 FrenchModelVariant102_HandleRequest(ModelVariantFunnelState *context, s32 command)
{
    ModelVariantFunnelState *state;
    ModelVariantFunnelConfig *config;
    MATRIX light;
    MATRIX matrix;
    POLY_G3 triangle;
    POLY_FT4 quad;
    SVECTOR rotation;
    SVECTOR translation;
    VECTOR scale;
    SVECTOR v0, v1, v2;
    ModelVariantFunnelProjection projected;
    u16 depths[66];
    u16 interpolation[66];
    u16 flags[66];
    long flag;
    long p;
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
    scale = gFrenchModelVariant102_UnitScale;
    state = context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = state->config = &gFrenchModelVariant102_aConfigs[command];
        angle = state->angles;
        velocity = state->speeds;
        for (i = 0; i < config->count; angle++, velocity++, i++) {
            setVector(angle, rand() % 4096,
                      rand() % 4096, rand() % 4096);
            setVector(velocity, rand() % 32,
                      rand() % 32, rand() % 32);
            velocity->vz = 32 - velocity->vx;
        }
        angle = state->rings;
        for (i = 0; i <= 32; angle++, i++) {
            setVector(angle,
                      config->outer_radius * ccos(i * 128) / 4096,
                      config->outer_radius * csin(i * 128) / 4096,
                      -side * config->inner_radius);
        }
        for (i = 0; i <= 32; angle++, i++) {
            setVector(angle,
                      config->inner_radius * ccos(i * 128) / 4096,
                      config->inner_radius * csin(i * 128) / 4096, 0);
        }
        state->hit = 0;
        for (i = 0; i < 3; i++) {
            state->texture[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant102_aImages[i]);
        }
        state->elapsed = -config->delay;
        state->frame = 0;
        return 0;
    }

    config = state->config;
    ot = Graphics_GetDefaultOrderingTable();
    if (state->elapsed < 0) {
        state->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    light = *Model_GetLightSourceMatrix();
    SetPolyG3(&triangle);
    setRGB0(&triangle, 0, 0, 0);
    setRGB1(&triangle, 0, 0, 0);
    SetPolyFT4(&quad);
    setRGB0(&quad, config->sprite_red, config->sprite_green,
            config->sprite_blue);
    GsSetLsMatrix(&light);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(),
                                    config->part), &matrix);
    setVector(&translation, matrix.t[0], matrix.t[1], matrix.t[2]);

    if (state->elapsed < config->spin_end) {
        Model_SetSlotPlaybackDirection(0, 0);
        Model_SetSlotPlaybackDirection(1, 0);
        Model_SetSlotAmbient(2, -1024, -1024, -1024);
        angle = state->angles;
        velocity = state->speeds;
        setVector(&v2, 0, 0, 0);
        for (i = 0; i < config->count; angle++, velocity++, i++) {
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
            setVector(&v0, config->width, config->height, 0);
            setVector(&v1, -config->width, config->height, 0);
            depth = RotTransPers3(&v0, &v1, &v2, (long *)&triangle.x0,
                               (long *)&triangle.x1, (long *)&triangle.x2,
                               &p, &flag);
            setRGB2(&triangle, config->red,
                    config->green, config->blue);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
            }
            setVector(&v0, config->width / 2,
                      config->height / 2, 0);
            setVector(&v1, -config->width / 2,
                      config->height / 2, 0);
            depth = RotTransPers3(&v0, &v1, &v2, (long *)&triangle.x0,
                               (long *)&triangle.x1, (long *)&triangle.x2,
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

    if (state->elapsed < config->burst_start) {
        quad.tpage = state->texture[0] >> 16;
        quad.clut = state->texture[0];
        SetSemiTrans(&quad, 1);
        setVector(&v0, -config->size / 4, -config->size / 4, 0);
        setVector(&v1, config->size / 4, -config->size / 4, 0);
        setVector(&v2, -config->size / 4, config->size / 4, 0);
        setVector(&projected.corner, config->size / 4,
                  config->size / 4, 0);
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&light);
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(),
                                        config->part), &matrix);
        setVector(&translation, matrix.t[0], matrix.t[1], matrix.t[2]);
        size = (state->frame & 1) * 512 + 4096;
        setRGB0(&quad, 128, 128, 128);
        setVector(&scale, size, size, size);
        GsSetLsMatrix(&light);
        RotTrans(&translation, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&v0, &v1, &v2, &projected.corner,
                           (long *)&quad.x0, (long *)&quad.x1,
                           (long *)&quad.x2, (long *)&quad.x3, &p, &flag) - 30;
        setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
        if (depth >= 0 && flag >= 0) {
            GsSortPoly(&quad, ot, (u16)depth);
        }
    }

    elapsed = state->elapsed - config->burst_start;
    if (elapsed >= 0 && elapsed < config->duration) {
        {
            quad.tpage = state->texture[0] >> 16;
            quad.clut = state->texture[0];
            SetSemiTrans(&quad, 1);
            setVector(&v0, -config->size, -config->size, 0);
            setVector(&v1, config->size, -config->size, 0);
            setVector(&v2, -config->size, config->size, 0);
            setVector(&projected.corner, config->size,
                      config->size, 0);
            setVector(&rotation, 0, 0, 0);
            setVector(&translation, 0, -350, side * 450);
            size = elapsed * 4096 / config->duration + 2048;
            setVector(&scale, size, size, size);
            red = config->sprite_red * (config->duration - elapsed) /
                  config->duration;
            green = config->sprite_green * (config->duration - elapsed) /
                    config->duration;
            blue = config->sprite_blue * (config->duration - elapsed) /
                   config->duration;
            setRGB0(&quad, red, green, blue);
            GsSetLsMatrix(&light);
            RotTrans(&translation, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&v0, &v1, &v2, &projected.corner,
                               (long *)&quad.x0, (long *)&quad.x1,
                               (long *)&quad.x2, (long *)&quad.x3, &p, &flag);
            setUV4(&quad, 48, 0, 111, 0, 48, 63, 111, 63);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        {
            quad.tpage = state->texture[1] >> 16;
            quad.clut = state->texture[1];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
            GsSetLsMatrix(&light);
            RotTrans(&translation, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&light, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(state->rings, projected.screen, depths, interpolation,
                          flags, 66);
            first = projected.screen;
            second = &projected.screen[33];
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
    state->frame++;
    if (state->elapsed >= config->spin_end) {
        if (state->elapsed < config->burst_start) {
            Model_SetSlotAmbient(2, 2048, 2048, 2048);
        } else if (state->elapsed > config->burst_start + config->duration) {
            return 2;
        } else if (!state->hit) {
            state->hit++;
            return 1;
        }
    }
    return 0;
}
