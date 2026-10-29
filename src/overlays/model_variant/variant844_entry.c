#include "../../types.h"
#include "variant844_entry.h"
#include "../../game/screen_projection.h"

/* Header 844's entry: two ring-buffer trails that follow four parts of the
 * slot and fade towards their tails, then two bursts in front of the slot,
 * each with animated sparks, animated flares and a four-point star at every
 * spark or flare that grows and fades out. Once the ray time is reached and
 * the ray colour is not yet black, a fan of 32 randomly oriented,
 * semi-transparent rays grows from the burst point with a four-point star at
 * its centre, both fading out together. The first trail's fade clears the
 * green channel when the red one is spent, as retail does. */

static const VECTOR D_8013D5C8 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model844State *work;
    MATRIX base, matrix;
    POLY_G4 gouraud;
    POLY_FT4 burst;
    POLY_FT4 star;
    POLY_GT4 ray;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    s32 direction, step;
    GsOT *ot;
    POLY_FT4 *packet;
    POLY_GT4 *ray_packet;
    s32 slot;
    SVECTOR *point, *other, *third, *fourth;
    s32 i, j, k;
    s32 y0;
    u16 side;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D5C8;
    work = (Model844State *)context;
    packet = &star;
    ray_packet = &ray;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D62C[command];
        point = work->trail[0];
        other = work->trail[1];
        third = work->trail[2];
        fourth = work->trail[3];
        for (i = 0; i < 64; i++) {
            setVector(point, 0, 0, 0);
            point++;
            setVector(other, 0, 0, 0);
            other++;
            setVector(third, 0, 0, 0);
            third++;
            setVector(fourth, 0, 0, 0);
            fourth++;
            for (j = 0; j < 2; j++) {
                work->colors[j][i].r = work->config->trail_r;
                work->colors[j][i].g = work->config->trail_g;
                work->colors[j][i].b = work->config->trail_b;
            }
        }
        point = work->sparks;
        for (i = 0; i < work->config->spark_max; point++, i++) {
            point->vx = work->config->speed * ccos(rand() % 4096) / 4096;
            point->vy = work->config->speed * csin(rand() % 4096) / 4096;
            point->vz = 0;
            work->spark_frames[0][i] = 0;
            work->spark_frames[1][i] = 0;
        }
        for (i = 0; i < work->config->flare_max; i++) {
            setVector(&work->flares[i], work->config->speed * ccos(rand() % 4096) / 4096,
                work->config->speed * csin(rand() % 4096) / 4096, 0);
            work->flare_frames[0][i] = 0;
            work->flare_frames[1][i] = 0;
            work->star_scale[0][i] = 0x1000;
            work->star_scale[1][i] = 0x1000;
            work->colors[2][i].r = work->config->star_r;
            work->colors[2][i].g = work->config->star_g;
            work->colors[2][i].b = work->config->star_b;
            work->colors[3][i].r = work->config->star_r;
            work->colors[3][i].g = work->config->star_g;
            work->colors[3][i].b = work->config->star_b;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->rays[i], rand() % 4096, rand() % 4096, rand() % 4096);
        }
        work->ray_color.r = work->config->star_r;
        work->ray_color.g = work->config->star_g;
        work->ray_color.b = work->config->star_b;
        work->burst_tpage = 0xAE;
#ifdef VERSION_FRENCH
        work->burst_clut = 0x3D28;
#else
        work->burst_clut = 0x3D20;
