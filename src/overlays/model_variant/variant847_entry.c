#include "../../types.h"
#include "variant847_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 847's entry: two model points leave trails of four-point stars while
 * the ambient level dips, then two bursts in front of the slot each draw a
 * widening ring, a double star, embers, sparks and debris; the shake and an
 * optional full-screen flash mark each burst before the ambient comes back. */

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
    s32 i, j, k, count;
    u16 side;
    s32 y0;
    u16 half;
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
        for (i = 0; i < 16; i++) {
            setVector(&work->inner[i], work->config->inner_radius * ccos(i << 8) / 4096,
                work->config->inner_radius * csin(i << 8) / 4096, 0);
            setVector(&work->outer[i], work->config->outer_radius * ccos(i << 8) / 4096,
                work->config->outer_radius * csin(i << 8) / 4096, -direction * work->config->ring_depth);
        }
        for (j = 0; j < 2; j++) {
            work->color[j].rgb.r = work->config->r[j];
            work->color[j].rgb.g = work->config->g[j];
            work->color[j].rgb.b = work->config->b[j];
            work->glow[j].r = 0;
            work->glow[j].g = 0;
            work->glow[j].b = 0;
            for (i = 0; i < 8; i++) {
                setVector(&work->debris[i][j], 0, 0, 0);
                setVector(&work->debris_velocities[i][j],
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256);
                work->debris_frame[j] = 0;
            }
            for (i = 0; i < 64; i++) {
                setVector(&work->embers[i][j], 0, 0, 0);
                setVector(&work->ember_velocities[i][j],
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256);
            }
            for (i = 0; i < 32; i++) {
                setVector(&work->sparks[i][j], 0, 0, 0);
                setVector(&work->spark_velocities[i][j], 18 * ((rand() - rand()) % 256) / 256,
                    18 * ((rand() - rand()) % 256) / 256, 18 * ((rand() - rand()) % 256) / 256);
            }
            for (i = 0; i < 48; i++) {
                setVector(&work->trail[i][j], 0, 0, 0);
            }
            work->ring_scale[j] = 0x1000;
            work->unknown_1612[j] = 0;
            work->head[j] = 0;
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
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CE94[i]);
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
        if (work->elapsed <= work->config->first_end) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part[0]), &matrix);
            setVector(&work->trail[work->head[0]][0], matrix.t[0], matrix.t[1], matrix.t[2]);
            work->head[0] = (work->head[0] + 1) % 48;
        }
        if (work->elapsed <= work->config->second_end && work->elapsed >= work->config->second_start) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part[1]), &matrix);
            setVector(&work->trail[work->head[1]][1], matrix.t[0], matrix.t[1], matrix.t[2]);
            work->head[1] = (work->head[1] + 1) % 48;
        }
        for (k = 0; k < 2; k++) {
            if (work->glow[k].r || work->glow[k].g || work->glow[k].b) {
                packet->tpage = work->texture[k + 2] >> 16;
                packet->clut = work->texture[k + 2];
                setSemiTrans(packet, 1);
                setUV4(packet, k * 64, 96, k * 64 + 63, 96, k * 64, 159, k * 64 + 63, 159);
                for (i = work->head[k] - 1; i >= 0; i--) {
                    setRGB0(packet, work->glow[k].r / (work->head[k] - i), work->glow[k].g / (work->head[k] - i),
                        work->glow[k].b / (work->head[k] - i));
                    side = 1;
                    GsSetLsMatrix(&base);
                    RotTrans(&work->trail[i][k], (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    for (j = 0; j < 4; j++) {
                        s32 next_side = -(s16)side;
                        s32 first_x = -(s16)next_side * 80;
                        side = next_side;
                        vertices[0].vx = first_x;
                        vertices[0].vy = (j - 2 >= 0) ? -80 : 80;
                        vertices[0].vz = 0;
                        vertices[1].vx = 0;
                        vertices[1].vy = (j - 2 >= 0) ? -80 : 80;
                        vertices[1].vz = 0;
                        setVector(&vertices[2], -(s16)side * 80, 0, 0);
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
            if (work->glow[0].r < 113) work->glow[0].r += 15;
            else work->glow[0].r = 128;
            if (work->glow[0].g < 113) work->glow[0].g += 15;
            else work->glow[0].g = 128;
            if (work->glow[0].b < 113) work->glow[0].b += 15;
            else work->glow[0].b = 128;
        } else {
            if (work->glow[0].r > 15) work->glow[0].r -= 15;
            else work->glow[0].r = 0;
            if (work->glow[0].g > 15) work->glow[0].g -= 15;
            else work->glow[0].g = 0;
            if (work->glow[0].b > 15) work->glow[0].b -= 15;
            else work->glow[0].b = 0;
        }
        if (work->phase < 2) {
            if (work->glow[1].r < 113) work->glow[1].r += 15;
            else work->glow[1].r = 128;
            if (work->glow[1].g < 113) work->glow[1].g += 15;
            else work->glow[1].g = 128;
            if (work->glow[1].b < 113) work->glow[1].b += 15;
            else work->glow[1].b = 128;
        } else {
            if (work->glow[1].r > 15) work->glow[1].r -= 15;
            else work->glow[1].r = 0;
            if (work->glow[1].g > 15) work->glow[1].g -= 15;
            else work->glow[1].g = 0;
            if (work->glow[1].b > 15) work->glow[1].b -= 15;
            else work->glow[1].b = 0;
        }
    }
    count = work->phase;
    if (count > 2) {
        count = 2;
    }
    for (k = 0; k < count; k++) {
        setSemiTrans(packet, 1);
        packet->tpage = work->burst_tpage;
        packet->clut = work->burst_clut;
        setVector(&position, 0, -350, direction * 450);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        saved = matrix;
        if (work->color[k].rgb.r || work->color[k].rgb.g || work->color[k].rgb.b) {
            setUV4(packet, 0, 128, 31, 128, 0, 255, 31, 255);
            setRGB0(packet, work->color[k].rgb.r >> 2, work->color[k].rgb.g >> 2, work->color[k].rgb.b >> 2);
            setVector(&scale, work->ring_scale[k], work->ring_scale[k], work->ring_scale[k]);
            matrix = saved;
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < 16; i++) {
                depth = RotAverage4(&work->inner[i], &work->inner[(i + 1) % 16], &work->outer[i],
                    &work->outer[(i + 1) % 16], (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                    (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
            }
            setVector(&scale, work->ring_scale[k], work->ring_scale[k], work->ring_scale[k]);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setUV4(packet, 64, 0, 127, 0, 64, 63, 127, 63);
            side = 1;
            for (j = 0; j < 4; j++) {
                s32 next_side;
                s32 first_x;
                setRGB0(packet, work->color[k].rgb.r >> 1, work->color[k].rgb.g >> 1, work->color[k].rgb.b >> 1);
                next_side = -(s16)side;
                first_x = -(s16)next_side * work->config->star_size;
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
                    GsSortPoly(packet, ot, (u16)depth);
                }
                setRGB0(packet, work->color[k].rgb.r, work->color[k].rgb.g, work->color[k].rgb.b);
                vertices[0].vx = -(s16)side * work->config->star_size / 2;
                half = work->config->star_size;
                vertices[0].vy = (j - 2 >= 0) ? -half / 2 : half / 2;
                vertices[0].vz = 0;
                vertices[1].vx = 0;
                half = work->config->star_size;
                vertices[1].vy = (j - 2 >= 0) ? -half / 2 : half / 2;
                vertices[1].vz = 0;
                setVector(&vertices[2], -(s16)side * work->config->star_size / 2, 0, 0);
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
            for (i = 0; i < work->config->ember_count; i++) {
                setVector(&vertices[0], -work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[1], work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[2], -work->config->ember_size, work->config->ember_size, 0);
                setVector(&vertices[3], work->config->ember_size, work->config->ember_size, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&vertices[j], &work->embers[i][k]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->embers[i][k], &work->ember_velocities[i][k]);
            }
            packet->tpage = work->burst_tpage;
            packet->clut = work->burst_clut;
            setSemiTrans(packet, 1);
            for (i = 0; i < 32; i++) {
                j = rand() % 4;
                setUV4(packet, j % 2 * 32, j / 2 * 32 + 64, j % 2 * 32 + 31, j / 2 * 32 + 64,
                    j % 2 * 32, j / 2 * 32 + 95, j % 2 * 32 + 31, j / 2 * 32 + 95);
                setVector(&vertices[0], -16, -16, 0);
                setVector(&vertices[1], 16, -16, 0);
                setVector(&vertices[2], -16, 16, 0);
                setVector(&vertices[3], 16, 16, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&vertices[j], &work->sparks[i][k]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->sparks[i][k], &work->spark_velocities[i][k]);
            }
            if (work->color[k].rgb.r > 15) work->color[k].rgb.r -= 15;
            else work->color[k].rgb.r = 0;
            if (work->color[k].rgb.g > 15) work->color[k].rgb.g -= 15;
            else work->color[k].rgb.g = 0;
            if (work->color[k].rgb.b > 15) work->color[k].rgb.b -= 15;
            else work->color[k].rgb.b = 0;
            if (work->ring_scale[k] >= 0x2000) work->ring_scale[k] += 0x80;
            else work->ring_scale[k] += 0x800;
        }
        if (work->debris_frame[k] < 16) {
            packet->tpage = work->texture[1] >> 16;
            packet->clut = work->texture[1];
            setRGB0(packet, 128, 128, 128);
            setUV4(packet, work->debris_frame[k] / 2 * 32, 0, work->debris_frame[k] / 2 * 32 + 31, 0,
                work->debris_frame[k] / 2 * 32, 31, work->debris_frame[k] / 2 * 32 + 31, 31);
            setSemiTrans(packet, 1);
            setVector(&scale, 4096, 4096, 4096);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < work->config->debris_count; i++) {
                setVector(&vertices[0], -work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[1], work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[2], -work->config->debris_size, work->config->debris_size, 0);
                setVector(&vertices[3], work->config->debris_size, work->config->debris_size, 0);
                for (j = 0; j < 4; j++) {
                    addVector(&vertices[j], &work->debris[i][k]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->debris[i][k], &work->debris_velocities[i][k]);
            }
            work->debris_frame[k]++;
        }
    }
    if (work->flash.word & 0xFFFFFF) {
        setPolyF4(flash_packet);
        setRGB0(flash_packet, work->flash.rgb.r, work->flash.rgb.g, work->flash.rgb.b);
        setXY4(flash_packet, 0, 0, 320, 0, 0, GRAPHICS_DEFAULT_HEIGHT, 320, GRAPHICS_DEFAULT_HEIGHT);
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
    if (work->elapsed >= work->config->second_end && work->phase == 1) {
        Model_QueueCameraShake(work->config->shake_value, work->config->shake_arg);
        if (work->config->flash == 1) {
            work->flash.rgb.r = 255;
            work->flash.rgb.g = 255;
            work->flash.rgb.b = 255;
        }
        work->phase = 2;
        return 1;
    }
    if (work->elapsed >= work->config->first_end && work->phase == 0) {
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
