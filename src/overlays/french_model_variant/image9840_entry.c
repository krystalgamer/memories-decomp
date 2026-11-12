#include "../../types.h"
#include "image9840_entry.h"
#include "../../psyq/memory.h"
#ifdef MODEL_IMAGE9840_WORK_SIZE_PRINTF
#include "../../psyq/stdio.h"
/* variant631us_entry.c defines gModelVariant631Us_szWorkSizeFormat
   before including this source. */
#endif

#define FADE_PRIMARY(c, k) \
    if (gFrenchModelVariant648_aConfigs[work->head.command].c / ((u16)(work->primary[i].head.duration / 10) - 1) * step \
        < work->primary[i].head.color[k]) { \
        work->primary[i].head.color[k] -= gFrenchModelVariant648_aConfigs[work->head.command].c \
            / ((u16)(work->primary[i].head.duration / 10) - 1) * step; \
    } else { \
        work->primary[i].head.color[k] = 0; \
    }

#define FADE_SECONDARY(c, k) \
    if (gFrenchModelVariant648_aConfigs[work->head.command].secondary_##c / (work->secondary[i].head.duration / 2 - 1) * step \
        < work->secondary[i].head.color[k]) { \
        work->secondary[i].head.color[k] -= gFrenchModelVariant648_aConfigs[work->head.command].secondary_##c \
            / (work->secondary[i].head.duration / 2 - 1) * step; \
    } else { \
        work->secondary[i].head.color[k] = 0; \
    }

#define FADE_BURST(c, k) \
    if (gFrenchModelVariant648_aConfigs[work->head.command].c / (work->bursts[i].head.duration / 2 - 1) * step \
        < work->bursts[i].head.color[k]) { \
        work->bursts[i].head.color[k] -= gFrenchModelVariant648_aConfigs[work->head.command].c \
            / (work->bursts[i].head.duration / 2 - 1) * step; \
    } else { \
        work->bursts[i].head.color[k] = 0; \
    }

s32 FrenchModelVariant648_HandleRequest(u8 *context, s32 command)
{
    Image9840State *work;
    MATRIX ls, world;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    SVECTOR zero, midpoint0, midpoint1;
    VECTOR position;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, k;
    u32 texture;
    long depth;

    work = (Image9840State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Image9840State));
