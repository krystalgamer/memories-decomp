#include "../../types.h"
#include "image9840_entry.h"
#include "../../psyq/memory.h"
#ifdef MODEL_IMAGE9840_WORK_SIZE_PRINTF
#include "../../psyq/stdio.h"
#endif

#define FADE_PRIMARY(c) \
    if (D_8013CD70[work->command].c / ((u16)(work->primary[i].duration / 10) - 1) * step \
        < work->primary[i].c) { \
        work->primary[i].c -= D_8013CD70[work->command].c \
            / ((u16)(work->primary[i].duration / 10) - 1) * step; \
    } else { \
        work->primary[i].c = 0; \
    }

#define FADE_SECONDARY(c) \
    if (D_8013CD70[work->command].secondary_##c / (work->secondary[i].duration / 2 - 1) * step \
        < work->secondary[i].c) { \
        work->secondary[i].c -= D_8013CD70[work->command].secondary_##c \
            / (work->secondary[i].duration / 2 - 1) * step; \
    } else { \
        work->secondary[i].c = 0; \
    }

#define FADE_BURST(c) \
    if (D_8013CD70[work->command].c / (work->bursts[i].duration / 2 - 1) * step \
        < work->bursts[i].c) { \
        work->bursts[i].c -= D_8013CD70[work->command].c \
            / (work->bursts[i].duration / 2 - 1) * step; \
    } else { \
        work->bursts[i].c = 0; \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Image9840State *work;
    MATRIX ls, world;
    GsCOORDUNIT unit;
    Image9840ViewSnapshot view;
    SVECTOR zero, midpoint0, midpoint1;
    VECTOR position;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, k;
    u32 texture;
    PSXLONG depth;

    work = (Image9840State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Image9840State));
