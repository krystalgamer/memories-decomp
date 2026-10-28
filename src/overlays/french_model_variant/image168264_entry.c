#include "../../types.h"
#include "image168264_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Image168264State *state;
    MATRIX light;
    MATRIX matrix;
    POLY_FT4 unused;
    POLY_G3 triangle;
    GsBOXF box;
    SVECTOR rotation;
    SVECTOR translation;
    VECTOR scale;
    SVECTOR corners[4];
    DVECTOR screens[64];
    u16 depths[64];
    u16 interpolation[64];
    u16 flags[64];
    ModelTintColor tint_start;
    ModelTintColor tint_end;
    PSXLONG p;
    PSXLONG flag;
    GsOT *ot;
    s32 i;
    s32 step;
    s32 j;
    s32 value;
    s32 age;
    s32 elevation;
    u16 red, green, blue;
    s32 other;
    Image168264Config *config;
    SVECTOR *position;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&translation, 0, 8);
    scale = D_8013BFA0;
    state = (Image168264State *)context;
    Model_GetActiveSlotIndex();
    step = Model_GetFrameStep();
    if (command >= 0) {
        config = state->config = &D_8013BFE8[command % 100];
        position = state->origins;
        for (j = 0; j < 16; position++, j++) {
            setVector(position, 0, 0, 0);
        }
        for (j = 0; j < 64; j++) {
            state->ray_scales[j] = rand() % 4096 + 2048;
        }
        position = state->particles;
        for (j = 0; j < 64; position++, j++) {
            value = config->radius_base + rand() % config->radius_range / 2;
            other = rand() % 4096;
            elevation = rand() % 4096;
            setVector(position,
                (value * rsin(other) / 4096) * rsin(elevation) / 4096,
                (value * rcos(other) / 4096) * rsin(elevation) / 4096,
                value * rcos(elevation) / 4096);
        }
        for (j = 0; j < 2; j++) {
            state->textures[j] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &D_8013BFB0[j]);
        }
        state->phase = 2;
        state->elapsed = 0;
        state->notified = 0;
        if (command >= 100) {
            state->tint_enabled = 1;
        } else {
            state->tint_enabled = 0;
        }
        return 0;
    }

    config = state->config;
    ot = func_80058F10();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (state->elapsed < config->delay) {
        state->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    age = Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex()) -
          config->animation_anchor;
    if (age >= 0 && age < config->tint_duration / 2 &&
        age % 3 < 2 && state->tint_enabled == 1) {
        Model_SetFrameStepOverride(1);
        tint_start.r = 96;
        tint_start.g = 96;
        tint_start.b = 96;
        tint_end.r = 0;
        tint_end.g = 0;
        tint_end.b = 0;
        Model_QueueTintRequestForAllParts(Model_GetActiveSlotIndex(), 2,
            tint_start, tint_end, config->tint_duration / 2 - age);
    }
    SetPolyFT4(&unused);
    SetPolyG3(&triangle);
    i = 0;
    box.attribute = 0x50000000;
    box.w = 2;
    box.h = 2;
    position = state->origins;
    if (config->starts[i] >= 0) {
        do {
            age = state->elapsed - config->starts[i];
            if (age < 0) {
                GsSetLsMatrix(&light);
                GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(),
                    config->parts[i]), &matrix);
                setVector(position, matrix.t[0], matrix.t[1], matrix.t[2]);
            } else if (age < config->ray_duration) {
                setVector(&corners[0], 0, -config->radius_range, -config->ray_width);
                setVector(&corners[1], -config->ray_width / 2, 0, 0);
                setVector(&corners[2], config->ray_width / 2, 0, 0);
                red = config->ray_rgb[0] * (config->ray_duration - age) /
                      config->ray_duration;
                blue = config->ray_rgb[2] * (config->ray_duration - age) /
                       config->ray_duration;
                green = config->ray_rgb[1] * (config->ray_duration - age) /
                        config->ray_duration;
                setRGB0(&triangle, 0, 0, 0);
                setRGB1(&triangle, red >> 3, green >> 3, blue >> 3);
                setRGB2(&triangle, red, green, blue);
                copyVector(&translation, position);
                GsSetLsMatrix(&light);
                RotTransPers(position, (PSXLONG *)&triangle.x2, &p, &flag);
                for (j = 0; j < 64; j++) {
                    value = j * 64;
                    setVector(&rotation, 0, 0, value);
                    value = state->ray_scales[j] * age / config->ray_duration;
                    setVector(&scale, value, value, value);
                    GsSetLsMatrix(&light);
                    RotTrans(&translation, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage3(&corners[0], &corners[1], &corners[2],
                        (PSXLONG *)&triangle.x0, (PSXLONG *)&triangle.x1,
                        (PSXLONG *)&unused.x0, &p, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
                        triangle.x1 = unused.x0;
                        triangle.y1 = unused.y0;
                        Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
                    }
                }
                blue = (config->ray_duration - age) * 128 / config->ray_duration;
                setRGB0(&triangle, 0, 0, 0);
                setRGB1(&triangle, 0, 0, 0);
                setRGB2(&triangle, blue, blue, blue);
                for (j = 0; j < 64; j++) {
                    value = j * 64;
                    setVector(&rotation, 0, 0, value);
                    value = (state->ray_scales[j] * 2 / 3) * age /
                            config->ray_duration;
                    setVector(&scale, value, value, value);
                    GsSetLsMatrix(&light);
                    RotTrans(&translation, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage3(&corners[0], &corners[1], &corners[2],
                        (PSXLONG *)&triangle.x0, (PSXLONG *)&triangle.x1,
                        (PSXLONG *)&unused.x0, &p, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
                        triangle.x1 = unused.x0;
                        triangle.y1 = unused.y0;
                        Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
                    }
                }
            }
            i++;
            position++;
        } while (config->starts[i] >= 0);
        position = state->origins;
        for (i = 0; config->starts[i] >= 0; i++, position++) {
            age = state->elapsed - config->starts[i];
            if (age >= 0 && age < config->particle_duration) {
                box.r = config->particle_rgb[0] * (config->particle_duration - age) /
                        config->particle_duration;
                box.g = config->particle_rgb[1] * (config->particle_duration - age) /
                        config->particle_duration;
                box.b = config->particle_rgb[2] * (config->particle_duration - age) /
                        config->particle_duration;
                copyVector(&translation, position);
                setVector(&rotation, 0, 0, 0);
                value = age * 4096 / config->particle_duration;
                setVector(&scale, value, value, value);
                GsSetLsMatrix(&light);
                RotTrans(&translation, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&light, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                RotTransPersN(state->particles, screens, depths, interpolation, flags, 64);
                for (j = 0; j < 64; j++) {
                    box.x = screens[j].vx;
                    box.y = screens[j].vy;
                    depth = depths[j] / 4;
                    if (depth >= 0 && (flags[j] & 32) == 0) {
                        GsSortBoxFill(&box, ot, depth);
                    }
                }
            }
        }
    }
    if (Model_GetSlotAnimationFrame(Model_GetActiveSlotIndex()) >=
        config->animation_anchor + config->tint_duration / 2) {
        value = Model_GetActiveSlotIndex();
        other = Model_GetActiveSlotIndex() ^ 1;
        setVector(&corners[0], 0, 0, 0);
        switch (state->phase) {
        case 0:
            Model_RequestCameraMoveToSlots(1, -1, -1, 0);
            break;
        case 1:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 1, &corners[0]);
            state->elapsed = config->animation_anchor * 2;
            Model_SeekSlotAnimation(value, Model_GetSlotAnimationIndex(value),
                                   config->animation_anchor);
            Model_SeekSlotAnimation(other, 1, 0);
            state->notified = 0;
            state->phase--;
            break;
        case 2:
            Model_SetCameraOrbitFromPreset(Model_GetActiveSlotIndex(), 0, &corners[0]);
            state->elapsed = config->animation_anchor * 2;
            Model_SeekSlotAnimation(value, Model_GetSlotAnimationIndex(value),
                                   config->animation_anchor);
            Model_SeekSlotAnimation(other, 1, 0);
            state->notified = 0;
            state->phase--;
            break;
        }
    }
    PopMatrix();
    state->elapsed += step;
    age = state->elapsed - config->starts[0];
    if (age >= 0) {
        for (j = 0; config->starts[j] >= 0; j++) {
            age = state->elapsed - config->starts[j];
        }
        if (age >= config->particle_duration) {
            return 2;
        }
        for (j = 0; config->starts[j] >= 0; j++) {
            age = state->elapsed - config->starts[j];
            if (age >= 0 && state->notified == j) {
                state->notified = j + 1;
                if (config->starts[state->notified] < 0) {
                    return 1;
                }
                return 4;
            }
        }
        value = Model_GetSlotAnimationIndex(Model_GetActiveSlotIndex() ^ 1);
        if (value == 5 || value == 8) {
            return 4;
        }
    }
    return 0;
}
