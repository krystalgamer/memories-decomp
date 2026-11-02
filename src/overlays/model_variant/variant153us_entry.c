#include "../../types.h"
#include "variant153us_entry.h"

/* The North American build reports the work size once per setup command. */
#ifndef VERSION_FRENCH
#include "../../psyq/stdio.h"
#endif

/* Header 153's entry: a textured 5x5 sheet grows at the followed slot part,
 * then two sheets trail towards an anchor beside it and towards a point
 * between the slots, both spin there, and a white flash closes the effect. */

static const VECTOR gModelVariant153Us_D143C = {4096, 4096, 4096, 0};

s32 ModelVariant153Us_HandleRequest(u8 *context, s32 command)
{
    Model153UsState *work;
    s32 direction;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR target;
    SVECTOR unused[3];
    PSXLONG flag, interpolation;
    Model153UsConfig *config;
    GsOT *ot;
    SVECTOR *point, *previous;
    SVECTOR *G32 *corner;
    s32 step, i, j, k, m, time;
    SVECTOR *anchor;
    POLY_FT4 *poly;
    s32 value, amount;
    u8 red, green, blue;
    s32 rows;
    u8 shade;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant153Us_D143C;
    work = (Model153UsState *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    step = Model_GetFrameStep();
    if (command >= 0) {
        work->rotation.vx = 0;
        work->rotation.vy = 0;
        work->rotation.vz = 0;
        config = work->config = &gModelVariant153Us_D1478[command % 100];
        work->spin[0] = config->spin[0];
        work->spin[1] = config->spin[1];
        work->spin[2] = config->spin[2];
        point = work->grid;
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 5; point++, j++) {
                point->vx = config->half_size * (j << 1) / 4 - config->half_size;
                point->vy = config->half_size * (i << 1) / 4 - config->half_size;
                point->vz = 0;
            }
        }
        corner = work->corners;
        point = work->grid;
        previous = &work->grid[5];
        for (i = 0; i < 4; point++, previous++, i++) {
            for (j = 0; j < 4; corner += 4, point++, previous++, j++) {
                corner[0] = point;
                corner[1] = point + 1;
                corner[2] = previous;
                corner[3] = previous + 1;
            }
        }
        work->trail_index = 0;
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant153Us_D145C[i]);
        }
        work->elapsed = 0;
        work->frames = 0;
        work->completed = 0;
#ifndef VERSION_FRENCH
        printf("%d (0x%x) Byte\n", sizeof(Model153UsState), sizeof(Model153UsState));
