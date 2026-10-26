#include "../../types.h"
#include "variant752_entry.h"
#include "../../game/screen_projection.h"

/* Header 752's entry, a sibling of header 754's: while the camera moves
 * through the row's presets and the slot's parts are tinted, a burst in front
 * of the slot draws a growing cloud of small four-point sparkles that grow and
 * fade one by one, a widening ring, a four-point star, embers and debris. The
 * ambient level dips until the burst and comes back afterwards. */

static const VECTOR D_8013C980 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model752State *work;
    MATRIX base, matrix, saved;
    POLY_FT4 burst;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    POLY_FT4 *packet;
    s32 i, k;
    s32 y0;
    u16 side;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C980;
    work = (Model752State *)context;
    packet = &burst;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C9E4[command];
        for (i = 0; i < 64; i++) {
            setVector(&work->sparkles[i], work->config->sparkle_speed * ((rand() - rand()) % 256) / 256,
                work->config->sparkle_speed * ((rand() - rand()) % 256) / 256,
                ((u16)work->config->sparkle_speed >> 2) * ((rand() - rand()) % 256) / 256);
            work->scales[i] = 0;
            work->colors[i].r = 128;
            work->colors[i].g = 128;
            work->colors[i].b = 128;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->inner[i], work->config->inner_radius * ccos(i << 8) / 4096,
                work->config->inner_radius * csin(i << 8) / 4096, 0);
            setVector(&work->outer[i], work->config->outer_radius * ccos(i << 8) / 4096,
                work->config->outer_radius * csin(i << 8) / 4096, -direction * work->config->ring_depth);
        }
        work->color.rgb.r = work->config->r;
        work->color.rgb.g = work->config->g;
        work->color.rgb.b = work->config->b;
        for (i = 0; i < 8; i++) {
            setVector(&work->debris[i], 0, 0, 0);
            setVector(&work->debris_velocities[i], work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                work->config->debris_speed * ((rand() - rand()) % 256) / 256,
                work->config->debris_speed * ((rand() - rand()) % 256) / 256);
            work->debris_frame = 0;
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->embers[i], 0, 0, 0);
            setVector(&work->ember_velocities[i], work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                work->config->ember_speed * ((rand() - rand()) % 256) / 256,
                work->config->ember_speed * ((rand() - rand()) % 256) / 256);
        }
        work->level = 2048;
        work->ring_scale = 0x1000;
        work->burst_tpage = 0xAE;
        work->phase = 0;
        work->elapsed = 0;
        work->tick = 0;
        work->sparkle_count = 0;
#ifdef VERSION_FRENCH
        work->burst_clut = 0x3D28;
#else
        work->burst_clut = 0x3D20;
