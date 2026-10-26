#include "../../types.h"
#include "variant766_entry.h"
#include "../../game/screen_projection.h"

/* PAL images load the burst palette from VRAM x = 640. */
#ifndef MODEL_VARIANT766_CLUT_X
#define MODEL_VARIANT766_CLUT_X 512
#endif

/* Header 766's entry, a sibling of the French header-770 entry: a gouraud
 * trail between two parts of the active slot, then a flash that spins and
 * widens, a burst of two stars, falling sparks and a fan of light columns
 * in front of the slot. The slot's ambient level dips while the trail runs
 * and comes back afterwards. */

static const VECTOR D_8013C838 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model766State *work;
    MATRIX base, matrix;
    POLY_G4 gouraud;
    POLY_G4 *gouraud_packet;
    POLY_FT4 burst;
    POLY_FT4 *burst_packet;
    POLY_GT4 flash;
    POLY_GT4 *flash_packet;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    SVECTOR *point, *other;
    s32 i, k;
    s32 y0;
    u16 side;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C838;
    work = (Model766State *)context;
    gouraud_packet = &gouraud;
    burst_packet = &burst;
    flash_packet = &flash;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C880[command];
        point = work->trail_first;
        other = work->trail_second;
        for (i = 0; i < 32; point++, other++, i++) {
            setVector(point, 0, 0, 0);
            setVector(other, 0, 0, 0);
        }
        work->trail_count = 0;
        work->elapsed = 0;
        work->unknown_550 = 0;
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
        work->trail_color.rgb.r = work->config->trail_r;
        work->trail_color.rgb.g = work->config->trail_g;
        work->trail_color.rgb.b = work->config->trail_b;
        work->burst_color.rgb.r = work->config->burst_r;
        work->burst_color.rgb.g = work->config->burst_g;
        work->burst_color.rgb.b = work->config->burst_b;
        work->flash_color.rgb.r = work->config->flash_r;
        work->flash_color.rgb.g = work->config->flash_g;
        work->flash_color.rgb.b = work->config->flash_b;
        work->burst_tpage = 0xAE;
        work->burst_clut = (244 << 6) | (MODEL_VARIANT766_CLUT_X >> 4);
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C848[i]);
        }
        setVector(&work->square[0], -work->config->square_size, 0, direction << 7);
        setVector(&work->square[1], work->config->square_size, 0, direction << 7);
        setVector(&work->square[2], -work->config->square_size, 0, -direction << 7);
        setVector(&work->square[3], work->config->square_size, 0, -direction << 7);
        work->burst_scale = 0;
        work->flash_scale = 0;
        work->level = 2048;
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
        if (work->flash_color.word & 0xFFFFFF) {
            flash_packet->tpage = work->texture[0] >> 16;
            flash_packet->clut = work->texture[0];
            setVector(&rotation, 0, 0, direction * work->config->flash_spin);
            setVector(&position, 0, -350, direction * 450);
            setVector(&scale, work->flash_scale, (work->flash_scale >> 2) + 4096, (work->flash_scale >> 2) + 4096);
            setRGB0(flash_packet, work->flash_color.rgb.r, work->flash_color.rgb.g, work->flash_color.rgb.b);
            setRGB1(flash_packet, work->flash_color.rgb.r, work->flash_color.rgb.g, work->flash_color.rgb.b);
            setRGB2(flash_packet, 0, 0, 0);
            setRGB3(flash_packet, 0, 0, 0);
            GsSetLsMatrix(&base);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&base, &matrix);
            saved = matrix;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                (PSXLONG *)&flash_packet->x0, (PSXLONG *)&flash_packet->x1, (PSXLONG *)&flash_packet->x2, (PSXLONG *)&flash_packet->x3,
                &interpolation, &flag);
            setUV4(flash_packet, 0, 0, 127, 0, 0, 63, 127, 63);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)flash_packet, ot, (u16)depth, 1);
            }
            matrix = saved;
            setVector(&scale, work->flash_scale + 0x400, (work->flash_scale >> 2) + 4096,
                (work->flash_scale >> 2) + 4096);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                (PSXLONG *)&flash_packet->x0, (PSXLONG *)&flash_packet->x1, (PSXLONG *)&flash_packet->x2, (PSXLONG *)&flash_packet->x3,
                &interpolation, &flag);
            setUV4(flash_packet, 0, 0, 127, 0, 0, 63, 127, 63);
            if (depth >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)flash_packet, ot, (u16)depth, 3);
            }
            if (work->flash_scale < 0x2000) work->flash_scale += 0x800;
            else work->flash_scale += 0x80;
            if (work->flash_color.rgb.r > 15) work->flash_color.rgb.r -= 15;
            else work->flash_color.rgb.r = 0;
            if (work->flash_color.rgb.g > 15) work->flash_color.rgb.g -= 15;
            else work->flash_color.rgb.g = 0;
            if (work->flash_color.rgb.b > 15) work->flash_color.rgb.b -= 15;
            else work->flash_color.rgb.b = 0;
        }
        if (work->phase > 0 && (work->burst_color.word & 0xFFFFFF)) {
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
        && !(work->flash_color.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->phase == 0 && work->elapsed >= work->config->burst_delay) {
        work->phase = 1;
        return 1;
    }
    return 0;
}
