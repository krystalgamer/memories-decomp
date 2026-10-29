#include "../../types.h"
#include "image32768_entry.h"

#define IMAGE32768_FADE(value, amount) \
    if ((value) >= 16) (value) -= (amount); else (value) = 0

#define IMAGE32768_PROJECT_FLARE() \
    depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3], \
        (PSXLONG *)&flare.x0, (PSXLONG *)&flare.x1, \
        (PSXLONG *)&flare.x2, (PSXLONG *)&flare.x3, &interpolation, &flag); \
    setUV4(&flare, 0, 0, 63, 0, 0, 63, 63, 63); \
    if (depth >= 0 && flag >= 0) \
        Graphics_SubmitDrawModePacket((u32 *)&flare, ot, (u16)depth, 1)

#define IMAGE32768_PROJECT_FAN(tip) \
    depth = RotAverage3((tip), &corners[0], &corners[1], \
        (PSXLONG *)&fan.x0, (PSXLONG *)&fan.x1, (PSXLONG *)&fan.x2, \
        &interpolation, &flag); \
    if (depth >= 0 && flag >= 0) \
        Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1)

s32 func_8013B004(u8 *context, s32 command)
{
    Image32768State *work;
    MATRIX light, matrix;
    POLY_GT4 beam;
    POLY_FT4 flare;
    POLY_F4 flash;
    POLY_G3 fan;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[4];
    PSXLONG flag, interpolation, depth;
    GsOT *ot;
    s32 i, slot, direction, step;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C2DC;
    work = (Image32768State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C324[command];
        setVector(&work->tips[0], 0, 0, direction * work->config->tip_distance);
        setVector(&work->tips[1], 0, 0,
            ((direction << 1) + direction) * work->config->tip_distance / 4);
        work->textures[0] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &D_8013C2EC[0]);
        work->textures[1] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &D_8013C2EC[1]);
        work->textures[2] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 0, &D_8013C2EC[1]);
        work->ambient = 2048;
        work->ambient_phase = 0;
        work->elapsed = 0;
        work->flare_r = work->config->flare_r;
        work->flare_g = work->config->flare_g;
        work->flare_b = work->config->flare_b;
        work->fan_r = work->config->flare_r;
        work->fan_g = work->config->flare_g;
        work->fan_b = work->config->flare_b;
        for (i = 0; i < 3; i++) {
            work->r[i] = work->config->beam_r;
            work->g[i] = work->config->beam_g;
            work->b[i] = work->config->beam_b;
        }
        work->phase = 0;
        work->flash = 255;
        work->white = 255;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_end) {
        if (direction > 0)
            Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, -16);
        else
            Model_QueueCameraOrbitFromPreset(slot, 1, &rotation, -16);
    } else {
        if (direction > 0)
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rotation, -8);
        else
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, -8);
    }
    if (work->ambient_phase == 0) {
        if (work->ambient > -512)
            work->ambient -= step * 64;
        else
            work->ambient = -512;
    } else if (work->ambient_phase == 1) {
        if (work->ambient < 2048)
            work->ambient += step * 64;
        else {
            work->ambient_phase = 2;
            work->ambient = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();

    if (work->r[1] || work->g[1] || work->b[1] ||
        work->r[2] || work->g[2] || work->b[2]) {
        setPolyGT4(&beam);
        setVector(&corners[0], 0, -work->config->plane_height,
            direction * work->config->plane_extent);
        setVector(&corners[1], 0, -work->config->plane_height,
            -direction * work->config->plane_extent);
        setVector(&corners[2], 0, work->config->plane_height,
            direction * work->config->plane_extent);
        setVector(&corners[3], 0, work->config->plane_height,
            -direction * work->config->plane_extent);
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, direction * work->config->roll);
        GsSetLsMatrix(&light);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&light, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
            (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1,
            (PSXLONG *)&beam.x2, (PSXLONG *)&beam.x3, &interpolation, &flag);
        if (work->phase == 0) {
            beam.clut = work->textures[2];
            beam.tpage = work->textures[2] >> 16;
            setRGB0(&beam, work->r[0], work->g[0], work->b[0]);
            setRGB1(&beam, work->r[0], work->g[0], work->b[0]);
            setRGB2(&beam, 0, 0, 0);
            setRGB3(&beam, 0, 0, 0);
            setUV4(&beam, 0, 64, 63, 64, 0, 138, 63, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 0);
        }
        if (work->phase == 1) {
            beam.clut = work->textures[2];
            beam.tpage = work->textures[2] >> 16;
            setRGB0(&beam, work->r[1], work->g[1], work->b[1]);
            setRGB1(&beam, work->r[1], work->g[1], work->b[1]);
            setRGB2(&beam, work->r[1], work->g[1], work->b[1]);
            setRGB3(&beam, 0, 0, 0);
            setUV4(&beam, 64, 64, 127, 64, 64, 138, 127, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 0);
        }
        if (work->phase == 2) {
            beam.clut = work->textures[1];
            beam.tpage = work->textures[1] >> 16;
            setRGB0(&beam, work->r[1], work->g[1], work->b[1]);
            setRGB1(&beam, work->r[1], work->g[1], work->b[1]);
            setRGB2(&beam, work->r[1], work->g[1], work->b[1]);
            setRGB3(&beam, 0, 0, 0);
            setUV4(&beam, 64, 64, 127, 64, 64, 138, 127, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 1);
            setRGB0(&beam, work->r[2], work->g[2], work->b[2]);
            setRGB1(&beam, work->r[2], work->g[2], work->b[2]);
            setRGB2(&beam, work->r[2], work->g[2], work->b[2]);
            setRGB3(&beam, work->r[2], work->g[2], work->b[2]);
            setUV4(&beam, 128, 64, 191, 64, 128, 138, 191, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 1);
            IMAGE32768_FADE(work->r[1], work->config->beam_fade);
            IMAGE32768_FADE(work->g[1], work->config->beam_fade);
            IMAGE32768_FADE(work->b[1], work->config->beam_fade);
            IMAGE32768_FADE(work->r[2], work->config->flare_fade);
            IMAGE32768_FADE(work->g[2], work->config->flare_fade);
            IMAGE32768_FADE(work->b[2], work->config->flare_fade);
        }
        if (work->phase < 2)
            work->phase++;
    }

    if (work->flash) {
        setPolyF4(&flash);
        setRGB0(&flash, work->flash, work->flash, work->flash);
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
        depth = (work->phase < 2) * 2;
        if (depth >= 0)
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, depth, 1);
        IMAGE32768_FADE(work->flash, work->config->beam_fade);
    }

    if (work->flare_r || work->flare_g || work->flare_b) {
        setPolyFT4(&flare);
        setRGB0(&flare, work->flare_r, work->flare_g, work->flare_b);
        flare.clut = work->textures[0];
        flare.tpage = work->textures[0] >> 16;
        setVector(&corners[0], -work->config->flare_size, -work->config->flare_size, 0);
        setVector(&corners[1], work->config->flare_size, -work->config->flare_size, 0);
        setVector(&corners[2], -work->config->flare_size, work->config->flare_size, 0);
        setVector(&corners[3], work->config->flare_size, work->config->flare_size, 0);
        setVector(&position, 0, -350, direction * (work->config->plane_extent + 450));
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&light);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        IMAGE32768_PROJECT_FLARE();
        setVector(&corners[0], -work->config->flare_size / 2, -work->config->flare_size / 2, 0);
        setVector(&corners[1], work->config->flare_size / 2, -work->config->flare_size / 2, 0);
        setVector(&corners[2], -work->config->flare_size / 2, work->config->flare_size / 2, 0);
        setVector(&corners[3], work->config->flare_size / 2, work->config->flare_size / 2, 0);
        IMAGE32768_PROJECT_FLARE();

        setPolyG3(&fan);
        setVector(&position, 0, -350, direction * 450);
        for (i = 0; i < work->config->fan_count; i++) {
            setRGB0(&fan, 0, 0, 0);
            setRGB1(&fan, work->fan_r / 2, work->fan_g / 2, work->fan_b / 2);
            setRGB2(&fan, work->fan_r / 2, work->fan_g / 2, work->fan_b / 2);
            setVector(&corners[0], 0, -work->config->fan_height,
                direction * work->config->fan_depth);
            setVector(&corners[1], 0, work->config->fan_height,
                direction * work->config->fan_depth);
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, i * 128 - 768, 0, 0);
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&light, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            IMAGE32768_PROJECT_FAN(&work->tips[0]);
            setRGB0(&fan, 0, 0, 0);
            setRGB1(&fan, work->white / 2, work->white / 2, work->white / 2);
            setRGB2(&fan, work->white / 2, work->white / 2, work->white / 2);
            IMAGE32768_PROJECT_FAN(&work->tips[1]);
        }
        if (work->fan_r > 24) work->fan_r -= 24; else work->fan_r = 0;
        if (work->fan_g > 24) work->fan_g -= 24; else work->fan_g = 0;
        if (work->fan_b > 24) work->fan_b -= 24; else work->fan_b = 0;
        if (work->white > 24) work->white -= 24; else work->white = 0;
        IMAGE32768_FADE(work->flare_r, work->config->flare_fade);
        IMAGE32768_FADE(work->flare_g, work->config->flare_fade);
        IMAGE32768_FADE(work->flare_b, work->config->flare_fade);
    }
    work->elapsed += step;
    PopMatrix();
    if (work->ambient_phase == 2 &&
        !(work->flare_r || work->flare_g || work->flare_b))
        return 2;
    if (work->ambient_phase == 0) {
        work->ambient_phase = 1;
        return 1;
    }
    return 0;
}
