#include "../../types.h"
#include "h180_entry.h"
#include "../../game/model_slot_support.h"

/* The entry draws expanding, fading rings at this slot, a screen flash, and
 * then pairs of rays that grow upwards around the other slot. A negative
 * command draws a frame; any other command sets the work area up. */

static const VECTOR gModelVariant180_UnitScale = {4096, 4096, 4096, 0};

s32 ModelVariant180_HandleRequest(ModelVariantBeamRingState *context, s32 command)
{
    ModelVariantBeamRingState *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    DVECTOR screen[66];
    u16 depths[66], projection[66], flags[66];
    long flag, interpolation;
    ModelVariantBeamRingConfig *config;
    GsOT *ot;
    SVECTOR *point;
    s32 direction, i, j, angle, factor, time, offset, scale_value;
    long depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant180_UnitScale;
    work = context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &gModelVariant180_aConfigs[command];
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), &work->origin);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, &work->opponent);
        point = work->ring;
        for (i = 0; i < 33; point++, i++) {
            angle = i << 7;
            point->vx = config->radius * ccos(angle) / 4096;
            point->vy = -config->thickness;
            point->vz = config->radius * csin(angle) / 4096;
        }
        factor = config->radius - config->thickness;
        for (i = 0; i < 33; point++, i++) {
            angle = i << 7;
            point->vx = factor * ccos(angle) / 4096;
            point->vy = 0;
            point->vz = factor * csin(angle) / 4096;
        }
        point = work->beams;
        for (i = 0; i < 16; i++) {
            point->vx = config->spread * (i << 1) / 16;
            point->vy = 0;
            point->vz = direction * config->spread * i / 16;
            point++;
            point->vx = -config->spread * (i << 1) / 16;
            point->vy = 0;
            point->vz = direction * config->spread * i / 16;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->textures[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant180_aImages[i]);
        }
        work->time = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = Graphics_GetDefaultOrderingTable();
    base = *Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->textures[0] >> 16;
    quad.clut = work->textures[0];
    setVector(&rotation, 0, 0, 0);
    setVector(&position, work->origin.vx, work->origin.vy, work->origin.vz);
    position.vy = -350;
    factor = work->time - config->ring_begin;
    for (offset = 0; offset < config->ring_span; offset += config->ring_spacing) {
        if (factor >= offset && factor < offset + config->ring_duration) {
            s32 red, green, blue, remaining, t;
            t = factor - offset;
            scale_value = (t << 12) / config->ring_duration;
            setVector(&scale, scale_value, scale_value, scale_value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 66);
            remaining = config->ring_duration - t;
            red = config->ring_red * remaining / config->ring_duration;
            green = config->ring_green * remaining / config->ring_duration;
            blue = config->ring_blue * remaining / config->ring_duration;
            setRGB0(&quad, red, green, blue);
            for (i = 0; i < 32; i++) {
                scale_value = i + 33;
                setXY4(&quad, screen[i].vx, screen[i].vy,
                       screen[i + 1].vx, screen[i + 1].vy,
                       screen[scale_value].vx, screen[scale_value].vy,
                       screen[scale_value + 1].vx, screen[scale_value + 1].vy);
                depth = AverageZ4(depths[i], depths[i + 1], depths[i + 33], depths[i + 34]);
                flag = (flags[i] | flags[i + 1] | flags[i + 33] | flags[i + 34]) & 0x20;
                setUV4(&quad, i * 8, 0, i * 8 + 7, 0, i * 8, 31, i * 8 + 7, 31);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    offset = work->time - config->flash_begin;
    if (offset >= 0 && offset < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, 240, 320, 240);
        scale_value = (config->flash_duration - offset) * 255;
        scale_value /= config->flash_duration;
        setRGB0(&flash, scale_value, scale_value, scale_value);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    setVector(&vertices[0], -config->half_width, -config->height, 0);
    setVector(&vertices[1], config->half_width, -config->height, 0);
    setVector(&vertices[2], -config->half_width, config->height, 0);
    setVector(&vertices[3], config->half_width, config->height, 0);
    setVector(&rotation, 0, 0, 0);
    quad.tpage = work->textures[1] >> 16;
    quad.clut = work->textures[1];
    setUV4(&quad, 0, 128, 31, 128, 0, 255, 31, 255);
    point = work->beams;
    for (i = 0; i < 16; i++) {
        time = work->time - config->beam_begin - config->beam_spacing * i;
        if (time >= 0 && time < config->beam_duration) {
            for (j = 0; j < 2; point++, j++) {
                s32 red, green, blue, remaining;
                setVector(&position, point->vx, point->vy, point->vz);
                position.vx += work->opponent.vx;
                position.vy += work->opponent.vy;
                position.vz += work->opponent.vz;
                position.vy = -350;
                factor = (time << 12) / config->beam_duration;
                setVector(&scale, 4096, factor, 4096);
                remaining = config->beam_duration - time;
                red = config->beam_red * remaining / config->beam_duration;
                green = config->beam_green * remaining / config->beam_duration;
                blue = config->beam_blue * remaining / config->beam_duration;
                setRGB0(&quad, red, green, blue);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (long *)&quad.x0, (long *)&quad.x1,
                    (long *)&quad.x2, (long *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        } else {
            point += 2;
        }
    }
    PopMatrix();
    work->time += Model_GetFrameStep();
    time = work->time - config->beam_begin;
    if (time >= 0) {
        time -= config->beam_spacing * 16;
        if (time < config->beam_duration) {
            return 4;
        }
        if (work->completed != 0) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 0;
}
