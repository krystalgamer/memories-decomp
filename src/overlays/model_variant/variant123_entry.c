#include "../../types.h"
#include "variant123_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 123's entry: rings of 16 textured quads grow and fade around this
 * slot, a full-screen flash fades out, and then up to 96 quads rise from the
 * command's slot part, cross to the other slot and fade while pushing past it.
 * The return value reports the hit (1), the end (2) or a still-moving entry. */

static const VECTOR D_8013C340 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model123State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[9];
    DVECTOR screen[34];
    u16 depths[34], projection[34], flags[34];
    PSXLONG flag, interpolation;
    s32 side;
    GsOT *ot;
    Model123Config *config;
    SVECTOR *point;
    s32 i, time, rise, fall, value, shade;
    PSXLONG depth;
    s16 red, green, blue;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C340;
    work = (Model123State *)context;
    side = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C388[command];
        point = work->points;
        for (i = 0; i < config->count; i++) {
            point->vx = 0;
            point->vy = 0;
            point->vz = 0;
        }
        Model_CopySlotPosition(Model_GetActiveSlotIndex(), (u16 *)&work->self);
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->other);
        point = work->ring;
        for (i = 0; i < 17; i++) {
            point->vx = config->radius * ccos(i << 8) / 4096;
            point->vy = -config->width / 2;
            point->vz = config->radius * csin(i << 8) / 4096;
            point++;
        }
        value = config->radius - config->width;
        for (i = 0; i < 17; i++) {
            point->vx = value * ccos(i << 8) / 4096;
            point->vy = 0;
            point->vz = value * csin(i << 8) / 4096;
            point++;
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C350[i]);
        }
        work->elapsed = 0;
        work->completed = 0;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < config->ring_delay) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    SetSemiTrans(&quad, 1);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    setVector(&rotation, 0, 0, 0);
    setVector(&position, work->self.vx, work->self.vy, work->self.vz);
    position.vy = config->ring_y;
    value = work->elapsed - config->ring_delay;
    for (time = 0; time < config->ring_span; time += config->ring_spacing) {
        if (value >= time && value < time + config->ring_duration) {
            rise = value - time;
            shade = (rise << 12) / config->ring_duration;
            setVector(&scale, shade, shade, shade);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->ring, screen, depths, projection, flags, 34);
            red = config->ring_red * (config->ring_duration - rise) / config->ring_duration;
            green = config->ring_green * (config->ring_duration - rise) / config->ring_duration;
            blue = config->ring_blue * (config->ring_duration - rise) / config->ring_duration;
            setRGB0(&quad, red, green, blue);
            for (i = 0; i < 16; i++) {
                shade = i + 17;
                setXY4(&quad, screen[i].vx, screen[i].vy, screen[i + 1].vx, screen[i + 1].vy,
                       screen[shade].vx, screen[shade].vy, screen[i + 18].vx, screen[i + 18].vy);
                depth = AverageZ4(depths[i], depths[i + 1], depths[i + 17], depths[i + 18]);
                flag = (flags[i] | flags[i + 1] | flags[i + 17] | flags[i + 18]) & 0x20;
                setUV4(&quad, i * 16, 64, i * 16 + 15, 64, i * 16, 95, i * 16 + 15, 95);
                if (depth >= 0 && flag == 0) {
                    GsSortPoly(&quad, ot, (u16)depth);
                }
            }
        }
    }
    time = work->elapsed - config->flash_delay;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        shade = (config->flash_duration - time) * 255;
        shade = shade / config->flash_duration;
        setRGB0(&flash, shade, shade, shade);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    setVector(&vertices[0], -config->half_size, -config->half_size, 0);
    setVector(&vertices[1], 0, -config->half_size, 0);
    setVector(&vertices[2], config->half_size, -config->half_size, 0);
    setVector(&vertices[3], -config->half_size, 0, 0);
    setVector(&vertices[4], 0, 0, 0);
    setVector(&vertices[5], config->half_size, 0, 0);
    setVector(&vertices[6], -config->half_size, config->half_size, 0);
    setVector(&vertices[7], 0, config->half_size, 0);
    setVector(&vertices[8], config->half_size, config->half_size, 0);
    quad.tpage = work->texture[0] >> 16;
    quad.clut = work->texture[0];
    setUV4(&quad, 0, 0, 63, 0, 0, 63, 63, 63);
    point = work->points;
    for (i = 0; i < config->count; i++) {
        time = work->elapsed - config->move_delay - config->spacing * i;
        if (time < 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
            point->vx = matrix.t[0];
            point->vy = -350;
            point->vz = matrix.t[2];
        }
        rise = time - config->duration - config->distance * 2 * config->duration / (450 - config->distance);
        fall = rise - config->duration;
        if (time > config->duration && rise < 0) {
            point->vz = side * config->distance;
        }
        setVector(&position, point->vx, point->vy, point->vz);
        if (time >= 0 && time < config->duration) {
            value = (time << 12) / config->duration;
            point->vy += (-350 - point->vy) / (config->duration - time);
            point->vz += (-side * config->distance - point->vz) / (config->duration - time);
            red = config->red * (config->duration - time) / config->duration;
            green = config->green * (config->duration - time) / config->duration;
            blue = config->blue * (config->duration - time) / config->duration;
            setRGB0(&quad, red, green, blue);
        }
        if (rise >= 0 && rise < config->duration) {
            value = ((config->duration - rise) << 12) / config->duration;
            point->vx += -point->vx / (config->duration - rise);
            point->vy = -350;
            point->vz += (work->other.vz - point->vz) / (config->duration - rise);
            red = config->red * rise / config->duration;
            green = config->green * rise / config->duration;
            blue = config->blue * rise / config->duration;
            setRGB0(&quad, red, green, blue);
        }
        if (fall >= 0 && fall < config->duration) {
            value = (fall << 12) / config->duration;
            point->vx += -point->vx / (config->duration - fall);
            point->vy = -350;
            point->vz += (work->other.vz + side * config->push - point->vz) / (config->duration - fall);
            red = config->red * (config->duration - fall) / config->duration / 2;
            green = config->green * (config->duration - fall) / config->duration / 2;
            blue = config->blue * (config->duration - fall) / config->duration / 2;
            setRGB0(&quad, red, green, blue);
        }
        if ((time >= 0 && time < config->duration) || (rise >= 0 && rise < config->duration) ||
            (fall >= 0 && fall < config->duration)) {
            setVector(&scale, value, value, value);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[3], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            depth = RotAverage4(&vertices[2], &vertices[1], &vertices[5], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            depth = RotAverage4(&vertices[6], &vertices[7], &vertices[3], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
            depth = RotAverage4(&vertices[8], &vertices[7], &vertices[5], &vertices[4],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(&quad, ot, (u16)depth);
            }
        }
        point++;
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    rise = work->elapsed - config->move_delay - config->duration -
           config->distance * 2 * config->duration / (450 - config->distance);
    fall = rise - config->duration - config->spacing * config->count;
    if (rise < config->duration) {
        return 0;
    }
    if (fall >= config->duration) {
        if (work->completed != 0) {
            return 2;
        }
        work->completed = 1;
        return 1;
    }
    return 4;
}