#endif
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D5D8[i]);
        }
        work->spark_counts[0] = 0;
        work->spark_counts[1] = 0;
        work->flare_counts[0] = 0;
        work->flare_counts[1] = 0;
        work->head[0] = 0;
        work->head[1] = 0;
        work->level = 2048;
        work->phase = 0;
        work->drawing[0] = 0;
        work->drawing[1] = 0;
        work->elapsed = 0;
        work->ray_scale = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    setPolyFT4(packet);
    setPolyGT4(ray_packet);
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 2) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
            work->phase = 3;
            work->level = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->first_start) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->first_start && work->config->first_end >= work->elapsed) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->first_part), &matrix);
        point = work->trail[0];
        setVector(&point[work->head[0]], matrix.t[0], matrix.t[1], matrix.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->second_part), &matrix);
        other = work->trail[1];
        setVector(&other[work->head[0]], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->head[0] = (work->head[0] + 1) % 64;
        if (work->head[0] >= 2 && work->drawing[0] == 0) {
            work->drawing[0] = 1;
        }
    }
    if (work->elapsed >= work->config->second_start && work->config->second_end >= work->elapsed) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->third_part), &matrix);
        point = work->trail[2];
        setVector(&point[work->head[1]], matrix.t[0], matrix.t[1], matrix.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->fourth_part), &matrix);
        other = work->trail[3];
        setVector(&other[work->head[1]], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->head[1] = (work->head[1] + 1) % 64;
        if (work->head[1] >= 2 && work->drawing[1] == 0) {
            work->drawing[1] = 1;
        }
    }
    if (work->drawing[0] > 0) {
        point = work->trail[0];
        other = work->trail[1];
        setPolyG4(&gouraud);
        setRGB2(&gouraud, 0, 0, 0);
        setRGB3(&gouraud, 0, 0, 0);
        for (i = work->head[0] - 1, k = 1; i > 0; i--) {
            setRGB0(&gouraud, work->colors[0][i].r / k, work->colors[0][i].g / k, work->colors[0][i].b / k);
            setRGB1(&gouraud, work->colors[0][i].r / (k + 1), work->colors[0][i].g / (k + 1), work->colors[0][i].b / (k + 1));
            k++;
            for (j = 0; j < work->config->layer_count; j++) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -j * work->config->layer_step, 0);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&point[i], &point[i - 1], &other[i], &other[i - 1],
                    (PSXLONG *)&gouraud.x0, (PSXLONG *)&gouraud.x1,
                    (PSXLONG *)&gouraud.x2, (PSXLONG *)&gouraud.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gouraud, ot, (u16)depth, 1);
                }
            }
            /* Retail clears the green channel when the red one has faded. */
            if (work->colors[0][i].r > 15) work->colors[0][i].r -= 15;
            else work->colors[0][i].g = 0;
            if (work->colors[0][i].g > 15) work->colors[0][i].g -= 15;
            else work->colors[0][i].g = 0;
            if (work->colors[0][i].b > 15) work->colors[0][i].b -= 15;
            else work->colors[0][i].b = 0;
        }
    }
    if (work->drawing[1] > 0) {
        point = work->trail[2];
        other = work->trail[3];
        setPolyG4(&gouraud);
        setRGB2(&gouraud, 0, 0, 0);
        setRGB3(&gouraud, 0, 0, 0);
        for (i = work->head[1] - 1, k = 1; i > 0; i--) {
            setRGB0(&gouraud, work->colors[1][i].r / k, work->colors[1][i].g / k, work->colors[1][i].b / k);
            setRGB1(&gouraud, work->colors[1][i].r / (k + 1), work->colors[1][i].g / (k + 1), work->colors[1][i].b / (k + 1));
            k++;
            for (j = 0; j < work->config->layer_count; j++) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -j * work->config->layer_step, 0);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                MulMatrix2(&base, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&point[i], &point[i - 1], &other[i], &other[i - 1],
                    (PSXLONG *)&gouraud.x0, (PSXLONG *)&gouraud.x1,
                    (PSXLONG *)&gouraud.x2, (PSXLONG *)&gouraud.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&gouraud, ot, (u16)depth, 1);
                }
            }
            if (work->colors[1][i].r > 15) work->colors[1][i].r -= 8;
            else work->colors[1][i].r = 0;
            if (work->colors[1][i].g > 15) work->colors[1][i].g -= 8;
            else work->colors[1][i].g = 0;
            if (work->colors[1][i].b > 15) work->colors[1][i].b -= 8;
            else work->colors[1][i].b = 0;
        }
    }
    if (work->elapsed >= work->config->burst_start) {
        setPolyFT4(&burst);
        setRGB0(&burst, 255, 0, 0);
        burst.tpage = work->texture[0] >> 16;
        burst.clut = work->texture[0];
        setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
        setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
        point = work->sparks;
        for (i = 0; i < work->spark_counts[0]; point++, i++) {
            if (work->spark_frames[0][i] < 8) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, point);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2,
                    (PSXLONG *)&burst.x3, &interpolation, &flag);
                setUV4(&burst, work->spark_frames[0][i] * 32, 64, (work->spark_frames[0][i] * 32) + 31, 64,
                    work->spark_frames[0][i] * 32, 95, (work->spark_frames[0][i] * 32) + 31, 95);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
                }
                work->spark_frames[0][i]++;
            }
        }
        work->spark_counts[0] += step;
        if (work->config->spark_max < work->spark_counts[0]) {
            work->spark_counts[0] = work->config->spark_max;
        }
        setRGB0(&burst, 255, 128, 128);
        burst.tpage = work->texture[2] >> 16;
        burst.clut = work->texture[2];
        setVector(&vertices[0], -work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[1], work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[2], -work->config->flare_width, work->config->flare_height, 0);
        setVector(&vertices[3], work->config->flare_width, work->config->flare_height, 0);
        point = work->flares;
        for (i = 0; i < work->flare_counts[0]; point++, i++) {
            if (work->flare_frames[0][i] < 4) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, point);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2,
                    (PSXLONG *)&burst.x3, &interpolation, &flag);
                setUV4(&burst, (work->flare_frames[0][i] * 32) + 128, 0, (work->flare_frames[0][i] * 32) + 159, 0,
                    (work->flare_frames[0][i] * 32) + 128, 63, (work->flare_frames[0][i] * 32) + 159, 63);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
                }
                work->flare_frames[0][i]++;
            }
        }
        point = work->sparks;
        packet->tpage = work->burst_tpage;
        packet->clut = work->burst_clut;
        setUV4(packet, 64, 0, 127, 0, 64, 63, 127, 63);
        for (i = 0; i < work->flare_counts[0]; point++, i++) {
            if (work->colors[2][i].r != 0 || work->colors[2][i].g != 0 || work->colors[2][i].b != 0) {
                setRGB0(packet, work->colors[2][i].r, work->colors[2][i].g, work->colors[2][i].b);
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, point);
                setVector(&scale, work->star_scale[0][i], work->star_scale[0][i], work->star_scale[0][i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                side = 1;
                for (j = 0; j < 4; j++) {
                    s32 next_side = -(s16)side;
                    s32 first_x = -(s16)next_side * work->config->star_size;
                    side = next_side;
                    vertices[0].vx = first_x;
                    y0 = work->config->star_size;
                    if (j - 2 >= 0) y0 = -y0;
                    vertices[0].vy = y0;
                    vertices[0].vz = 0;
                    vertices[1].vx = 0;
                    y0 = work->config->star_size;
                    if (j - 2 >= 0) y0 = -y0;
                    vertices[1].vy = y0;
                    vertices[1].vz = 0;
                    setVector(&vertices[2], -(s16)side * work->config->star_size, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                        (PSXLONG *)&packet->x3, &interpolation, &flag);
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)packet, ot, (u16)depth, 1);
                    }
                }
                if (work->star_scale[0][i] > 30000) work->star_scale[0][i] += 0x40;
                else work->star_scale[0][i] += 0x2000;
                if (work->colors[2][i].r > 15) work->colors[2][i].r -= 15;
                else work->colors[2][i].r = 0;
                if (work->colors[2][i].g > 15) work->colors[2][i].g -= 15;
                else work->colors[2][i].g = 0;
                if (work->colors[2][i].b > 15) work->colors[2][i].b -= 15;
                else work->colors[2][i].b = 0;
            }
        }
        work->flare_counts[0] += step;
        if (work->config->flare_max < work->flare_counts[0]) {
            work->flare_counts[0] = work->config->flare_max;
        }
    }
    if (work->elapsed >= work->config->second_burst) {
        setVector(&scale, 4096, 4096, 4096);
        setPolyFT4(&burst);
        setRGB0(&burst, 255, 0, 0);
        burst.tpage = work->texture[0] >> 16;
        burst.clut = work->texture[0];
        setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
        setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
        setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
        point = work->sparks;
        for (i = 0; i < work->spark_counts[1]; point++, i++) {
            if (work->spark_frames[1][i] < 8) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, point);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2,
                    (PSXLONG *)&burst.x3, &interpolation, &flag);
                setUV4(&burst, work->spark_frames[1][i] * 32, 64, (work->spark_frames[1][i] * 32) + 31, 64,
                    work->spark_frames[1][i] * 32, 95, (work->spark_frames[1][i] * 32) + 31, 95);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
                }
                work->spark_frames[1][i]++;
            }
        }
        work->spark_counts[1] += step;
        if (work->config->spark_max < work->spark_counts[1]) {
            work->spark_counts[1] = work->config->spark_max;
        }
        setRGB0(&burst, 255, 128, 128);
        burst.tpage = work->texture[2] >> 16;
        burst.clut = work->texture[2];
        setVector(&vertices[0], -work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[1], work->config->flare_width, -work->config->flare_height, 0);
        setVector(&vertices[2], -work->config->flare_width, work->config->flare_height, 0);
        setVector(&vertices[3], work->config->flare_width, work->config->flare_height, 0);
        point = work->flares;
        for (i = 0; i < work->flare_counts[1]; point++, i++) {
            if (work->flare_frames[1][i] < 4) {
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, point);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2,
                    (PSXLONG *)&burst.x3, &interpolation, &flag);
                setUV4(&burst, (work->flare_frames[1][i] * 32) + 128, 0, (work->flare_frames[1][i] * 32) + 159, 0,
                    (work->flare_frames[1][i] * 32) + 128, 63, (work->flare_frames[1][i] * 32) + 159, 63);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
                }
                work->flare_frames[1][i]++;
            }
        }
        point = work->flares;
        packet->tpage = work->burst_tpage;
        packet->clut = work->burst_clut;
        setUV4(packet, 64, 0, 127, 0, 64, 63, 127, 63);
        for (i = 0; i < work->flare_counts[1]; point++, i++) {
            if (work->colors[3][i].r != 0 || work->colors[3][i].g != 0 || work->colors[3][i].b != 0) {
                setRGB0(packet, work->colors[3][i].r, work->colors[3][i].g, work->colors[3][i].b);
                GsSetLsMatrix(&base);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, point);
                setVector(&scale, work->star_scale[1][i], work->star_scale[1][i], work->star_scale[1][i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                side = 1;
                for (j = 0; j < 4; j++) {
                    s32 next_side = -(s16)side;
                    s32 first_x = -(s16)next_side * work->config->star_size;
                    side = next_side;
                    vertices[0].vx = first_x;
                    y0 = work->config->star_size;
                    if (j - 2 >= 0) y0 = -y0;
                    vertices[0].vy = y0;
                    vertices[0].vz = 0;
                    vertices[1].vx = 0;
                    y0 = work->config->star_size;
                    if (j - 2 >= 0) y0 = -y0;
                    vertices[1].vy = y0;
                    vertices[1].vz = 0;
                    setVector(&vertices[2], -(s16)side * work->config->star_size, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                        (PSXLONG *)&packet->x3, &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)packet, ot, (u16)depth, 1);
                    }
                }
                if (work->star_scale[1][i] > 30000) work->star_scale[1][i] += 0x40;
                else work->star_scale[1][i] += 0x2000;
                if (work->colors[3][i].r > 15) work->colors[3][i].r -= 15;
                else work->colors[3][i].r = 0;
                if (work->colors[3][i].g > 15) work->colors[3][i].g -= 15;
                else work->colors[3][i].g = 0;
                if (work->colors[3][i].b > 15) work->colors[3][i].b -= 15;
                else work->colors[3][i].b = 0;
            }
        }
        work->flare_counts[1] += step;
        if (work->config->flare_max < work->flare_counts[1]) {
            work->flare_counts[1] = work->config->flare_max;
        }
    }
    if (work->elapsed >= work->config->ray_start && (*(s32 *)&work->ray_color & 0xFFFFFF)) {
        ray_packet->tpage = work->burst_tpage;
        ray_packet->clut = work->burst_clut;
        setUV4(ray_packet, 0, 128, 31, 128, 0, 255, 31, 255);
        setRGB0(ray_packet, 0, 0, 0);
        setRGB1(ray_packet, 0, 0, 0);
        setRGB2(ray_packet, work->ray_color.r, work->ray_color.g, work->ray_color.b);
        setRGB3(ray_packet, work->ray_color.r, work->ray_color.g, work->ray_color.b);
        setVector(&vertices[0], -4, -160, 0);
        setVector(&vertices[1], 4, -160, 0);
        setVector(&vertices[2], -12, 0, 0);
        setVector(&vertices[3], 12, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        setVector(&scale, 4096, work->ray_scale, 4096);
        setSemiTrans(ray_packet, 1);
        for (i = 0; i < 32; i++) {
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&work->rays[i], &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&ray_packet->x0, (PSXLONG *)&ray_packet->x1, (PSXLONG *)&ray_packet->x2,
                (PSXLONG *)&ray_packet->x3, &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(ray_packet, ot, (u16)depth);
            }
        }
        setVector(&rotation, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        side = 1;
        setUV4(packet, 64, 0, 127, 0, 64, 63, 127, 63);
        setRGB0(packet, work->ray_color.r, work->ray_color.g, work->ray_color.b);
        for (j = 0; j < 4; j++) {
            s32 next_side = -(s16)side;
            s32 first_x = -(s16)next_side * 128;
            side = next_side;
            vertices[0].vx = first_x;
            if (j - 2 >= 0) vertices[0].vy = -128;
            else vertices[0].vy = 128;
            vertices[0].vz = 0;
            vertices[1].vx = 0;
            if (j - 2 >= 0) vertices[1].vy = -128;
            else vertices[1].vy = 128;
            vertices[1].vz = 0;
            setVector(&vertices[2], -(s16)side * 128, 0, 0);
            setVector(&vertices[3], 0, 0, 0);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                (PSXLONG *)&packet->x3, &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)packet, ot, (u16)depth, 1);
            }
        }
        if (work->ray_scale >= 0x2000) work->ray_scale += 0x100;
        else work->ray_scale += 0x800;
        if (work->ray_color.r > 15) work->ray_color.r -= 15;
        else work->ray_color.r = 0;
        if (work->ray_color.g > 15) work->ray_color.g -= 15;
        else work->ray_color.g = 0;
        if (work->ray_color.b > 15) work->ray_color.b -= 15;
        else work->ray_color.b = 0;
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 0 && work->elapsed >= work->config->burst_start) {
        work->phase = 1;
        return 4;
    }
    if (work->phase == 1 && work->elapsed >= work->config->second_burst) {
        work->phase = 2;
        return 1;
    }
    if (work->phase == 3 && work->config->end < work->elapsed) {
        return 2;
    }
    return 0;
}