#ifdef MODEL_IMAGE9840_WORK_SIZE_PRINTF
        printf(gModelVariant631Us_szWorkSizeFormat, sizeof(Image9840State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->head.elapsed = 0;
        texture = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gFrenchModelVariant648_Image);
        work->tpage = texture >> 16;
        work->clut = texture;
        switch (command) {
        case 0:
            work->primary_count = 2;
            work->secondary_count = 1;
            work->burst_count = 16;
            break;
        }
        work->radius = gFrenchModelVariant648_aConfigs[command].radius;
        work->length = gFrenchModelVariant648_aConfigs[command].length;
        work->start_radius = gFrenchModelVariant648_aConfigs[command].start_radius;
        work->end_radius = gFrenchModelVariant648_aConfigs[command].end_radius;
        for (i = 0; i < work->primary_count; i++) {
            work->primary[i].head.duration = 70;
            work->primary[i].head.active = 0;
            work->primary[i].head.elapsed = 0;
            work->primary[i].head.mode = 1;
            work->primary[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->primary[i].head.color[0] = gFrenchModelVariant648_aConfigs[command].red;
            work->primary[i].head.color[1] = gFrenchModelVariant648_aConfigs[command].green;
            work->primary[i].head.color[2] = gFrenchModelVariant648_aConfigs[command].blue;
            setVector(&work->primary[i].xform.scale, 0, 0, 0);
            setVector(&work->primary[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->primary[i].xform.rotation, 0, work->head.slot * 2048, i * 256);
                setVector(&work->primary[i].xform.position, -5 + i * 10, -10, -30);
                break;
            }
        }
        for (i = 0; i < work->secondary_count; i++) {
            work->secondary[i].head.duration = 48;
            work->secondary[i].head.active = 0;
            work->secondary[i].head.elapsed = 0;
            work->secondary[i].head.mode = 1;
            work->secondary[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->secondary[i].head.color[0] = gFrenchModelVariant648_aConfigs[command].secondary_red;
            work->secondary[i].head.color[1] = gFrenchModelVariant648_aConfigs[command].secondary_green;
            work->secondary[i].head.color[2] = gFrenchModelVariant648_aConfigs[command].secondary_blue;
            work->secondary[i].head.u = 0;
            work->secondary[i].head.v = 0;
            setVector(&work->secondary[i].xform.scale,
                4096 - (i << 12) / work->secondary_count,
                4096 - (i << 12) / work->secondary_count, 4096);
            setVector(&work->secondary[i].growth, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->secondary[i].xform.position, 0, -10, -30);
                setVector(&work->secondary[i].xform.rotation, 0, work->head.slot * 2048 + 2048, 0);
                break;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].head.duration = 32;
            work->bursts[i].head.active = 0;
            work->bursts[i].head.elapsed = 0;
            work->bursts[i].head.mode = 1;
            work->bursts[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].head.color[0] = gFrenchModelVariant648_aConfigs[command].red;
            work->bursts[i].head.color[1] = gFrenchModelVariant648_aConfigs[command].green;
            work->bursts[i].head.color[2] = gFrenchModelVariant648_aConfigs[command].blue;
            work->bursts[i].head.u = 0;
            work->bursts[i].head.v = 0;
            setVector(&work->bursts[i].xform.scale, 0, 0, 0);
            setVector(&work->bursts[i].growth, 0, 0, 0);
            switch (command) {
            case 0: {
                s32 vertical;

                setVector(&work->bursts[i].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->bursts[i].xform.position,
                    work->radius * ((i % 3 + 1) << 1)
                        * rsin((i << 12) / work->burst_count) / 4096 * (i % 2 * 2 - 1) / 2,
                    (vertical = ((work->burst_count / 2 - i) % 3) * 2,
                        work->radius * vertical * (i % 2 * 2 - 1)),
                    work->radius * ((i % 3 + 1) << 1)
                        * rcos((i << 12) / work->burst_count) / 4096 * (i % 2 * 2 - 1) / 2);
                break;
            }
            }
            work->bursts[i].xform.position.vy -= 350;
            work->bursts[i].xform.position.vz += (work->head.slot * 2 - 1) * 450;
        }
        work->delay = gFrenchModelVariant648_aConfigs[command].delay;
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
    switch (work->head.command) {
    case 0:
        if (work->head.elapsed == 0) {
            u8 active;

            active = 1;
            for (i = 1; i >= 0; i--) {
                work->primary[i].head.active = active;
            }
        }
        if (work->head.elapsed >= 45 && work->flags[0] == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 7);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->secondary_count; i++) {
                work->secondary[i].head.active = 1;
                work->secondary[i].xform.position.vx += ls.t[0];
                work->secondary[i].xform.position.vy += ls.t[1];
                work->secondary[i].xform.position.vz += ls.t[2];
            }
            work->flags[0] = 1;
        }
        if (work->head.elapsed >= 45 && work->flags[1] < work->burst_count) {
            work->bursts[work->flags[1]].head.active = 1;
            work->flags[1]++;
        }
        break;
    }
    PushMatrix();
    view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
    inactive = 0;
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 7);
    GsGetLwUnit(&unit, &world);
    for (i = 0; i < work->primary_count; i++) {
        if (work->primary[i].head.active == 1) {
            unit.flg = 0;
            ApplyMatrixLV(&world, &work->primary[i].xform.position, &position);
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->primary[i].xform.scale);
            RotMatrixZ(work->primary[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->primary[i].xform.rotation.vx
                - (*(s16 *)&view.bytes[4] - 4096) * (work->head.slot * 2 - 1), &unit.matrix);
            {
                s32 heading;

                heading = work->primary[i].xform.rotation.vy + 3078;
                RotMatrixY(heading - *(s16 *)&view.bytes[2], &unit.matrix);
            }
            unit.matrix.t[0] = position.vx + world.t[0];
            unit.matrix.t[1] = position.vy + world.t[1];
            unit.matrix.t[2] = position.vz + world.t[2];
            unit.super = work->primary[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            work->primary[i].xform.rotation.vz += 81;
            if (work->primary[i].head.u % 2 == 1) {
                setVector(&work->primary[i].xform.scale,
                    work->primary[i].head.elapsed * 200,
                    work->primary[i].head.elapsed * 200,
                    work->primary[i].head.elapsed * 200);
            } else {
                setVector(&work->primary[i].xform.scale,
                    work->primary[i].head.elapsed * 100,
                    work->primary[i].head.elapsed * 100,
                    work->primary[i].head.elapsed * 100);
            }
            if (work->primary[i].head.elapsed > work->primary[i].head.duration * 9 / 10) {
                FADE_PRIMARY(red, 0);
                FADE_PRIMARY(green, 1);
                FADE_PRIMARY(blue, 2);
            }
            setVector(&zero, 0, 0, 0);
            for (k = 0; k < 4; k++) {
                setPolyG3(&work->triangle);
                setRGB0(&work->triangle, 0, 0, 0);
                setRGB1(&work->triangle, 0, 0, 0);
                setRGB2(&work->triangle, work->primary[i].head.color[0],
                    work->primary[i].head.color[1], work->primary[i].head.color[2]);
                depth = RotAverage3(&work->square[k], &work->inner[k], &zero,
                    (long *)&work->triangle.x0, (long *)&work->triangle.x1,
                    (long *)&work->triangle.x2, &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->triangle,
                        Graphics_GetDefaultOrderingTable(), (u16)depth, work->primary[i].head.mode);
                }
                setPolyG3(&work->triangle);
                setRGB0(&work->triangle, 0, 0, 0);
                setRGB1(&work->triangle, 0, 0, 0);
                setRGB2(&work->triangle, work->primary[i].head.color[0],
                    work->primary[i].head.color[1], work->primary[i].head.color[2]);
                depth = RotAverage3(&work->square[k], &work->inner[k + 1], &zero,
                    (long *)&work->triangle.x0, (long *)&work->triangle.x1,
                    (long *)&work->triangle.x2, &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->triangle,
                        Graphics_GetDefaultOrderingTable(), (u16)depth, work->primary[i].head.mode);
                }
            }
            work->primary[i].head.u++;
            work->primary[i].head.elapsed += step;
            if (work->primary[i].head.elapsed > work->primary[i].head.duration) {
                work->primary[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->secondary_count; i++) {
        if (work->secondary[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->secondary[i].xform.scale);
            RotMatrixY(work->secondary[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->secondary[i].xform.position.vx;
            unit.matrix.t[1] = work->secondary[i].xform.position.vy;
            unit.matrix.t[2] = work->secondary[i].xform.position.vz;
            unit.super = work->secondary[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->secondary[i].head.elapsed > work->secondary[i].head.duration / 2) {
                FADE_SECONDARY(red, 0);
                FADE_SECONDARY(green, 1);
                FADE_SECONDARY(blue, 2);
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
                setUV4(&work->quad, work->secondary[i].head.u + 32, work->secondary[i].head.v + 1,
                    work->secondary[i].head.u + 32, work->secondary[i].head.v + 127,
                    work->secondary[i].head.u + 64, work->secondary[i].head.v + 1,
                    work->secondary[i].head.u + 64, work->secondary[i].head.v + 127);
                setRGB0(&work->quad, 0, 0, 0);
                setRGB1(&work->quad, 0, 0, 0);
                setRGB2(&work->quad, work->secondary[i].head.color[0],
                    work->secondary[i].head.color[1], work->secondary[i].head.color[2]);
                setRGB3(&work->quad, work->secondary[i].head.color[0],
                    work->secondary[i].head.color[1], work->secondary[i].head.color[2]);
                depth = RotAverage4(&work->first_ring[k], &work->first_ring[k + 1],
                    &midpoint0, &midpoint1, (long *)&work->quad.x0,
                    (long *)&work->quad.x1, (long *)&work->quad.x2,
                    (long *)&work->quad.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                setPolyGT4(&work->quad);
                setSemiTrans(&work->quad, 1);
                work->quad.tpage = work->tpage;
                work->quad.clut = work->clut;
                setUV4(&work->quad, work->secondary[i].head.u + 64, work->secondary[i].head.v + 1,
                    work->secondary[i].head.u + 64, work->secondary[i].head.v + 127,
                    work->secondary[i].head.u + 96, work->secondary[i].head.v + 1,
                    work->secondary[i].head.u + 96, work->secondary[i].head.v + 127);
                setRGB2(&work->quad, 0, 0, 0);
                setRGB3(&work->quad, 0, 0, 0);
                setRGB0(&work->quad, work->secondary[i].head.color[0],
                    work->secondary[i].head.color[1], work->secondary[i].head.color[2]);
                setRGB1(&work->quad, work->secondary[i].head.color[0],
                    work->secondary[i].head.color[1], work->secondary[i].head.color[2]);
                depth = RotAverage4(&midpoint0, &midpoint1,
                    &work->second_ring[k], &work->second_ring[k + 1],
                    (long *)&work->quad.x0, (long *)&work->quad.x1,
                    (long *)&work->quad.x2, (long *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
            }
            work->secondary[i].head.elapsed += step;
            if (work->secondary[i].head.elapsed > work->secondary[i].head.duration) {
                work->secondary[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->burst_count; i++) {
        if (work->bursts[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->bursts[i].xform.scale);
            RotMatrixZ(work->bursts[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->bursts[i].xform.rotation.vx
                - (*(s16 *)&view.bytes[4] - 4096) * (work->head.slot * 2 - 1), &unit.matrix);
            {
                s32 heading;

                heading = work->bursts[i].xform.rotation.vy + 3078;
                RotMatrixY(heading - *(s16 *)&view.bytes[2], &unit.matrix);
            }
            unit.matrix.t[0] = work->bursts[i].xform.position.vx;
            unit.matrix.t[1] = work->bursts[i].xform.position.vy;
            unit.matrix.t[2] = work->bursts[i].xform.position.vz;
            unit.super = work->bursts[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            work->bursts[i].xform.scale.vx =
                (work->bursts[i].head.elapsed << 12) / work->bursts[i].head.duration;
            work->bursts[i].xform.scale.vy =
                (work->bursts[i].head.elapsed << 12) / work->bursts[i].head.duration;
            work->bursts[i].xform.scale.vz =
                (work->bursts[i].head.elapsed << 12) / work->bursts[i].head.duration;
            if (work->bursts[i].head.elapsed > work->bursts[i].head.duration / 2) {
                FADE_BURST(red, 0);
                FADE_BURST(green, 1);
                FADE_BURST(blue, 2);
            }
            setVector(&zero, 0, 0, 0);
            for (k = 0; k < 16; k++) {
                setPolyG3(&work->triangle);
                setRGB0(&work->triangle, 0, 0, 0);
                setRGB1(&work->triangle, 0, 0, 0);
                setRGB2(&work->triangle, work->bursts[i].head.color[0],
                    work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                depth = RotAverage3(&work->burst_ring[k], &work->burst_ring[k + 1], &zero,
                    (long *)&work->triangle.x0, (long *)&work->triangle.x1,
                    (long *)&work->triangle.x2, &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->triangle,
                        Graphics_GetDefaultOrderingTable(), (u16)depth, work->bursts[i].head.mode);
                }
            }
            work->bursts[i].head.elapsed += step;
            if (work->bursts[i].head.elapsed > work->bursts[i].head.duration) {
                work->bursts[i].head.active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    if (work->head.elapsed > 80 && work->flags[4] == 0) {
        result = 3;
        work->flags[4] = 1;
    }
    PopMatrix();
    work->head.elapsed += step;
    if (inactive == work->primary_count + work->secondary_count + work->burst_count) {
        result = 2;
    }
    return result;
}
