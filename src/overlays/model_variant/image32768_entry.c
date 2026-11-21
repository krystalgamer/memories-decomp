#include "../../types.h"
#include "image32768_entry.h"

#define IMAGE32768_FADE(value, amount) \
    if ((value) >= 16) (value) -= (amount); else (value) = 0

#define IMAGE32768_PROJECT_FLARE() \
    depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3], \
        (long *)&flare.x0, (long *)&flare.x1, \
        (long *)&flare.x2, (long *)&flare.x3, &interpolation, &flag); \
    setUV4(&flare, 0, 0, 63, 0, 0, 63, 63, 63); \
    if (depth >= 0 && flag >= 0) \
        Graphics_SubmitDrawModePacket((u32 *)&flare, ot, (u16)depth, 1)

#define IMAGE32768_PROJECT_FAN(tip) \
    depth = RotAverage3((tip), &corners[0], &corners[1], \
        (long *)&fan.x0, (long *)&fan.x1, (long *)&fan.x2, \
        &interpolation, &flag); \
    if (depth >= 0 && flag >= 0) \
        Graphics_SubmitDrawModePacket((u32 *)&fan, ot, (u16)depth, 1)

s32 FrenchModelVariant804_HandleRequest(ModelVariantBandSpokeState *context, s32 command)
{
    ModelVariantBandSpokeState *work;
    MATRIX light, matrix;
    POLY_GT4 beam;
    POLY_FT4 flare;
    POLY_F4 flash;
    POLY_G3 fan;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR corners[4];
    long flag, interpolation, depth;
    GsOT *ot;
    s32 i, slot, direction, step;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gFrenchModelVariant804_UnitScale;
    work = context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gFrenchModelVariant804_aConfigs[command];
        setVector(&work->tips[0], 0, 0, direction * work->config->tip_depth);
        setVector(&work->tips[1], 0, 0,
            ((direction << 1) + direction) * work->config->tip_depth / 4);
        work->texture[0] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant804_aImages[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant804_aImages[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(
            Model_GetActiveSlotIndex(), 0, &gFrenchModelVariant804_aImages[1]);
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->glow_r = work->config->glow_r;
        work->glow_g = work->config->glow_g;
        work->glow_b = work->config->glow_b;
        work->spoke_r = work->config->glow_r;
        work->spoke_g = work->config->glow_g;
        work->spoke_b = work->config->glow_b;
        for (i = 0; i < 3; i++) {
            work->red[i] = work->config->band_r;
            work->green[i] = work->config->band_g;
            work->blue[i] = work->config->band_b;
        }
        work->stage = 0;
        work->flash = 255;
        work->spoke = 255;
        return 0;
    }

    ot = Graphics_GetDefaultOrderingTable();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->switch_time) {
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
    if (work->phase == 0) {
        if (work->level > -512)
            work->level -= step * 64;
        else
            work->level = -512;
    } else if (work->phase == 1) {
        if (work->level < 2048)
            work->level += step * 64;
        else {
            work->phase = 2;
            work->level = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    light = *Model_GetLightSourceMatrix();

    if (work->red[1] || work->green[1] || work->blue[1] ||
        work->red[2] || work->green[2] || work->blue[2]) {
        setPolyGT4(&beam);
        setVector(&corners[0], 0, -work->config->band_height,
            direction * work->config->band_depth);
        setVector(&corners[1], 0, -work->config->band_height,
            -direction * work->config->band_depth);
        setVector(&corners[2], 0, work->config->band_height,
            direction * work->config->band_depth);
        setVector(&corners[3], 0, work->config->band_height,
            -direction * work->config->band_depth);
        setVector(&position, 0, -350, direction * 450);
        setVector(&rotation, 0, 0, direction * work->config->spin);
        GsSetLsMatrix(&light);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&light, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
            (long *)&beam.x0, (long *)&beam.x1,
            (long *)&beam.x2, (long *)&beam.x3, &interpolation, &flag);
        if (work->stage == 0) {
            beam.clut = work->texture[2];
            beam.tpage = work->texture[2] >> 16;
            setRGB0(&beam, work->red[0], work->green[0], work->blue[0]);
            setRGB1(&beam, work->red[0], work->green[0], work->blue[0]);
            setRGB2(&beam, 0, 0, 0);
            setRGB3(&beam, 0, 0, 0);
            setUV4(&beam, 0, 64, 63, 64, 0, 138, 63, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 0);
        }
        if (work->stage == 1) {
            beam.clut = work->texture[2];
            beam.tpage = work->texture[2] >> 16;
            setRGB0(&beam, work->red[1], work->green[1], work->blue[1]);
            setRGB1(&beam, work->red[1], work->green[1], work->blue[1]);
            setRGB2(&beam, work->red[1], work->green[1], work->blue[1]);
            setRGB3(&beam, 0, 0, 0);
            setUV4(&beam, 64, 64, 127, 64, 64, 138, 127, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 0);
        }
        if (work->stage == 2) {
            beam.clut = work->texture[1];
            beam.tpage = work->texture[1] >> 16;
            setRGB0(&beam, work->red[1], work->green[1], work->blue[1]);
            setRGB1(&beam, work->red[1], work->green[1], work->blue[1]);
            setRGB2(&beam, work->red[1], work->green[1], work->blue[1]);
            setRGB3(&beam, 0, 0, 0);
            setUV4(&beam, 64, 64, 127, 64, 64, 138, 127, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 1);
            setRGB0(&beam, work->red[2], work->green[2], work->blue[2]);
            setRGB1(&beam, work->red[2], work->green[2], work->blue[2]);
            setRGB2(&beam, work->red[2], work->green[2], work->blue[2]);
            setRGB3(&beam, work->red[2], work->green[2], work->blue[2]);
            setUV4(&beam, 128, 64, 191, 64, 128, 138, 191, 138);
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, 0, 1);
            IMAGE32768_FADE(work->red[1], work->config->band_fade);
            IMAGE32768_FADE(work->green[1], work->config->band_fade);
            IMAGE32768_FADE(work->blue[1], work->config->band_fade);
            IMAGE32768_FADE(work->red[2], work->config->glow_fade);
            IMAGE32768_FADE(work->green[2], work->config->glow_fade);
            IMAGE32768_FADE(work->blue[2], work->config->glow_fade);
        }
        if (work->stage < 2)
            work->stage++;
    }

    if (work->flash) {
        setPolyF4(&flash);
        setRGB0(&flash, work->flash, work->flash, work->flash);
        setXY4(&flash, 0, 0, 320, 0, 0, 256, 320, 256);
        depth = (work->stage < 2) * 2;
        if (depth >= 0)
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, depth, 1);
        IMAGE32768_FADE(work->flash, work->config->band_fade);
    }

    if (work->glow_r || work->glow_g || work->glow_b) {
        setPolyFT4(&flare);
        setRGB0(&flare, work->glow_r, work->glow_g, work->glow_b);
        flare.clut = work->texture[0];
        flare.tpage = work->texture[0] >> 16;
        setVector(&corners[0], -work->config->glow_size, -work->config->glow_size, 0);
        setVector(&corners[1], work->config->glow_size, -work->config->glow_size, 0);
        setVector(&corners[2], -work->config->glow_size, work->config->glow_size, 0);
        setVector(&corners[3], work->config->glow_size, work->config->glow_size, 0);
        setVector(&position, 0, -350, direction * (work->config->band_depth + 450));
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&light);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        IMAGE32768_PROJECT_FLARE();
        setVector(&corners[0], -work->config->glow_size / 2, -work->config->glow_size / 2, 0);
        setVector(&corners[1], work->config->glow_size / 2, -work->config->glow_size / 2, 0);
        setVector(&corners[2], -work->config->glow_size / 2, work->config->glow_size / 2, 0);
        setVector(&corners[3], work->config->glow_size / 2, work->config->glow_size / 2, 0);
        IMAGE32768_PROJECT_FLARE();

        setPolyG3(&fan);
        setVector(&position, 0, -350, direction * 450);
        for (i = 0; i < work->config->spoke_count; i++) {
            setRGB0(&fan, 0, 0, 0);
            setRGB1(&fan, work->spoke_r / 2, work->spoke_g / 2, work->spoke_b / 2);
            setRGB2(&fan, work->spoke_r / 2, work->spoke_g / 2, work->spoke_b / 2);
            setVector(&corners[0], 0, -work->config->spoke_length,
                direction * work->config->spoke_depth);
            setVector(&corners[1], 0, work->config->spoke_length,
                direction * work->config->spoke_depth);
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
            setRGB1(&fan, work->spoke / 2, work->spoke / 2, work->spoke / 2);
            setRGB2(&fan, work->spoke / 2, work->spoke / 2, work->spoke / 2);
            IMAGE32768_PROJECT_FAN(&work->tips[1]);
        }
        if (work->spoke_r > 24) work->spoke_r -= 24; else work->spoke_r = 0;
        if (work->spoke_g > 24) work->spoke_g -= 24; else work->spoke_g = 0;
        if (work->spoke_b > 24) work->spoke_b -= 24; else work->spoke_b = 0;
        if (work->spoke > 24) work->spoke -= 24; else work->spoke = 0;
        IMAGE32768_FADE(work->glow_r, work->config->glow_fade);
        IMAGE32768_FADE(work->glow_g, work->config->glow_fade);
        IMAGE32768_FADE(work->glow_b, work->config->glow_fade);
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2 &&
        !(work->glow_r || work->glow_g || work->glow_b))
        return 2;
    if (work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    return 0;
}
