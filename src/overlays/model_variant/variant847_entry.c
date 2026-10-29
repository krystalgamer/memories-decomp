#include "../../types.h"
#include "variant847_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 847's entry: while the ambient light dims, two model parts leave
 * fading trails of glowing points. At each of the two hits the camera shakes,
 * an optional full-screen flash fades out, and a burst at a fixed point ahead
 * of the active slot draws a widening ring, a four-point star, embers, sparks
 * and animated debris in that hit's colour. After the second hit the light
 * comes back, and the entry ends once the second burst and the flash fade. */

static const VECTOR D_8013CE84 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model847State *work;
    MATRIX base, matrix;
    POLY_F4 screen;
    POLY_FT4 burst;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    POLY_F4 *flash_packet;
    POLY_FT4 *packet;
    s32 j, k;
    s32 side_index;
    s32 count;
    s32 y0;
    u16 half;
    s16 sign;
    s32 next_sign, first_x;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CE84;
    work = (Model847State *)context;
    flash_packet = &screen;
    packet = &burst;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CF04[command];
        for (j = 0; j < 16; j++) {
            setVector(&work->inner[j], work->config->inner_radius * ccos(j << 8) / 4096,
                work->config->inner_radius * csin(j << 8) / 4096, 0);
            setVector(&work->outer[j], work->config->outer_radius * ccos(j << 8) / 4096,
                work->config->outer_radius * csin(j << 8) / 4096, -direction * work->config->ring_depth);
        }
        /* The burst loop reuses the vertex counter; a separate counter changes the register allocation. */
        for (k = 0; k < 2; k++) {
            work->color[k].rgb.r = work->config->r[k];
            work->color[k].rgb.g = work->config->g[k];
            work->color[k].rgb.b = work->config->b[k];
            work->trail_color[k].rgb.r = 0;
            work->trail_color[k].rgb.g = 0;
            work->trail_color[k].rgb.b = 0;
            for (j = 0; j < 8; j++) {
                setVector(&work->debris[j][k], 0, 0, 0);
                setVector(&work->debris_velocities[j][k],
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256);
                work->debris_frame[k] = 0;
            }
            for (j = 0; j < 64; j++) {
                setVector(&work->embers[j][k], 0, 0, 0);
                setVector(&work->ember_velocities[j][k],
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256);
            }
            for (j = 0; j < 32; j++) {
                setVector(&work->sparks[j][k], 0, 0, 0);
                setVector(&work->spark_velocities[j][k], ((rand() - rand()) % 256) * 18 / 256,
                    ((rand() - rand()) % 256) * 18 / 256, ((rand() - rand()) % 256) * 18 / 256);
            }
            for (j = 0; j < 48; j++) {
                setVector(&work->trail[j][k], 0, 0, 0);
            }
            work->ring_scale[k] = 0x1000;
            work->unknown_1612[k] = 0;
            work->trail_head[k] = 0;
        }
        work->level = 2048;
        work->burst_tpage = 0xAE;
        work->flash.rgb.r = 0;
        work->flash.rgb.g = 0;
        work->flash.rgb.b = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->tick = 0;
#ifdef VERSION_FRENCH
        work->burst_clut = 0x3D28;
#else
        work->burst_clut = 0x3D20;