#endif
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    PushMatrix();
    SetPolyFT4(&quad);
    SetPolyF4(&flash);
    time = work->elapsed - config->delay;
    if (time < 0) {
        Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&target);
        GsSetLsMatrix(&base);
        previous = work->trail;
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
        point = work->anchor;
        point->vx = matrix.t[0];
        point->vy = matrix.t[1];
        point->vz = matrix.t[2];
        point->vz += direction * config->height;
        for (i = 0; i < 10; previous++, i++) {
            previous->vx = point->vx;
            previous->vy = point->vy;
            previous->vz = point->vz;
            previous->pad = 0;
        }
        point++;
        target.vy = 0;
        for (i = 1; i < 2; point++, i++) {
            point->vx = config->radius * csin(i * 2048) / 4096;
            point->vy = -350;
            point->vz = direction * config->radius * ccos(i * 2048) / 4096;
            point->vx += target.vx;
            point->vy += target.vy;
            point->vz += target.vz;
        }
    } else {
        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        setVector(&rotation, work->rotation.vx, work->rotation.vy, work->rotation.vz);
        if (time < config->grow_duration + config->travel_duration) {
            value = work->frames % 2 * 256 + 4096;
            setVector(&scale, value, value, value);
            if (time < config->grow_duration) {
                red = config->red * time / config->grow_duration;
                green = config->green * time / config->grow_duration;
                blue = config->blue * time / config->grow_duration;
            } else {
                red = config->red;
                green = config->green;
                blue = config->blue;
            }
            setRGB0(&quad, red, green, blue);
            setVector(&position, work->anchor[0].vx, work->anchor[0].vy, work->anchor[0].vz);
            GsSetLsMatrix(&base);
            corner = work->corners;
            value = 32;
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            poly = &quad;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; corner += 4, j++) {
                    depth = RotAverage4(corner[0], corner[1], corner[2], corner[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                        (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                    setUV4(&quad, j * value, i * value, j * value + 31, i * value,
                        j * value, i * value + 31, j * value + 31, i * value + 31);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(poly, ot, (u16)depth);
                    }
                }
            }
        }
        time -= config->grow_duration;
        if (time >= 0 && time < config->travel_duration + 10) {
            anchor = &work->anchor[1];
            for (k = 1; k < 2; anchor++, k++) {
                point = &(work->trail + k * 5)[work->trail_index];
                previous = &(work->trail + k * 5)[(work->trail_index + 4) % 5];
                if (time < config->travel_duration) {
                    point->vx = previous->vx;
                    point->vy = previous->vy;
                    point->vz = previous->vz;
                    point->vx += (anchor->vx - previous->vx) / (config->travel_duration - time) * step;
                    point->vy += (anchor->vy - previous->vy) / (config->travel_duration - time) * step;
                    point->vz += (anchor->vz - previous->vz) / (config->travel_duration - time) * step;
                    point->pad = 1;
                    work->trail_index = (work->trail_index + 1) % 5;
                } else {
                    point->pad = 0;
                    work->trail_index = (work->trail_index + 1) % 5;
                }
                for (m = 0; m < 5; m++) {
                    point = &(work->trail + k * 5)[(work->trail_index + m) % 5];
                    if (point->pad != 0) {
                        value = time - (5 - m) * 2;
                        amount = value * 8192 / config->travel_duration + 4096;
                        amount = amount < 4096 ? 4096 : amount > 0x3000 ? 0x3000 : amount;
                        setVector(&scale, amount, amount, amount);
                        red = config->red * m / 5;
                        green = config->green * m / 5;
                        blue = config->blue * m / 5;
                        setRGB0(&quad, red, green, blue);
                        setVector(&position, point->vx, point->vy, point->vz);
                        GsSetLsMatrix(&base);
                        corner = work->corners;
                        value = 32;
                        RotTrans(&position, (VECTOR *)matrix.t, &flag);
                        RotMatrix(&rotation, &matrix);
                        MulMatrix2(&base, &matrix);
                        ScaleMatrix(&matrix, &scale);
                        GsSetLsMatrix(&matrix);
                        rows = 4;
                        for (i = 0; i < rows; i++) {
                            for (j = 0; j < 4; corner += 4, j++) {
                                depth = RotAverage4(corner[0], corner[1], corner[2], corner[3],
                                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                                    (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                                setUV4(&quad, j * value, i * value, j * value + 31, i * value,
                                    j * value, i * value + 31, j * value + 31, i * value + 31);
                                if (depth >= 0 && flag >= 0) {
                                    GsSortPoly(&quad, ot, (u16)depth);
                                }
                            }
                        }
                    }
                }
            }
        }
        time -= config->travel_duration;
        if (time >= 0 && time < config->spin_duration + config->hold_duration) {
            point = work->anchor;
            for (k = 0; k < 2; point++, k++) {
                if (k > 0 && !(work->frames & 1)) {
                    red = config->red / 2;
                    green = config->green / 2;
                    blue = config->blue / 2;
                    setRGB0(&quad, red, green, blue);
                } else {
                    red = config->red;
                    green = config->green;
                    blue = config->blue;
                    setRGB0(&quad, red, green, blue);
                }
                if (k != 0) {
                    setVector(&scale, 0x3000, 0x3000, 0x3000);
                } else {
                    value = work->frames % 2 * 256 + 4096;
                    setVector(&scale, value, value, value);
                }
                setVector(&position, point->vx, point->vy, point->vz);
                GsSetLsMatrix(&base);
                corner = work->corners;
                value = 32;
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                for (i = 0; i < 4; i++) {
                    for (j = 0; j < 4; corner += 4, j++) {
                        depth = RotAverage4(corner[0], corner[1], corner[2], corner[3],
                            (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                        setUV4(&quad, j * value, i * value, j * value + 31, i * value,
                            j * value, i * value + 31, j * value + 31, i * value + 31);
                        if (depth >= 0 && flag >= 0) {
                            GsSortPoly(&quad, ot, (u16)depth);
                        }
                    }
                }
            }
            if (time < config->spin_duration) {
                work->rotation.vx = (work->rotation.vx + work->spin[0] * time / config->spin_duration) % 4096;
                work->rotation.vy = (work->rotation.vy + work->spin[1] * time / config->spin_duration) % 4096;
                work->rotation.vz = (work->rotation.vz + work->spin[2] * time / config->spin_duration) % 4096;
            } else {
                work->rotation.vx += work->spin[0];
                work->rotation.vy += work->spin[1];
                work->rotation.vz += work->spin[2];
                work->rotation.vx %= 4096;
                work->rotation.vy %= 4096;
                work->rotation.vz %= 4096;
            }
        }
    }
    time = work->elapsed - config->delay - config->grow_duration - config->travel_duration -
        config->spin_duration - config->hold_duration;
    if (time >= 0 && time < config->flash_duration) {
        setXY4(&flash, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
        shade = (config->flash_duration - time) * 255 / config->flash_duration;
        setRGB0(&flash, shade, shade, shade);
        Graphics_SubmitDrawModePacket((u32 *)&flash, ot, 1, 1);
    }
    PopMatrix();
    work->elapsed += step;
    work->frames++;
    time = work->elapsed - config->delay - config->grow_duration - config->travel_duration;
    if (time < 0) {
        return 0;
    }
    time -= config->spin_duration + config->hold_duration;
    if (time < 0) {
        return 4;
    }
    if (time >= config->flash_duration) {
        return 2;
    }
    if (work->completed != 0) {
        return 0;
    }
    work->completed++;
    return 1;
}
