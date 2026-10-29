#include "../../types.h"
#include "variant767_entry.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_constants.h"

/* Header 767's entry, the header-754 burst run twice: after the row's delay
 * each of two bursts in front of the slot draws a growing cloud of animated
 * smoke puffs, a widening ring, a four-point star, embers and debris, each
 * burst shaking the camera and optionally flashing the screen. The ambient
 * level dips until the second burst and comes back afterwards. */

static const VECTOR D_8013C7A4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model767State *work;
    MATRIX base, matrix;
    POLY_F4 screen;
    POLY_FT4 burst;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 direction, step;
    GsOT *ot;
    POLY_F4 *flash_packet;
    POLY_FT4 *packet;
    s32 i, j, k;
    s32 count;
    s32 y0;
    u16 side;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C7A4;
    work = (Model767State *)context;
    flash_packet = &screen;
    packet = &burst;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C808[command];
        for (i = 0; i < 16; i++) {
            setVector(&work->inner[i], work->config->inner_radius * ccos(i << 8) / 4096,
                work->config->inner_radius * csin(i << 8) / 4096, 0);
            setVector(&work->outer[i], work->config->outer_radius * ccos(i << 8) / 4096,
                work->config->outer_radius * csin(i << 8) / 4096, -direction * work->config->ring_depth);
        }
        for (k = 0; k < 2; k++) {
            for (i = 0; i < 64; i++) {
                setVector(&work->smoke[i][k], work->config->smoke_speed * ((rand() - rand()) % 256) / 256,
                    work->config->smoke_speed * ((rand() - rand()) % 256) / 256,
                    ((u16)work->config->smoke_speed >> 2) * ((rand() - rand()) % 256) / 256);
                work->frames[i][k] = 0;
            }
            work->color[k].rgb.r = work->config->r;
            work->color[k].rgb.g = work->config->g;
            work->color[k].rgb.b = work->config->b;
            for (i = 0; i < 8; i++) {
                setVector(&work->debris[i][k], 0, 0, 0);
                setVector(&work->debris_velocities[i][k], work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                    work->config->debris_speed * ((rand() - rand()) % 256) / 256);
                work->debris_frame[k] = 0;
            }
            for (i = 0; i < 64; i++) {
                setVector(&work->embers[i][k], 0, 0, 0);
                setVector(&work->ember_velocities[i][k], work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                    work->config->ember_speed * ((rand() - rand()) % 256) / 256);
            }
            work->ring_scale[k] = 0x1000;
            work->smoke_count[k] = 0;
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
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C7B4[i]);
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
    count = work->phase;
    if (count > 2) {
        count = 2;
    }
    for (j = 0; j < count; j++) {
        packet->tpage = work->texture[2] >> 16;
        packet->clut = work->texture[2];
        for (i = 0; i < work->smoke_count[j]; i++) {
            if (work->frames[i][j] < 8) {
                setUV4(packet, work->frames[i][j] % 4 * 64, work->frames[i][j] / 4 * 64 + 96,
                    work->frames[i][j] % 4 * 64 + 63, work->frames[i][j] / 4 * 64 + 96,
                    work->frames[i][j] % 4 * 64, work->frames[i][j] / 4 * 64 + 159,
                    work->frames[i][j] % 4 * 64 + 63, work->frames[i][j] / 4 * 64 + 159);
                setVector(&scale, 4096, 4096, 4096);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, &work->smoke[i][j]);
                setRGB0(packet, 128, 128, 128);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                setVector(&vertices[0], -work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&vertices[1], work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&vertices[2], -work->config->smoke_size, work->config->smoke_size, 0);
                setVector(&vertices[3], work->config->smoke_size, work->config->smoke_size, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                setSemiTrans(packet, 1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                work->frames[i][j]++;
            }
        }
        packet->tpage = work->burst_tpage;
        packet->clut = work->burst_clut;
        setVector(&position, 0, -350, direction * 450);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        saved = matrix;
        if (work->color[j].rgb.r || work->color[j].rgb.g || work->color[j].rgb.b) {
            setUV4(packet, 0, 128, 31, 128, 0, 255, 31, 255);
            setRGB0(packet, work->color[j].rgb.r >> 2, work->color[j].rgb.g >> 2, work->color[j].rgb.b >> 2);
            setVector(&scale, work->ring_scale[j], work->ring_scale[j], work->ring_scale[j]);
            matrix = saved;
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < 16; i++) {
                depth = RotAverage4(&work->inner[i], &work->inner[(i + 1) % 16], &work->outer[i],
                    &work->outer[(i + 1) % 16], (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1,
                    (PSXLONG *)&packet->x2, (PSXLONG *)&packet->x3, &interpolation, &flag);
                setSemiTrans(packet, 1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
            }
            setVector(&scale, 4096, 4096, 4096);
            matrix = saved;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            setRGB0(packet, work->color[j].rgb.r >> 1, work->color[j].rgb.g >> 1, work->color[j].rgb.b >> 1);
            setUV4(packet, 64, 0, 127, 0, 64, 63, 127, 63);
            side = 1;
            for (k = 0; k < 4; k++) {
                s32 next_side = -(s16)side;
                s32 first_x = -(s16)next_side * work->config->star_size;
                side = next_side;
                vertices[0].vx = first_x;
                y0 = work->config->star_size;
                if (k - 2 >= 0) y0 = -y0;
                vertices[0].vy = y0;
                vertices[0].vz = 0;
                vertices[1].vx = 0;
                y0 = work->config->star_size;
                if (k - 2 >= 0) y0 = -y0;
                vertices[1].vy = y0;
                vertices[1].vz = 0;
                setVector(&vertices[2], -(s16)side * work->config->star_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                setSemiTrans(packet, 1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
            }
            packet->tpage = work->texture[0] >> 16;
            packet->clut = work->texture[0];
            setUV4(packet, 128, 32, 191, 32, 128, 95, 191, 95);
            setSemiTrans(packet, 1);
            for (i = 0; i < work->config->ember_count; i++) {
                setVector(&vertices[0], -work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[1], work->config->ember_size, -work->config->ember_size, 0);
                setVector(&vertices[2], -work->config->ember_size, work->config->ember_size, 0);
                setVector(&vertices[3], work->config->ember_size, work->config->ember_size, 0);
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->embers[i][j]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->embers[i][j], &work->ember_velocities[i][j]);
            }
            if (work->color[j].rgb.r > 15) work->color[j].rgb.r -= 15;
            else work->color[j].rgb.r = 0;
            if (work->color[j].rgb.g > 15) work->color[j].rgb.g -= 15;
            else work->color[j].rgb.g = 0;
            if (work->color[j].rgb.b > 15) work->color[j].rgb.b -= 15;
            else work->color[j].rgb.b = 0;
            if (work->ring_scale[j] >= 0x2000) work->ring_scale[j] += 0x80;
            else work->ring_scale[j] += 0x800;
        }
        if (work->debris_frame[j] < 16) {
            packet->tpage = work->texture[1] >> 16;
            packet->clut = work->texture[1];
            setRGB0(packet, 128, 128, 128);
            setUV4(packet, work->debris_frame[j] / 2 * 32, 0, work->debris_frame[j] / 2 * 32 + 31, 0,
                work->debris_frame[j] / 2 * 32, 31, work->debris_frame[j] / 2 * 32 + 31, 31);
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
                for (k = 0; k < 4; k++) {
                    addVector(&vertices[k], &work->debris[i][j]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->debris[i][j], &work->debris_velocities[i][j]);
            }
            work->debris_frame[j]++;
        }
        work->smoke_count[j] += work->config->smoke_rate;
        if (work->config->smoke_max < work->smoke_count[j]) {
            work->smoke_count[j] = work->config->smoke_max;
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
    if (work->phase == 3 && work->frames[work->config->smoke_max - 1][1] >= 8
        && !(work->color[1].word & 0xFFFFFF) && !(work->flash.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->elapsed >= work->config->second_burst_time && work->phase == 1) {
        Model_QueueCameraShake(work->config->shake_value, work->config->shake_arg);
        if (work->config->flash == 1) {
            work->flash.rgb.r = 255;
            work->flash.rgb.g = 255;
            work->flash.rgb.b = 255;
        }
        work->phase = 2;
        return 1;
    }
    if (work->elapsed >= work->config->first_burst_time && work->phase == 0) {
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
