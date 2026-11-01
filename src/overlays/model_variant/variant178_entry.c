#include "../../types.h"
#include "variant178_entry.h"
#include "../../game/screen_projection.h"

/* A square of grid lines fades in around the slot, a strip wall rises from
 * two of its edges, rows of boxes grow upwards at random grid crossings and
 * fade, and full-screen texture bands scroll with wavy row offsets while the
 * slot light ramps down and back up. A negative command draws a frame; any
 * other command sets the work area up and prints its size. */

static const VECTOR gModelVariant178_D1AEC = {4096, 4096, 4096, 0};

s32 ModelVariant178_HandleRequest(u8 *context, s32 command)
{
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_F4 flat;
    GsBOXF dot;
    GsLINE line;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[8];
    SVECTOR center;
    PSXLONG flag, interpolation;
    Model178State *work;
    Model178Config *config;
    GsOT *ot;
    s32 direction, step;
    s32 t, i, j, k, value, finish, otz;
    u8 red, green, blue;
    u8 r, g, b;
    SVECTOR *point, *other;
    s16 *pick, *phase, *speed;
    SVECTOR **face;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant178_D1AEC;
    /* The four side faces of a box: vertices 0-3 are its raised corners and
     * 4-7 the same corners on the floor. */
    {
        SVECTOR *faces[16] = {
            &vertices[0], &vertices[1], &vertices[4], &vertices[5],
            &vertices[1], &vertices[3], &vertices[5], &vertices[7],
            &vertices[3], &vertices[2], &vertices[7], &vertices[6],
            &vertices[2], &vertices[0], &vertices[6], &vertices[4],
        };

        work = (Model178State *)context;
        direction = Model_GetActiveSlotIndex() * 2 - 1;
        step = Model_GetFrameStep();
        if (command >= 0) {
            work->config = &gModelVariant178_D1B60[command % 100];
            config = work->config;
            point = work->grid;
            for (i = 0; i < 13; i++) {
                point->vx = i * config->size / 6 - config->size;
                point->vy = 0;
                point->vz = -direction * config->size;
                point++;
                point->vx = i * config->size / 6 - config->size;
                point->vy = 0;
                point->vz = direction * config->size;
                point++;
            }
            for (i = 0; i < 13; i++) {
                point->vx = -direction * config->size;
                point->vy = 0;
                point->vz = i * config->size / 6 - config->size;
                point++;
                point->vx = direction * config->size;
                point->vy = 0;
                point->vz = i * config->size / 6 - config->size;
                point++;
            }
            pick = work->picks;
            for (i = 0; i < config->box_count * config->box_rows; i++) {
                *pick++ = rand() % 144;
            }
            work->scroll = 0;
            phase = work->phase;
            speed = work->speed;
            for (i = 0; i < GRAPHICS_DEFAULT_HEIGHT; i++) {
                *phase = 0;
#ifdef VERSION_FRENCH
                *speed = config->speed + config->sway * csin(i << 5) / 4096;
#else
                *speed = config->speed + config->sway * csin((i << 13) / GRAPHICS_DEFAULT_HEIGHT) / 4096;
#endif
                phase++;
                speed++;
            }
            work->offset = 0;
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&work->target);
            for (i = 0; i < 3; i++) {
                work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant178_D1B0C[i]);
            }
            work->elapsed = 0;
            work->frames = 0;
            work->completed = 0;
#ifndef VERSION_FRENCH
            printf("%d (0x%x) Byte\n", sizeof(Model178State), sizeof(Model178State));
#endif
            return 0;
        }
        config = work->config;
        ot = func_80058F10();
        base = *(MATRIX *)Model_GetLightSourceMatrix();
        PushMatrix();
        SetPolyFT4(&quad);
        SetPolyF4(&flat);
        dot.attribute = 0x50000000;
        dot.w = 1;
        dot.h = 1;
        line.attribute = 0x50000000;
        center.vx = work->target.vx;
        center.vy = work->target.vy;
        center.vz = work->target.vz;

        t = work->elapsed - config->start;
        if (t >= 0 && t < config->ramp) {
            value = 2048 - t * 6144 / config->ramp;
            Model_SetSlotAmbient(2, value, value, value);
        } else if (t >= config->ramp) {
            value = -4096;
            Model_SetSlotAmbient(2, value, value, value);
        }
        t = work->elapsed - config->box_start - config->box_rows * config->box_interval - config->box_time - config->fade;
        if (t >= 0 && t < config->ramp) {
            value = t * 6144 / config->ramp - 4096;
            Model_SetSlotAmbient(2, value, value, value);
        } else if (t >= config->ramp) {
            value = 2048;
            Model_SetSlotAmbient(2, value, value, value);
        }

        t = work->elapsed - config->start;
        finish = work->elapsed - config->box_start - config->box_rows * config->box_interval - config->box_time;
        if (t >= 0 && finish < config->fade && !(work->frames & 1)) {
            if (t < config->fade) {
                line.r = config->line_color[0] * t / config->fade;
                line.g = config->line_color[1] * t / config->fade;
                line.b = config->line_color[2] * t / config->fade;
            } else if (finish >= 0 && finish < config->fade) {
                line.r = config->line_color[0] * (config->fade - finish) / config->fade;
                line.g = config->line_color[1] * (config->fade - finish) / config->fade;
                line.b = config->line_color[2] * (config->fade - finish) / config->fade;
            } else {
                line.r = config->line_color[0];
                line.g = config->line_color[1];
                line.b = config->line_color[2];
            }
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            setVector(&position, center.vx, center.vy, center.vz);
            position.vy = 0;
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            point = work->grid;
            for (i = 0; i < 26; i++) {
                vertices[0].vx = point[0].vx;
                vertices[0].vy = point[0].vy;
                vertices[0].vz = point[0].vz;
                vertices[0].vx += point[1].vx;
                vertices[0].vy += point[1].vy;
                vertices[0].vz += point[1].vz;
                vertices[0].vx /= 2;
                vertices[0].vy /= 2;
                vertices[0].vz /= 2;
                otz = RotTransPers3(&point[0], &point[1], &vertices[0], (PSXLONG *)&line.x0, (PSXLONG *)&line.x1,
                    (PSXLONG *)&quad.x0, &interpolation, &flag);
                if (otz >= 0 && flag >= 0) {
                    GsSortLine(&line, ot, otz);
                }
                point += 2;
            }
        }

        t = work->elapsed - config->start;
        if (t >= 0 && t < config->fade) {
            quad.tpage = work->texture[0] >> 16;
            quad.clut = work->texture[0];
            SetSemiTrans(&quad, 1);
            setUV4(&quad, 0, 64, 31, 64, 0, 128, 31, 128);
            r = config->strip_color[0] * (config->fade - t) / config->fade;
            g = config->strip_color[1] * (config->fade - t) / config->fade;
            b = config->strip_color[2] * (config->fade - t) / config->fade;
            setVector(&rotation, 0, 0, 0);
            setVector(&position, center.vx, center.vy, center.vz);
            position.vy = 0;
            setVector(&scale, 4096, 4096, 4096);
            setRGB0(&quad, r, g, b);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            point = work->grid;
            for (i = 0; i < 2; i++) {
                for (j = 0; j < 24; j++) {
                    other = point + 2;
                    vertices[0].vx = point->vx;
                    vertices[0].vy = point->vy;
                    vertices[0].vz = point->vz;
                    vertices[1].vx = other->vx;
                    vertices[1].vy = other->vy;
                    vertices[1].vz = other->vz;
                    vertices[2].vx = point->vx;
                    vertices[2].vy = point->vy;
                    vertices[2].vz = point->vz;
                    vertices[3].vx = other->vx;
                    vertices[3].vy = other->vy;
                    vertices[3].vz = other->vz;
                    value = -config->sway * t * 2 / config->fade;
                    value = value < -config->height ? -config->height : value > 0 ? 0 : value;
                    vertices[1].vy = value;
                    vertices[0].vy = value;
                    otz = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    if (otz >= 0 && flag >= 0) {
                        GsSortPoly(&quad, ot, otz);
                    }
                    point++;
                }
                point += 2;
            }
        }

        quad.tpage = work->texture[0] >> 16;
        quad.clut = work->texture[0];
        SetSemiTrans(&quad, 1);
        pick = work->picks;
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&position, center.vx, center.vy, center.vz);
        position.vy = 0;
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < config->box_rows; i++) {
            t = work->elapsed - config->box_start - i * config->box_interval;
            if (t >= 0 && t < config->box_time) {
                r = config->box_color[0] * (config->box_time - t) / config->box_time;
                g = config->box_color[1] * (config->box_time - t) / config->box_time;
                b = config->box_color[2] * (config->box_time - t) / config->box_time;
                setRGB0(&quad, r, g, b);
                setUV4(&quad, 0, 64, 31, 64, 0, t * 128 / config->box_time + 63, 31, t * 128 / config->box_time + 63);
                for (j = 0; j < config->box_count; j++) {
                    point = &work->grid[(s16)(*pick % 12) * 2];
                    other = &work->grid[(s16)(*pick / 12) * 2 + 26];
                    vertices[0].vx = point->vx;
                    vertices[0].vy = -config->height * t / config->box_time;
                    vertices[0].vz = other->vz;
                    vertices[1].vx = point[2].vx;
                    vertices[1].vy = -config->height * t / config->box_time;
                    vertices[1].vz = other->vz;
                    vertices[2].vx = point->vx;
                    vertices[2].vy = -config->height * t / config->box_time;
                    vertices[2].vz = other[2].vz;
                    vertices[3].vx = point[2].vx;
                    vertices[3].vy = -config->height * t / config->box_time;
                    vertices[3].vz = other[2].vz;
                    vertices[4].vx = vertices[0].vx;
                    vertices[4].vy = vertices[0].vy;
                    vertices[4].vz = vertices[0].vz;
                    vertices[5].vx = vertices[1].vx;
                    vertices[5].vy = vertices[1].vy;
                    vertices[5].vz = vertices[1].vz;
                    vertices[6].vx = vertices[2].vx;
                    vertices[6].vy = vertices[2].vy;
                    vertices[6].vz = vertices[2].vz;
                    vertices[7].vx = vertices[3].vx;
                    vertices[7].vy = vertices[3].vy;
                    vertices[7].vz = vertices[3].vz;
                    for (k = 4; k < 8; k++) {
                        vertices[k].vy = 0;
                    }
                    face = faces;
                    for (k = 0; k < 4; k++) {
                        otz = RotAverage4(face[0], face[1], face[2], face[3], (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1,
                            (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3, &interpolation, &flag);
                        if (otz >= 0 && flag >= 0) {
                            GsSortPoly(&quad, ot, otz);
                        }
                        face += 4;
                    }
                    pick++;
                }
            } else {
                pick += config->box_count;
            }
        }

        t = work->elapsed - config->band_start;
        finish = work->elapsed - config->box_start - config->box_rows * config->box_interval - config->box_time - config->fade;
        if (finish < config->ramp && t >= 0) {
            quad.tpage = work->texture[2] >> 16;
            quad.clut = work->texture[2];
            t = work->elapsed - config->band_start;
            if (t >= 0 && t < config->ramp) {
                red = config->band_color[0] * t / config->ramp;
                green = config->band_color[1] * t / config->ramp;
                blue = config->band_color[2] * t / config->ramp;
                SetSemiTrans(&quad, 1);
            } else {
                t = work->elapsed - config->box_start - config->box_rows * config->box_interval - config->box_time - config->fade;
                if (t < 0) {
                    red = config->band_color[0];
                    green = config->band_color[1];
                    blue = config->band_color[2];
                    SetSemiTrans(&quad, 0);
                } else {
                    value = config->ramp - t;
                    red = config->band_color[0] * value / config->ramp;
                    green = config->band_color[1] * value / config->ramp;
                    blue = config->band_color[2] * value / config->ramp;
                    SetSemiTrans(&quad, 1);
                }
            }
            setRGB0(&quad, red, green, blue);
            phase = work->phase;
            for (i = 0; i < GRAPHICS_DEFAULT_HEIGHT; i++) {
                quad.u0 = 0;
                quad.v0 = (work->scroll / 256 + i) % 64;
                quad.u1 = 192;
                quad.v1 = (work->scroll / 256 + i) % 64;
                quad.u2 = 0;
                quad.v2 = (work->scroll / 256 + i) % 64 + 1;
                quad.u3 = 192;
                quad.v3 = (work->scroll / 256 + i) % 64 + 1;
                quad.x0 = *phase / 256 - 64;
                quad.y0 = i;
                quad.x1 = *phase / 256 + 128;
                quad.y1 = i;
                quad.x2 = *phase / 256 - 64;
                quad.y2 = i + 1;
                quad.x3 = *phase / 256 + 128;
                quad.y3 = i + 1;
                Graphics_SubmitTextureWindowPacket((u32 *)&quad, ot, 0xFFF, 0, 0, 64, 64);
                quad.x0 = *phase / 256 + 128;
                quad.y0 = i;
                quad.x1 = *phase / 256 + 320;
                quad.y1 = i;
                quad.x2 = *phase / 256 + 128;
                quad.y2 = i + 1;
                quad.x3 = *phase / 256 + 320;
                quad.y3 = i + 1;
                Graphics_SubmitTextureWindowPacket((u32 *)&quad, ot, 0xFFF, 0, 0, 64, 64);
                phase++;
            }
            phase = work->phase;
            speed = &work->speed[work->offset];
            for (i = 0; i < GRAPHICS_DEFAULT_HEIGHT - work->offset; i++) {
                *phase += *speed;
                *phase %= 16384;
                phase++;
                speed++;
            }
            speed = work->speed;
            for (i = 0; i < work->offset; i++) {
                *phase += *speed;
                *phase %= 16384;
                phase++;
                speed++;
            }
            work->scroll = (work->scroll + (step << 8)) % 16384;
            work->offset = (work->offset + step) % GRAPHICS_DEFAULT_HEIGHT;
        }
        PopMatrix();
        work->elapsed += step;
        work->frames++;
        t = work->elapsed - config->box_start;
        if (t >= 0) {
            t = t - config->box_rows * config->box_interval - config->box_time;
            if (t < 0) {
                return 4;
            }
            if (t >= config->fade + config->ramp) {
                return 2;
            }
            if (work->completed == 0) {
                work->completed++;
                return 1;
            }
        }
        return 0;
    }
}
