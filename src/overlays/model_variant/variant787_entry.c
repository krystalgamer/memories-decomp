#include "../../types.h"
#include "variant787_entry.h"
#include "../../game/graphics_constants.h"

/* Header 787's entry: the camera orbits the active slot while the ambient
 * light dims; a gouraud band grows in front of the slot in three stages over
 * a fading screen flash, then a glow and a fan of spokes appear there and
 * fade out until the light recovers. */

static const VECTOR gModelVariant787_D12DC = {4096, 4096, 4096, 0};

s32 ModelVariant787_HandleRequest(u8 *context, s32 command)
{
    Model787State *work;
    MATRIX base, matrix;
    POLY_GT4 band;
    POLY_FT4 glow;
    POLY_F4 flash;
    POLY_G3 spoke;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    s32 slot, side, step;
    GsOT *ot;
    s32 i;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant787_D12DC;
    work = (Model787State *)context;
    slot = Model_GetActiveSlotIndex();
    side = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant787_D1324[command];
        setVector(&work->tips[0], 0, 0, side * work->config->tip_depth);
        setVector(&work->tips[1], 0, 0, side * (3 * work->config->tip_depth) / 4);
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant787_D12EC[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant787_D12EC[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 0, &gModelVariant787_D12EC[1]);
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
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->switch_time) {
        if (side > 0) {
            Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, -16);
        } else {
            Model_QueueCameraOrbitFromPreset(slot, 1, &rotation, -16);
        }
    } else if (side > 0) {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rotation, -8);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, -8);
    }
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 1) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
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
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->red[1] || work->green[1] || work->blue[1] || work->red[2] || work->green[2] || work->blue[2]) {
        setPolyGT4(&band);
        setVector(&vertices[0], 0, -work->config->band_height, side * work->config->band_depth);
        setVector(&vertices[1], 0, -work->config->band_height, -side * work->config->band_depth);
        setVector(&vertices[2], 0, work->config->band_height, side * work->config->band_depth);
        setVector(&vertices[3], 0, work->config->band_height, -side * work->config->band_depth);
        setVector(&position, 0, -350, side * 450);
        setVector(&rotation, 0, 0, side * work->config->spin);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&band.x0, (PSXLONG *)&band.x1, (PSXLONG *)&band.x2, (PSXLONG *)&band.x3,
            &interpolation, &flag);
        if (work->stage == 0) {
            band.clut = work->texture[2];
            band.tpage = work->texture[2] >> 16;
            setRGB0(&band, work->red[0], work->green[0], work->blue[0]);
            setRGB1(&band, work->red[0], work->green[0], work->blue[0]);
            setRGB2(&band, 0, 0, 0);
            setRGB3(&band, 0, 0, 0);
            setUV4(&band, 0, 64, 63, 64, 0, 138, 63, 138);
            Graphics_SubmitDrawModePacket((u32 *)&band, ot, 0, 0);
        }
        if (work->stage == 1) {
            band.clut = work->texture[2];
            band.tpage = work->texture[2] >> 16;
            setRGB0(&band, work->red[1], work->green[1], work->blue[1]);
            setRGB1(&band, work->red[1], work->green[1], work->blue[1]);
            setRGB2(&band, work->red[1], work->green[1], work->blue[1]);
            setRGB3(&band, 0, 0, 0);
            setUV4(&band, 64, 64, 127, 64, 64, 138, 127, 138);
            Graphics_SubmitDrawModePacket((u32 *)&band, ot, 0, 0);
        }
        if (work->stage == 2) {
            band.clut = work->texture[1];
            band.tpage = work->texture[1] >> 16;
            setRGB0(&band, work->red[1], work->green[1], work->blue[1]);
            setRGB1(&band, work->red[1], work->green[1], work->blue[1]);
            setRGB2(&band, work->red[1], work->green[1], work->blue[1]);
            setRGB3(&band, 0, 0, 0);
            setUV4(&band, 64, 64, 127, 64, 64, 138, 127, 138);
            Graphics_SubmitDrawModePacket((u32 *)&band, ot, 0, 1);
            setRGB0(&band, work->red[2], work->green[2], work->blue[2]);
            setRGB1(&band, work->red[2], work->green[2], work->blue[2]);
            setRGB2(&band, work->red[2], work->green[2], work->blue[2]);
            setRGB3(&band, work->red[2], work->green[2], work->blue[2]);
            setUV4(&band, 128, 64, 191, 64, 128, 138, 191, 138);
            Graphics_SubmitDrawModePacket((u32 *)&band, ot, 0, 1);
            if (work->red[1] > 15) work->red[1] -= work->config->band_fade;
            else work->red[1] = 0;
            if (work->green[1] > 15) work->green[1] -= work->config->band_fade;
            else work->green[1] = 0;
            if (work->blue[1] > 15) work->blue[1] -= work->config->band_fade;
            else work->blue[1] = 0;
            if (work->red[2] > 15) work->red[2] -= work->config->glow_fade;
            else work->red[2] = 0;
            if (work->green[2] > 15) work->green[2] -= work->config->glow_fade;
            else work->green[2] = 0;
            if (work->blue[2] > 15) work->blue[2] -= work->config->glow_fade;
            else work->blue[2] = 0;
        }
        if (work->stage < 2) {
            work->stage++;
        }
    }
    if (work->flash) {
        setPolyF4(&flash);
        setRGB0(&flash, work->flash, work->flash, work->flash);
        setXY4(&flash, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT, GRAPHICS_DEFAULT_WIDTH,
            GRAPHICS_DEFAULT_HEIGHT);
        depth = work->stage < 2 ? 2 : 0;
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&flash, ot, depth, 1);
        }
        if (work->flash > 15) work->flash -= work->config->band_fade;
        else work->flash = 0;
    }
    if (work->glow_r || work->glow_g || work->glow_b) {
        setPolyFT4(&glow);
        setRGB0(&glow, work->glow_r, work->glow_g, work->glow_b);
        glow.clut = work->texture[0];
        glow.tpage = work->texture[0] >> 16;
        setVector(&vertices[0], -work->config->glow_size, -work->config->glow_size, 0);
        setVector(&vertices[1], work->config->glow_size, -work->config->glow_size, 0);
        setVector(&vertices[2], -work->config->glow_size, work->config->glow_size, 0);
        setVector(&vertices[3], work->config->glow_size, work->config->glow_size, 0);
        setVector(&position, 0, -350, side * (work->config->band_depth + 450));
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3,
            &interpolation, &flag);
        setUV4(&glow, 0, 0, 63, 0, 0, 63, 63, 63);
        if (depth >= 0 && flag >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        setVector(&vertices[0], -work->config->glow_size / 2, -work->config->glow_size / 2, 0);
        setVector(&vertices[1], work->config->glow_size / 2, -work->config->glow_size / 2, 0);
        setVector(&vertices[2], -work->config->glow_size / 2, work->config->glow_size / 2, 0);
        setVector(&vertices[3], work->config->glow_size / 2, work->config->glow_size / 2, 0);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3,
            &interpolation, &flag);
        setUV4(&glow, 0, 0, 63, 0, 0, 63, 63, 63);
        if (depth >= 0 && flag >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        setPolyG3(&spoke);
        setVector(&position, 0, -350, side * 450);
        for (i = 0; i < work->config->spoke_count; i++) {
            setRGB0(&spoke, 0, 0, 0);
            setRGB1(&spoke, work->spoke_r / 2, work->spoke_g / 2, work->spoke_b / 2);
            setRGB2(&spoke, work->spoke_r / 2, work->spoke_g / 2, work->spoke_b / 2);
            setVector(&vertices[0], 0, -work->config->spoke_length, side * work->config->spoke_depth);
            setVector(&vertices[1], 0, work->config->spoke_length, side * work->config->spoke_depth);
            setVector(&scale, 4096, 4096, 4096);
            setVector(&rotation, (i << 7) - 0x300, 0, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage3(&work->tips[0], &vertices[0], &vertices[1],
                (PSXLONG *)&spoke.x0, (PSXLONG *)&spoke.x1, (PSXLONG *)&spoke.x2, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&spoke, ot, (u16)depth, 1);
            }
            setRGB0(&spoke, 0, 0, 0);
            setRGB1(&spoke, work->spoke / 2, work->spoke / 2, work->spoke / 2);
            setRGB2(&spoke, work->spoke / 2, work->spoke / 2, work->spoke / 2);
            depth = RotAverage3(&work->tips[1], &vertices[0], &vertices[1],
                (PSXLONG *)&spoke.x0, (PSXLONG *)&spoke.x1, (PSXLONG *)&spoke.x2, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&spoke, ot, (u16)depth, 1);
            }
        }
        if (work->spoke_r > 24) work->spoke_r -= 24;
        else work->spoke_r = 0;
        if (work->spoke_g > 24) work->spoke_g -= 24;
        else work->spoke_g = 0;
        if (work->spoke_b > 24) work->spoke_b -= 24;
        else work->spoke_b = 0;
        if (work->spoke > 24) work->spoke -= 24;
        else work->spoke = 0;
        if (work->glow_r > 15) work->glow_r -= work->config->glow_fade;
        else work->glow_r = 0;
        if (work->glow_g > 15) work->glow_g -= work->config->glow_fade;
        else work->glow_g = 0;
        if (work->glow_b > 15) work->glow_b -= work->config->glow_fade;
        else work->glow_b = 0;
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2 && !(work->glow_r || work->glow_g || work->glow_b)) {
        return 2;
    }
    if (work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    return 0;
}