#endif
        for (j = 0; j < 4; j++) {
            work->texture[j] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CE94[j]);
        }
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    setPolyFT4(packet);
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
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed >= work->config->delay) {
        if (work->elapsed <= work->config->first_hit) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[0]), &matrix);
            setVector(&work->trail[work->trail_head[0]][0], matrix.t[0], matrix.t[1], matrix.t[2]);
            work->trail_head[0] = (work->trail_head[0] + 1) % 48;
        }
        if (work->elapsed <= work->config->second_hit && work->elapsed >= work->config->second_trail_start) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[1]), &matrix);
            setVector(&work->trail[work->trail_head[1]][1], matrix.t[0], matrix.t[1], matrix.t[2]);
            work->trail_head[1] = (work->trail_head[1] + 1) % 48;
        }
        for (side_index = 0; side_index < 2; side_index++) {
            if (work->trail_color[side_index].rgb.r != 0 || work->trail_color[side_index].rgb.g != 0
                || work->trail_color[side_index].rgb.b != 0) {
                packet->tpage = work->texture[side_index + 2] >> 16;
                packet->clut = work->texture[side_index + 2];
                setSemiTrans(packet, 1);
                setUV4(packet, side_index * 64, 96, side_index * 64 + 63, 96, side_index * 64, 159,
                    side_index * 64 + 63, 159);
                for (j = work->trail_head[side_index] - 1; j >= 0; j--) {
                    setRGB0(packet, work->trail_color[side_index].rgb.r / (work->trail_head[side_index] - j),
                        work->trail_color[side_index].rgb.g / (work->trail_head[side_index] - j),
                        work->trail_color[side_index].rgb.b / (work->trail_head[side_index] - j));
                    sign = 1;
                    GsSetLsMatrix(&base);
                    RotTrans(&work->trail[j][side_index], (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    for (k = 0; k < 4; k++) {
                        /* An s16 sign flipped with *= -1 keeps the sign extension before the negation. */
                        sign *= -1;
                        setVector(&vertices[0], -(s16)sign * 80, k - 2 >= 0 ? -80 : 80, 0);
                        setVector(&vertices[1], 0, k - 2 >= 0 ? -80 : 80, 0);
                        setVector(&vertices[2], -(s16)sign * 80, 0, 0);
                        setVector(&vertices[3], 0, 0, 0);
                        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                            (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                            (PSXLONG *)&packet->x3, &interpolation, &flag);
                        if (depth >= 0 && flag >= 0) {
                            GsSortPoly(packet, ot, (u16)depth);
                        }
                    }
                }
            }
        }
        if (work->phase == 0) {
            if (work->trail_color[0].rgb.r <= 112) work->trail_color[0].rgb.r += 15;
            else work->trail_color[0].rgb.r = 128;
            if (work->trail_color[0].rgb.g <= 112) work->trail_color[0].rgb.g += 15;
            else work->trail_color[0].rgb.g = 128;
            if (work->trail_color[0].rgb.b <= 112) work->trail_color[0].rgb.b += 15;
            else work->trail_color[0].rgb.b = 128;
        } else {
            if (work->trail_color[0].rgb.r > 15) work->trail_color[0].rgb.r -= 15;
            else work->trail_color[0].rgb.r = 0;
            if (work->trail_color[0].rgb.g > 15) work->trail_color[0].rgb.g -= 15;
            else work->trail_color[0].rgb.g = 0;
            if (work->trail_color[0].rgb.b > 15) work->trail_color[0].rgb.b -= 15;
            else work->trail_color[0].rgb.b = 0;
        }
        if (work->phase < 2) {
            if (work->trail_color[1].rgb.r <= 112) work->trail_color[1].rgb.r += 15;
            else work->trail_color[1].rgb.r = 128;
            if (work->trail_color[1].rgb.g <= 112) work->trail_color[1].rgb.g += 15;
            else work->trail_color[1].rgb.g = 128;
            if (work->trail_color[1].rgb.b <= 112) work->trail_color[1].rgb.b += 15;
            else work->trail_color[1].rgb.b = 128;
        } else {
            if (work->trail_color[1].rgb.r > 15) work->trail_color[1].rgb.r -= 15;
            else work->trail_color[1].rgb.r = 0;
            if (work->trail_color[1].rgb.g > 15) work->trail_color[1].rgb.g -= 15;
            else work->trail_color[1].rgb.g = 0;
            if (work->trail_color[1].rgb.b > 15) work->trail_color[1].rgb.b -= 15;
            else work->trail_color[1].rgb.b = 0;
        }
    }
    count = work->phase;
    if (count > 2) {
        count = 2;
    }
    for (side_index = 0; side_index < count; side_index++) {
        setSemiTrans(packet, 1);
        packet->tpage = work->burst_tpage;
        packet->clut = work->burst_clut;
        setVector(&position, 0, -350, direction * 450);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        saved = matrix;
        if (work->color[side_index].rgb.r != 0 || work->color[side_index].rgb.g != 0
            || work->color[side_index].rgb.b != 0) {
            setUV4(packet, 0, 128, 31, 128, 0, 255, 31, 255);
            setRGB0(packet, work->color[side_index].rgb.r >> 2, work->color[side_index].rgb.g >> 2,
                work->color[side_index].rgb.b >> 2);
            setVector(&scale, work->ring_scale[side_index], work->ring_scale[side_index],
                work->ring_scale[side_index]);
            matrix = saved;
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (j = 0; j < 16; j++) {
                depth = RotAverage4(&work->inner[j], &work->inner[(j + 1) % 16], &work->outer[j],
                    &work->outer[(j + 1) % 16], (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                    (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
            }
            setVector(&scale, work->ring_scale[side_index], work->ring_scale[side_index],
                work->ring_scale[side_index]);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setUV4(packet, 64, 0, 127, 0, 64, 63, 127, 63);
            sign = 1;
            for (k = 0; k < 4; k++) {
                setRGB0(packet, work->color[side_index].rgb.r >> 1, work->color[side_index].rgb.g >> 1,
                    work->color[side_index].rgb.b >> 1);
                /* The star negates a sign-extended copy and multiplies through a temporary. */
                next_sign = -(s16)sign;
                first_x = -(s16)next_sign * work->config->star_size;
                sign = next_sign;
                vertices[0].vx = first_x;
                /* Each height is reloaded and negated through a temporary. */
                y0 = work->config->star_size;
                if (k - 2 >= 0) y0 = -y0;
                vertices[0].vy = y0;
                vertices[0].vz = 0;
                vertices[1].vx = 0;
                y0 = work->config->star_size;
                if (k - 2 >= 0) y0 = -y0;
                vertices[1].vy = y0;
                vertices[1].vz = 0;
                setVector(&vertices[2], -(s16)sign * work->config->star_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                setRGB0(packet, work->color[side_index].rgb.r, work->color[side_index].rgb.g,
                    work->color[side_index].rgb.b);
                vertices[0].vx = -(s16)sign * work->config->star_size / 2;
                /* The half heights load the size once and halve it on either branch. */
                half = work->config->star_size;
                vertices[0].vy = k - 2 >= 0 ? -half / 2 : half / 2;
                vertices[0].vz = 0;
                vertices[1].vx = 0;
                half = work->config->star_size;
                vertices[1].vy = k - 2 >= 0 ? -half / 2 : half / 2;
                vertices[1].vz = 0;
                setVector(&vertices[2], -(s16)sign * work->config->star_size / 2, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
            }
            setVector(&scale, 4096, 4096, 4096);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            packet->tpage = work->texture[0] >> 16;
            packet->clut = work->texture[0];
            setUV4(packet, 128, 32, 191, 32, 128, 95, 191, 95);
            setSemiTrans(packet, 1);
            for (j = 0; j < work->config->ember_count; j++) {
                setVector(&vertices[0], -work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[1], work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[2], -work->config->ember_size, work->config->ember_size, 0);
                setVector(&vertices[3], work->config->ember_size, work->config->ember_size, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->embers[j][side_index]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->embers[j][side_index], &work->ember_velocities[j][side_index]);
            }
            packet->tpage = work->burst_tpage;
            packet->clut = work->burst_clut;
            setSemiTrans(packet, 1);
            for (j = 0; j < 32; j++) {
                /* The spark frame shares the vertex counter, which the register allocation requires. */
                k = rand() % 4;
                setUV4(packet, k % 2 * 32, k / 2 * 32 + 64, k % 2 * 32 + 31, k / 2 * 32 + 64, k % 2 * 32,
                    k / 2 * 32 + 95, k % 2 * 32 + 31, k / 2 * 32 + 95);
                setVector(&vertices[0], -16, -16, 0);
                setVector(&vertices[1], 16, -16, 0);
                setVector(&vertices[2], -16, 16, 0);
                setVector(&vertices[3], 16, 16, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->sparks[j][side_index]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->sparks[j][side_index], &work->spark_velocities[j][side_index]);
            }
            if (work->color[side_index].rgb.r > 15) work->color[side_index].rgb.r -= 15;
            else work->color[side_index].rgb.r = 0;
            if (work->color[side_index].rgb.g > 15) work->color[side_index].rgb.g -= 15;
            else work->color[side_index].rgb.g = 0;
            if (work->color[side_index].rgb.b > 15) work->color[side_index].rgb.b -= 15;
            else work->color[side_index].rgb.b = 0;
            if (work->ring_scale[side_index] >= 0x2000) work->ring_scale[side_index] += 0x80;
            else work->ring_scale[side_index] += 0x800;
        }
        if (work->debris_frame[side_index] < 16) {
            packet->tpage = work->texture[1] >> 16;
            packet->clut = work->texture[1];
            setRGB0(packet, 128, 128, 128);
            setUV4(packet, work->debris_frame[side_index] / 2 * 32, 0, work->debris_frame[side_index] / 2 * 32 + 31, 0,
                work->debris_frame[side_index] / 2 * 32, 31, work->debris_frame[side_index] / 2 * 32 + 31, 31);
            setSemiTrans(packet, 1);
            setVector(&scale, 4096, 4096, 4096);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (j = 0; j < work->config->debris_count; j++) {
                setVector(&vertices[0], -work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[1], work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[2], -work->config->debris_size, work->config->debris_size, 0);
                setVector(&vertices[3], work->config->debris_size, work->config->debris_size, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->debris[j][side_index]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->debris[j][side_index], &work->debris_velocities[j][side_index]);
            }
            work->debris_frame[side_index]++;
        }
    }
    if (work->flash.word & 0xFFFFFF) {
        setPolyF4(flash_packet);
        setRGB0(flash_packet, work->flash.rgb.r, work->flash.rgb.g, work->flash.rgb.b);
        setXY4(flash_packet, 0, 0, GRAPHICS_DEFAULT_WIDTH, 0, 0, GRAPHICS_DEFAULT_HEIGHT, GRAPHICS_DEFAULT_WIDTH,
            GRAPHICS_DEFAULT_HEIGHT);
        Graphics_SubmitDrawModePacket((u32 *)flash_packet, ot, 0, 1);
        if (work->flash.rgb.r > 31) work->flash.rgb.r -= 31;
        else work->flash.rgb.r = 0;
        if (work->flash.rgb.g > 31) work->flash.rgb.g -= 31;
        else work->flash.rgb.g = 0;
        if (work->flash.rgb.b > 31) work->flash.rgb.b -= 31;
        else work->flash.rgb.b = 0;
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 3 && !(work->color[1].word & 0xFFFFFF) && !(work->flash.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->elapsed >= work->config->second_hit && work->phase == 1) {
        Model_QueueCameraShake(work->config->shake_value, work->config->shake_arg);
        if (work->config->flash == 1) {
            work->flash.rgb.r = 255;
            work->flash.rgb.g = 255;
            work->flash.rgb.b = 255;
        }
        work->phase = 2;
        return 1;
    }
    if (work->elapsed >= work->config->first_hit && work->phase == 0) {
        Model_QueueCameraShake(work->config->shake_value, work->config->shake_arg);
        if (work->config->flash == 1) {
            work->flash.rgb.r = 255;
            work->flash.rgb.g = 255;
            work->flash.rgb.b = 255;
        }
        work->phase = 1;
        return 4;
    }
    return 0;
}