#ifdef MODEL_IMAGE9840_WORK_SIZE_PRINTF
        printf(MODEL_IMAGE9840_WORK_SIZE_FORMAT, sizeof(Image9840State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        texture = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CD54);
        work->tpage = texture >> 16;
        work->clut = texture;
        switch (command) {
        case 0:
            work->primary_count = 2;
            work->secondary_count = 1;
            work->burst_count = 16;
            break;
        }
        work->radius = D_8013CD70[command].radius;
        work->length = D_8013CD70[command].length;
        work->start_radius = D_8013CD70[command].start_radius;
        work->end_radius = D_8013CD70[command].end_radius;
        for (i = 0; i < work->primary_count; i++) {
            work->primary[i].duration = 70;
            work->primary[i].active = 0;
            work->primary[i].elapsed = 0;
            work->primary[i].mode = 1;
            work->primary[i].coord = Model_GetCameraCoordinateUnit();
            work->primary[i].red = D_8013CD70[command].red;
            work->primary[i].green = D_8013CD70[command].green;
            work->primary[i].blue = D_8013CD70[command].blue;
            setVector(&work->primary[i].scale, 0, 0, 0);
            setVector(&work->primary[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->primary[i].rotation, 0, work->slot * 2048, i * 256);
                setVector(&work->primary[i].position, -5 + i * 10, -10, -30);
                break;
            }
        }
        for (i = 0; i < work->secondary_count; i++) {
            work->secondary[i].duration = 48;
            work->secondary[i].active = 0;
            work->secondary[i].elapsed = 0;
            work->secondary[i].mode = 1;
            work->secondary[i].coord = Model_GetCameraCoordinateUnit();
            work->secondary[i].red = D_8013CD70[command].secondary_red;
            work->secondary[i].green = D_8013CD70[command].secondary_green;
            work->secondary[i].blue = D_8013CD70[command].secondary_blue;
            work->secondary[i].u = 0;
            work->secondary[i].v = 0;
            setVector(&work->secondary[i].scale,
                4096 - (i << 12) / work->secondary_count,
                4096 - (i << 12) / work->secondary_count, 4096);
            setVector(&work->secondary[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->secondary[i].position, 0, -10, -30);
                setVector(&work->secondary[i].rotation, 0, work->slot * 2048 + 2048, 0);
                break;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].duration = 32;
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].red = D_8013CD70[command].red;
            work->bursts[i].green = D_8013CD70[command].green;
            work->bursts[i].blue = D_8013CD70[command].blue;
            work->bursts[i].u = 0;
            work->bursts[i].v = 0;
            setVector(&work->bursts[i].scale, 0, 0, 0);
            setVector(&work->bursts[i].growth, 0, 0, 0);
            switch (command) {
            case 0: {
                s32 vertical;

                setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
                setVector(&work->bursts[i].position,
                    work->radius * ((i % 3 + 1) << 1)
                        * rsin((i << 12) / work->burst_count) / 4096 * (i % 2 * 2 - 1) / 2,
                    (vertical = ((work->burst_count / 2 - i) % 3) * 2,
                        work->radius * vertical * (i % 2 * 2 - 1)),
                    work->radius * ((i % 3 + 1) << 1)
                        * rcos((i << 12) / work->burst_count) / 4096 * (i % 2 * 2 - 1) / 2);
                break;
            }
            }
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += (work->slot * 2 - 1) * 450;
        }
        work->delay = D_8013CD70[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 4; i++) {
            setVector(&work->square[i],
                work->radius * rsin(i * 1024) / 4096,
                work->radius * rcos(i * 1024) / 4096, 0);
        }
        for (i = 0; i < 5; i++) {
            setVector(&work->inner[i],
                (u16)(work->radius / 3) * rsin(i * 1024 - 512) / 4096,
                (u16)(work->radius / 3) * rcos(i * 1024 - 512) / 4096, 0);
        }
        for (i = 0; i < 17; i++) {
            s32 sample, radius, sine_wave, cosine_wave;

            setVector(&work->burst_ring[i],
                (sample = rsin(i * 256), radius = work->radius,
                    sine_wave = sample * 5, radius * sine_wave / 4096),
                (sample = rcos(i * 256), radius = work->radius,
                    cosine_wave = sample * 5, radius * cosine_wave / 4096), 0);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->elapsed == 0) {
            u8 active;

            active = 1;
            for (i = 1; i >= 0; i--) {
                work->primary[i].active = active;
            }
        }
        if (work->elapsed >= 45 && work->flags[0] == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 7);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->secondary_count; i++) {
                work->secondary[i].active = 1;
                work->secondary[i].position.vx += ls.t[0];
                work->secondary[i].position.vy += ls.t[1];
                work->secondary[i].position.vz += ls.t[2];
            }
            work->flags[0] = 1;
        }
        if (work->elapsed >= 45 && work->flags[1] < work->burst_count) {
            work->bursts[work->flags[1]].active = 1;
            work->flags[1]++;
        }
        break;
    }
    PushMatrix();
    view = *(Image9840ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 7);
    GsGetLwUnit(&unit, &world);
    for (i = 0; i < work->primary_count; i++) {
        if (work->primary[i].active == 1) {
            unit.flg = 0;
            ApplyMatrixLV(&world, &work->primary[i].position, &position);
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->primary[i].scale);
            RotMatrixZ(work->primary[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->primary[i].rotation.vx
                - (*(s16 *)&view.bytes[4] - 4096) * (work->slot * 2 - 1), &unit.matrix);
            {
                s32 heading;

                heading = work->primary[i].rotation.vy + 3078;
                RotMatrixY(heading - *(s16 *)&view.bytes[2], &unit.matrix);
            }
            unit.matrix.t[0] = position.vx + world.t[0];
            unit.matrix.t[1] = position.vy + world.t[1];
            unit.matrix.t[2] = position.vz + world.t[2];
            unit.super = work->primary[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            work->primary[i].rotation.vz += 81;
            if (work->primary[i].u % 2 == 1) {
                setVector(&work->primary[i].scale,
                    work->primary[i].elapsed * 200,
                    work->primary[i].elapsed * 200,
                    work->primary[i].elapsed * 200);
            } else {
                setVector(&work->primary[i].scale,
                    work->primary[i].elapsed * 100,
                    work->primary[i].elapsed * 100,
                    work->primary[i].elapsed * 100);
            }
            if (work->primary[i].elapsed > work->primary[i].duration * 9 / 10) {
                FADE_PRIMARY(red);
                FADE_PRIMARY(green);
                FADE_PRIMARY(blue);
            }
            setVector(&zero, 0, 0, 0);
            for (k = 0; k < 4; k++) {
                setPolyG3(&work->triangle);
                setRGB0(&work->triangle, 0, 0, 0);
                setRGB1(&work->triangle, 0, 0, 0);
                setRGB2(&work->triangle, work->primary[i].red,
                    work->primary[i].green, work->primary[i].blue);
                depth = RotAverage3(&work->square[k], &work->inner[k], &zero,
                    (PSXLONG *)&work->triangle.x0, (PSXLONG *)&work->triangle.x1,
                    (PSXLONG *)&work->triangle.x2, &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->triangle,
                        func_80058F10(), (u16)depth, work->primary[i].mode);
                }
                setPolyG3(&work->triangle);
                setRGB0(&work->triangle, 0, 0, 0);
                setRGB1(&work->triangle, 0, 0, 0);
                setRGB2(&work->triangle, work->primary[i].red,
                    work->primary[i].green, work->primary[i].blue);
                depth = RotAverage3(&work->square[k], &work->inner[k + 1], &zero,
                    (PSXLONG *)&work->triangle.x0, (PSXLONG *)&work->triangle.x1,
                    (PSXLONG *)&work->triangle.x2, &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->triangle,
                        func_80058F10(), (u16)depth, work->primary[i].mode);
                }
            }
            work->primary[i].u++;
            work->primary[i].elapsed += step;
            if (work->primary[i].elapsed > work->primary[i].duration) {
                work->primary[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->secondary_count; i++) {
        if (work->secondary[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->secondary[i].scale);
            RotMatrixY(work->secondary[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->secondary[i].position.vx;
            unit.matrix.t[1] = work->secondary[i].position.vy;
            unit.matrix.t[2] = work->secondary[i].position.vz;
            unit.super = work->secondary[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->secondary[i].elapsed > work->secondary[i].duration / 2) {
                FADE_SECONDARY(red);
                FADE_SECONDARY(green);
                FADE_SECONDARY(blue);
            }
            {
                s32 radius0, radius1;
                s32 width0, width1;

                width0 = (work->secondary[i].growth.vz * work->end_radius
                    + (work->length - work->secondary[i].growth.vz) * work->start_radius)
                    * 2 / work->length;
                width1 = ((work->secondary[i].growth.vz + work->secondary[i].growth.vy)
                    * (work->end_radius - work->start_radius) + work->end_radius * work->length)
                    * 2 / work->length;
                radius0 = (s16)width0 / 2;
                radius1 = (s16)width1 / 2;
                for (k = 0; k < 17; k++) {
                    setVector(&work->first_ring[k],
                        radius0 * rsin(k * 256) / 4096,
                        radius0 * rcos(k * 256) / 4096,
                        -work->length / 2 + work->secondary[i].growth.vz);
                    setVector(&work->second_ring[k],
                        radius1 * rsin(k * 256) / 4096,
                        radius1 * rcos(k * 256) / 4096,
                        work->length / 2 + work->secondary[i].growth.vz + work->secondary[i].growth.vy);
                }
            }
            work->secondary[i].growth.vz += step * 2;
            work->secondary[i].growth.vy += step * 4;
            for (k = 0; k < 16; k++) {
                setVector(&midpoint0,
                    (work->first_ring[k].vx + work->second_ring[k].vx) / 2,
                    (work->first_ring[k].vy + work->second_ring[k].vy) / 2,
                    (work->first_ring[k].vz + work->second_ring[k].vz) / 2);
                setVector(&midpoint1,
                    (work->first_ring[k + 1].vx + work->second_ring[k + 1].vx) / 2,
                    (work->first_ring[k + 1].vy + work->second_ring[k + 1].vy) / 2,
                    (work->first_ring[k + 1].vz + work->second_ring[k + 1].vz) / 2);
                setPolyGT4(&work->quad);
                setSemiTrans(&work->quad, 1);
                work->quad.tpage = work->tpage;
                work->quad.clut = work->clut;
                setUV4(&work->quad, work->secondary[i].u + 32, work->secondary[i].v + 1,
                    work->secondary[i].u + 32, work->secondary[i].v + 127,
                    work->secondary[i].u + 64, work->secondary[i].v + 1,
                    work->secondary[i].u + 64, work->secondary[i].v + 127);
                setRGB0(&work->quad, 0, 0, 0);
                setRGB1(&work->quad, 0, 0, 0);
                setRGB2(&work->quad, work->secondary[i].red,
                    work->secondary[i].green, work->secondary[i].blue);
                setRGB3(&work->quad, work->secondary[i].red,
                    work->secondary[i].green, work->secondary[i].blue);
                depth = RotAverage4(&work->first_ring[k], &work->first_ring[k + 1],
                    &midpoint0, &midpoint1, (PSXLONG *)&work->quad.x0,
                    (PSXLONG *)&work->quad.x1, (PSXLONG *)&work->quad.x2,
                    (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
                setPolyGT4(&work->quad);
                setSemiTrans(&work->quad, 1);
                work->quad.tpage = work->tpage;
                work->quad.clut = work->clut;
                setUV4(&work->quad, work->secondary[i].u + 64, work->secondary[i].v + 1,
                    work->secondary[i].u + 64, work->secondary[i].v + 127,
                    work->secondary[i].u + 96, work->secondary[i].v + 1,
                    work->secondary[i].u + 96, work->secondary[i].v + 127);
                setRGB2(&work->quad, 0, 0, 0);
                setRGB3(&work->quad, 0, 0, 0);
                setRGB0(&work->quad, work->secondary[i].red,
                    work->secondary[i].green, work->secondary[i].blue);
                setRGB1(&work->quad, work->secondary[i].red,
                    work->secondary[i].green, work->secondary[i].blue);
                depth = RotAverage4(&midpoint0, &midpoint1,
                    &work->second_ring[k], &work->second_ring[k + 1],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
            }
            work->secondary[i].elapsed += step;
            if (work->secondary[i].elapsed > work->secondary[i].duration) {
                work->secondary[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->burst_count; i++) {
        if (work->bursts[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
            RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->bursts[i].rotation.vx
                - (*(s16 *)&view.bytes[4] - 4096) * (work->slot * 2 - 1), &unit.matrix);
            {
                s32 heading;

                heading = work->bursts[i].rotation.vy + 3078;
                RotMatrixY(heading - *(s16 *)&view.bytes[2], &unit.matrix);
            }
            unit.matrix.t[0] = work->bursts[i].position.vx;
            unit.matrix.t[1] = work->bursts[i].position.vy;
            unit.matrix.t[2] = work->bursts[i].position.vz;
            unit.super = work->bursts[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            work->bursts[i].scale.vx =
                (work->bursts[i].elapsed << 12) / work->bursts[i].duration;
            work->bursts[i].scale.vy =
                (work->bursts[i].elapsed << 12) / work->bursts[i].duration;
            work->bursts[i].scale.vz =
                (work->bursts[i].elapsed << 12) / work->bursts[i].duration;
            if (work->bursts[i].elapsed > work->bursts[i].duration / 2) {
                FADE_BURST(red);
                FADE_BURST(green);
                FADE_BURST(blue);
            }
            setVector(&zero, 0, 0, 0);
            for (k = 0; k < 16; k++) {
                setPolyG3(&work->triangle);
                setRGB0(&work->triangle, 0, 0, 0);
                setRGB1(&work->triangle, 0, 0, 0);
                setRGB2(&work->triangle, work->bursts[i].red,
                    work->bursts[i].green, work->bursts[i].blue);
                depth = RotAverage3(&work->burst_ring[k], &work->burst_ring[k + 1], &zero,
                    (PSXLONG *)&work->triangle.x0, (PSXLONG *)&work->triangle.x1,
                    (PSXLONG *)&work->triangle.x2, &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->triangle,
                        func_80058F10(), (u16)depth, work->bursts[i].mode);
                }
            }
            work->bursts[i].elapsed += step;
            if (work->bursts[i].elapsed > work->bursts[i].duration) {
                work->bursts[i].active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    if (work->elapsed > 80 && work->flags[4] == 0) {
        result = 3;
        work->flags[4] = 1;
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->primary_count + work->secondary_count + work->burst_count) {
        result = 2;
    }
    return result;
}