#endif
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C990[i]);
        }
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->config->mode == 0) {
        if (work->elapsed < work->config->camera_times[0]) {
            Model_QueueCameraOrbitFromPreset(slot, 14, &rotation, -8);
        } else if (work->elapsed < work->config->camera_times[1]) {
            Model_QueueCameraOrbitFromPreset(slot, 4, &rotation, 0);
        } else if (work->elapsed < work->config->camera_times[2]) {
            Model_QueueCameraOrbitFromPreset(slot, 2, &rotation, 0);
        } else if (work->elapsed < work->config->camera_times[3]) {
            Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, 0);
        } else if (work->elapsed < work->config->burst_time) {
            Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, 0);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, -8);
        }
    } else if (work->config->mode == 1) {
        if (work->elapsed < work->config->delay) {
            Model_QueueCameraOrbitFromPreset(slot, 14, &rotation, -4);
        } else if (direction > 0) {
            Model_QueueCameraOrbitFromPreset(2, 6, &rotation, 5);
        } else {
            Model_QueueCameraOrbitFromPreset(2, 8, &rotation, 5);
        }
    } else if (work->config->mode == 2) {
        if (work->elapsed < work->config->delay) {
            Model_QueueCameraOrbitFromPreset(slot, 14, &rotation, -8);
        } else {
            if (direction > 0) {
                Model_QueueCameraOrbitFromPreset(2, 6, &rotation, -12);
            } else {
                Model_QueueCameraOrbitFromPreset(2, 8, &rotation, -12);
            }
        }
    }
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
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    setPolyFT4(packet);
    if (work->config->mode == 0) {
        if (work->elapsed < work->config->tint_stop && work->tick % work->config->tint_period == 0) {
            Model_QueueTintRequestForAllParts(slot, work->config->tint_mode, work->config->tint_start,
                work->config->tint_end, work->config->tint_duration);
        }
    } else if (work->config->mode == 1) {
        if (work->elapsed < work->config->tint_stop && work->tick % work->config->tint_period == 0) {
            Model_QueueTintRequestForParts(slot, work->config->tint_mode, work->config->tint_start,
                work->config->tint_end, work->config->tint_duration, 7, 8, 9, -1);
        }
    } else if (work->config->mode == 2) {
        if (work->elapsed < work->config->tint_stop && work->tick % work->config->tint_period == 0) {
            Model_QueueTintRequestForParts(slot, work->config->tint_mode, work->config->tint_start,
                work->config->tint_end, work->config->tint_duration, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                13, 14, 15, 16, 17, 18, -1);
        }
    }
    if (work->phase != 0) {
        packet->tpage = work->texture[2] >> 16;
        packet->clut = work->texture[2];
        setUV4(packet, 0, 32, 63, 32, 0, 95, 63, 95);
        for (i = 0; i < work->sparkle_count; i++) {
            if (work->colors[i].r != 0 || work->colors[i].g != 0 || work->colors[i].b != 0) {
                setVector(&scale, work->scales[i], work->scales[i], work->scales[i]);
                setVector(&position, 0, -350, direction * 450);
                addVector(&position, &work->sparkles[i]);
                setRGB0(packet, work->colors[i].r, work->colors[i].g, work->colors[i].b);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                side = 1;
                for (k = 0; k < 4; k++) {
                    s32 next_side = -(s16)side;
                    s32 first_x = -(s16)next_side * work->config->sparkle_size;
                    side = next_side;
                    vertices[0].vx = first_x;
                    y0 = work->config->sparkle_size;
                    if (k - 2 >= 0) y0 = -y0;
                    vertices[0].vy = y0;
                    vertices[0].vz = 0;
                    vertices[1].vx = 0;
                    y0 = work->config->sparkle_size;
                    if (k - 2 >= 0) y0 = -y0;
                    vertices[1].vy = y0;
                    vertices[1].vz = 0;
                    setVector(&vertices[2], -(s16)side * work->config->sparkle_size, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                        (PSXLONG *)&packet->x3, &interpolation, &flag);
                    setSemiTrans(packet, 1);
                    if (depth >= 0 && flag >= 0) {
                        GsSortPoly(packet, ot, (u16)depth);
                    }
                }
                if (work->colors[i].r > 15) work->colors[i].r -= 15;
                else work->colors[i].r = 0;
                if (work->colors[i].g > 15) work->colors[i].g -= 15;
                else work->colors[i].g = 0;
                if (work->colors[i].b > 15) work->colors[i].b -= 15;
                else work->colors[i].b = 0;
                if (work->scales[i] >= 0x2000) work->scales[i] += 0x80;
                else work->scales[i] += 0x800;
            }
        }
        packet->tpage = work->burst_tpage;
        packet->clut = work->burst_clut;
        setVector(&position, 0, -350, direction * 450);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        saved = matrix;
        if (work->color.word & 0xFFFFFF) {
            setUV4(packet, 0, 128, 31, 128, 0, 255, 31, 255);
            setRGB0(packet, work->color.rgb.r >> 1, work->color.rgb.g >> 1, work->color.rgb.b >> 1);
            setVector(&scale, work->ring_scale, work->ring_scale, work->ring_scale);
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
            setRGB0(packet, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
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
                    addVector(&vertices[k], &work->embers[i]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->embers[i], &work->ember_velocities[i]);
            }
            if (work->color.rgb.r > 15) work->color.rgb.r -= 15;
            else work->color.rgb.r = 0;
            if (work->color.rgb.g > 15) work->color.rgb.g -= 15;
            else work->color.rgb.g = 0;
            if (work->color.rgb.b > 15) work->color.rgb.b -= 15;
            else work->color.rgb.b = 0;
            if (work->ring_scale >= 0x2000) work->ring_scale += 0x80;
            else work->ring_scale += 0x800;
        }
        if (work->debris_frame < 16) {
            packet->tpage = work->texture[1] >> 16;
            packet->clut = work->texture[1];
            setRGB0(packet, 128, 128, 128);
            setUV4(packet, work->debris_frame / 2 * 32, 0, work->debris_frame / 2 * 32 + 31, 0,
                work->debris_frame / 2 * 32, 31, work->debris_frame / 2 * 32 + 31, 31);
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
                    addVector(&vertices[k], &work->debris[i]);
                }
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&packet->x0, (PSXLONG *)&packet->x1, (PSXLONG *)&packet->x2,
                    (PSXLONG *)&packet->x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(packet, ot, (u16)depth);
                }
                addVector(&work->debris[i], &work->debris_velocities[i]);
            }
            work->debris_frame++;
        }
        work->sparkle_count += work->config->sparkle_rate;
        if (work->config->sparkle_max < work->sparkle_count) {
            work->sparkle_count = work->config->sparkle_max;
        }
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 2 && work->colors[work->config->sparkle_max - 1].r == 0
        && work->colors[work->config->sparkle_max - 1].g == 0 && work->colors[work->config->sparkle_max - 1].b == 0
        && !(work->color.word & 0xFFFFFF)) {
        return 2;
    }
    if (work->elapsed >= work->config->burst_time && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    return 0;
}
