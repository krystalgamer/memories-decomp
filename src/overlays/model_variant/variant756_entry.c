#include "../../types.h"
#include "variant756_entry.h"
#include "../../game/screen_projection.h"

/* PAL images load the burst palette from VRAM x = 640. */
#ifndef MODEL_VARIANT756_CLUT_X
#define MODEL_VARIANT756_CLUT_X 512
#endif

/* Header 756's entry, an extension of the header-766 entry: the same gouraud
 * trail, stars, sparks and light columns, plus a cloud of animated smoke
 * puffs, a debris burst and a ring of embers in front of the slot. The smoke
 * texture row is taken from the second puff's frame, as retail does. */

static const VECTOR D_8013D000 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model756State *work;
    MATRIX base, matrix;
    POLY_G4 gouraud;
    POLY_FT4 burst;
    POLY_GT4 flash;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    POLY_G4 *gouraud_packet;
    POLY_FT4 *burst_packet;
    POLY_GT4 *flash_packet;
    SVECTOR *point, *other;
    s32 i, k;
    s32 y0;
    u16 side;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013D000;
    work = (Model756State *)context;
    gouraud_packet = &gouraud;
    burst_packet = &burst;
    flash_packet = &flash;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D064[command];
        point = work->trail_first;
        other = work->trail_second;
        for (i = 0; i < 32; point++, other++, i++) {
            setVector(point, 0, 0, 0);
            setVector(other, 0, 0, 0);
        }
        work->trail_count = 0;
        work->elapsed = 0;
        work->unknown_CB4 = 0;
        work->phase = 0;
        point = work->rotations;
        for (i = 0; i < work->config->rotation_count; point++, i++) {
            point->vx = (rand() - rand()) % 4096;
            point->vy = (rand() - rand()) % 4096;
            point->vz = (rand() - rand()) % 4096;
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->positions[i], 0, 0, 0);
            setVector(&work->velocities[i], work->config->speed * ((rand() - rand()) % 256) / 256,
                work->config->speed * ((rand() - rand()) % 256) / 256,
                work->config->speed * ((rand() - rand()) % 256) / 256);
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->debris[i], 0, 0, 0);
            setVector(&work->debris_velocities[i], work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                work->config->debris_speed * ((rand() - rand()) % 256) / 256);
        }
        work->debris_frame = 0;
        for (i = 0; i < 64; i++) {
            setVector(&work->smoke[i], work->config->smoke_speed * ((rand() - rand()) % 256) / 256,
                work->config->smoke_speed * ((rand() - rand()) % 256) / 256,
                ((u16)work->config->smoke_speed >> 2) * ((rand() - rand()) % 256) / 256);
            work->frames[i] = 0;
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->embers[i], 0, 0, 0);
            setVector(&work->ember_velocities[i], work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                work->config->ember_speed * ((rand() - rand()) % 256) / 256);
        }
        work->trail_color.rgb.r = work->config->trail_r;
        work->trail_color.rgb.g = work->config->trail_g;
        work->trail_color.rgb.b = work->config->trail_b;
        work->burst_color.rgb.r = work->config->burst_r;
        work->burst_color.rgb.g = work->config->burst_g;
        work->burst_color.rgb.b = work->config->burst_b;
        work->burst_tpage = 0xAE;
        work->burst_clut = (244 << 6) | (MODEL_VARIANT756_CLUT_X >> 4);
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013D010[i]);
        }
        work->burst_scale = 0;
        work->level = 2048;
        work->smoke_count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
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
    if (work->elapsed < work->config->initial_delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->trail_duration && work->trail_count < 32) {
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->first_part), &matrix);
        point = work->trail_first;
        setVector(&point[work->trail_count], matrix.t[0], matrix.t[1], matrix.t[2]);
        GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->second_part), &matrix);
        other = work->trail_second;
        setVector(&other[work->trail_count], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->trail_count++;
    }
    point = work->trail_first;
    other = work->trail_second;
    setPolyG4(gouraud_packet);
    setPolyGT4(flash_packet);
    setPolyFT4(burst_packet);
    setRGB2(gouraud_packet, 0, 0, 0);
    setRGB3(gouraud_packet, 0, 0, 0);
    GsSetLsMatrix(&base);
    setVector(&position, 0, 0, 0);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    if (work->trail_count >= 2 && (work->trail_color.word & 0xFFFFFF)) {
        for (i = work->trail_count - 1, k = 1; i > 0; k++, i--) {
            depth = RotAverage4(&point[i], &point[i - 1], &other[i], &other[i - 1],
                (PSXLONG *)&gouraud_packet->x0, (PSXLONG *)&gouraud_packet->x1,
                (PSXLONG *)&gouraud_packet->x2, (PSXLONG *)&gouraud_packet->x3, &interpolation, &flag);
            setRGB0(gouraud_packet, work->trail_color.rgb.r / k, work->trail_color.rgb.g / k, work->trail_color.rgb.b / k);
            setRGB1(gouraud_packet, work->trail_color.rgb.r / (k + 1), work->trail_color.rgb.g / (k + 1),
                work->trail_color.rgb.b / (k + 1));
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)gouraud_packet, ot, (u16)depth, 1);
            }
        }
        if (work->phase > 0) {
            if (work->trail_color.rgb.r > 15) work->trail_color.rgb.r -= 15;
            else work->trail_color.rgb.r = 0;
            if (work->trail_color.rgb.g > 15) work->trail_color.rgb.g -= 15;
            else work->trail_color.rgb.g = 0;
            if (work->trail_color.rgb.b > 15) work->trail_color.rgb.b -= 15;
            else work->trail_color.rgb.b = 0;
        }
    }
    if (work->phase > 0) {
        setVector(&scale, 4096, 4096, 4096);
        setVector(&rotation, 0, 0, 0);
        setVector(&position, 0, -350, direction * 450);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        burst_packet->tpage = work->texture[1] >> 16;
        burst_packet->clut = work->texture[1];
        setRGB0(burst_packet, 128, 128, 128);
        setSemiTrans(burst_packet, 1);
        for (i = 0; i < work->smoke_count; i++) {
            if (work->frames[i] < 6) {
                /* Retail takes the texture row from the second puff's frame for every puff. */
                setUV4(burst_packet, work->frames[i] % 4 * 32, ((work->frames[1] >> 2) << 5) + 32,
                    (work->frames[i] % 4 * 32) + 31, ((work->frames[1] >> 2) << 5) + 32,
                    work->frames[i] % 4 * 32, ((work->frames[1] >> 2) << 5) + 63,
                    (work->frames[i] % 4 * 32) + 31, ((work->frames[1] >> 2) << 5) + 63);
                setVector(&vertices[0], -work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&vertices[1], work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&vertices[2], -work->config->smoke_size, work->config->smoke_size, 0);
                setVector(&vertices[3], work->config->smoke_size, work->config->smoke_size, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->smoke[i]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst_packet->x0, (PSXLONG *)&burst_packet->x1, (PSXLONG *)&burst_packet->x2,
                    (PSXLONG *)&burst_packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(burst_packet, ot, (u16)depth);
                }
                work->frames[i]++;
            }
        }
        work->smoke_count += work->config->smoke_rate;
        if (work->config->smoke_max < work->smoke_count) {
            work->smoke_count = work->config->smoke_max;
        }
        if (work->debris_frame < 24) {
            burst_packet->tpage = work->texture[0] >> 16;
            burst_packet->clut = work->texture[0];
            setRGB0(burst_packet, 128, 128, 128);
            setUV4(burst_packet, (u16)(work->debris_frame / 3) * 32, 0, ((u16)(work->debris_frame / 3) * 32) + 31, 0,
                (u16)(work->debris_frame / 3) * 32, 31, ((u16)(work->debris_frame / 3) * 32) + 31, 31);
            setSemiTrans(burst_packet, 1);
            for (i = 0; i < work->config->debris_count; i++) {
                setVector(&vertices[0], -work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[1], work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[2], -work->config->debris_size, work->config->debris_size, 0);
                setVector(&vertices[3], work->config->debris_size, work->config->debris_size, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->debris[i]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst_packet->x0, (PSXLONG *)&burst_packet->x1, (PSXLONG *)&burst_packet->x2,
                    (PSXLONG *)&burst_packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(burst_packet, ot, (u16)depth);
                }
                addVector(&work->debris[i], &work->debris_velocities[i]);
            }
            work->debris_frame++;
        }
        if (work->phase > 0 && (work->burst_color.word & 0xFFFFFF)) {
            burst_packet->tpage = work->texture[2] >> 16;
            burst_packet->clut = work->texture[2];
            setRGB0(burst_packet, work->burst_color.rgb.r / 2, work->burst_color.rgb.g / 2, work->burst_color.rgb.b / 2);
            setUV4(burst_packet, 128, 64, 191, 64, 128, 127, 191, 127);
            setSemiTrans(burst_packet, 1);
            for (i = 0; i < work->config->ember_count; i++) {
                setVector(&vertices[0], -work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[1], work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[2], -work->config->ember_size, work->config->ember_size, 0);
                setVector(&vertices[3], work->config->ember_size, work->config->ember_size, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->embers[i]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst_packet->x0, (PSXLONG *)&burst_packet->x1, (PSXLONG *)&burst_packet->x2,
                    (PSXLONG *)&burst_packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(burst_packet, ot, (u16)depth);
                }
                addVector(&work->embers[i], &work->ember_velocities[i]);
            }
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, work->burst_scale, work->burst_scale, work->burst_scale);
            setVector(&position, 0, -350, direction * 450);
            burst_packet->tpage = work->burst_tpage;
            burst_packet->clut = work->burst_clut;
            setUV4(burst_packet, 64, 0, 127, 0, 64, 63, 127, 63);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            saved = matrix;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setRGB0(burst_packet, work->burst_color.rgb.r / 2, work->burst_color.rgb.g / 2, work->burst_color.rgb.b / 2);
            side = 1;
            for (i = 0; i < 4; i++) {
                s32 next_side = -(s16)side;
                s32 first_x = -(s16)next_side * work->config->star_size;
                side = next_side;
                vertices[0].vx = first_x;
                y0 = work->config->star_size;
                if (i - 2 >= 0) y0 = -y0;
                vertices[0].vy = y0;
                vertices[0].vz = 0;
                vertices[1].vx = 0;
                y0 = work->config->star_size;
                if (i - 2 >= 0) y0 = -y0;
                vertices[1].vy = y0;
                vertices[1].vz = 0;
                setVector(&vertices[2], -(s16)side * work->config->star_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst_packet->x0, (PSXLONG *)&burst_packet->x1, (PSXLONG *)&burst_packet->x2, (PSXLONG *)&burst_packet->x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)burst_packet, ot, (u16)depth, 1);
                }
            }
            scale.vx /= 4;
            scale.vy /= 4;
            scale.vz /= 4;
            scale.vx *= 3;
            scale.vy *= 3;
            scale.vz *= 3;
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setRGB0(burst_packet, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
            side = 1;
            for (i = 0; i < 4; i++) {
                s32 next_side = -(s16)side;
                s32 first_x = -(s16)next_side * 128;
                side = next_side;
                setVector(&vertices[0], first_x, i - 2 >= 0 ? -128 : 128, 0);
                setVector(&vertices[1], 0, i - 2 >= 0 ? -128 : 128, 0);
                setVector(&vertices[2], -(s16)side * 128, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst_packet->x0, (PSXLONG *)&burst_packet->x1, (PSXLONG *)&burst_packet->x2, (PSXLONG *)&burst_packet->x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)burst_packet, ot, (u16)depth, 1);
                }
            }
            setVector(&scale, 4096, 4096, 4096);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < work->config->spark_count; i++) {
                setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->positions[i]);
                }
                k = rand() % 4;
                setUV4(burst_packet, k % 2 * 32, k / 2 * 32 + 64, k % 2 * 32 + 31, k / 2 * 32 + 64,
                    k % 2 * 32, k / 2 * 32 + 95, k % 2 * 32 + 31, k / 2 * 32 + 95);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst_packet->x0, (PSXLONG *)&burst_packet->x1, (PSXLONG *)&burst_packet->x2, (PSXLONG *)&burst_packet->x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)burst_packet, ot, (u16)depth, 1);
                }
                addVector(&work->positions[i], &work->velocities[i]);
                work->velocities[i].vy += work->config->gravity;
            }
            flash_packet->tpage = work->burst_tpage;
            flash_packet->clut = work->burst_clut;
            setUV4(flash_packet, 0, 128, 31, 128, 0, 255, 31, 255);
            setVector(&scale, 4096, work->burst_scale * 2, 4096);
            setVector(&vertices[0], -work->config->column_width / 2, -work->config->column_height, 0);
            setVector(&vertices[1], work->config->column_width / 2, work->config->column_height, 0);
            setVector(&vertices[2], -work->config->column_width, 0, 0);
            setVector(&vertices[3], work->config->column_width, 0, 0);
            setRGB0(flash_packet, 0, 0, 0);
            setRGB1(flash_packet, 0, 0, 0);
            setRGB2(flash_packet, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
            setRGB3(flash_packet, work->burst_color.rgb.r, work->burst_color.rgb.g, work->burst_color.rgb.b);
            for (i = 0; i < work->config->rotation_count; i++) {
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&work->rotations[i], &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&flash_packet->x0, (PSXLONG *)&flash_packet->x1, (PSXLONG *)&flash_packet->x2, (PSXLONG *)&flash_packet->x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)flash_packet, ot, (u16)depth, 1);
                }
            }
            if (work->burst_scale < 0x1000) work->burst_scale += 0x400;
            else work->burst_scale += 0x100;
            if (work->burst_color.rgb.r > 15) work->burst_color.rgb.r -= 15;
            else work->burst_color.rgb.r = 0;
            if (work->burst_color.rgb.g > 15) work->burst_color.rgb.g -= 15;
            else work->burst_color.rgb.g = 0;
            if (work->burst_color.rgb.b > 15) work->burst_color.rgb.b -= 15;
            else work->burst_color.rgb.b = 0;
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (work->phase == 2 && !(work->trail_color.word & 0xFFFFFF) && !(work->burst_color.word & 0xFFFFFF)
        && work->debris_frame >= 24 && work->frames[work->config->smoke_max - 1] >= 6) {
        return 2;
    }
    if (work->phase == 0 && work->elapsed >= work->config->burst_delay) {
        work->phase = 1;
        return 1;
    }
    return 0;
}
