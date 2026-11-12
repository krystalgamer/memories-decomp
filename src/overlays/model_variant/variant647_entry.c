#include "../../types.h"
#include "variant647_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/model_slot_support.h"

/* Header 647's entry: one or two bursts of rings, each set up from a command
 * row; every burst grows from the camera unit for 64 frames and
 * fades out in its second half. The setup also prints the work-area size. */

s32 ModelVariant647_HandleRequest(u8 *context, s32 command)
{
    Model647State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, fade;
    s32 angle, turn;
    s32 size;
    u32 tex;
    u16 duration;
    long depth;

    work = (Model647State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model647State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model647State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant647_Image);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant647_Image);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 16:
        case 17:
        case 18:
            work->count = 1;
            break;
        case 14:
        case 15:
            work->count = 2;
            break;
        }
        work->radius = gModelVariant647_aConfigs[command].radius;
        work->height = gModelVariant647_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->bursts[i].head.duration = 64;
            work->bursts[i].head.active = 0;
            work->bursts[i].head.elapsed = 0;
            work->bursts[i].head.mode = 1;
            work->bursts[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].head.color[0] = gModelVariant647_aConfigs[command].red;
            work->bursts[i].head.color[1] = gModelVariant647_aConfigs[command].green;
            work->bursts[i].head.color[2] = gModelVariant647_aConfigs[command].blue;
            work->bursts[i].head.u = 0;
            work->bursts[i].head.v = 0;
            setVector(&work->bursts[i].xform.scale, 0, 0, 0);
            setVector(&work->bursts[i].growth, 0, 0, 0);
            switch (command) {
            case 9:
            case 10:
                setVector(&work->bursts[i].xform.rotation, 0x200, work->head.slot * 2048, 0);
                break;
            case 15:
            case 18:
                setVector(&work->bursts[i].xform.rotation, -0x400, work->head.slot * 2048, 0);
                break;
            default:
                setVector(&work->bursts[i].xform.rotation, 0, work->head.slot * 2048, 0);
                break;
            }
            switch (command) {
            case 0:
            case 5:
            case 6:
            case 12:
                setVector(&work->bursts[i].xform.position, 0, 50, 0);
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 7:
            case 8:
            case 11:
            case 13:
            case 16:
            case 17:
                setVector(&work->bursts[i].xform.position, 0, 0, 0);
                break;
            case 9:
                setVector(&work->bursts[i].xform.position, 0, -50, -(work->direction * 200));
                break;
            case 10:
                setVector(&work->bursts[i].xform.position, 0, -50, -(work->direction * 150));
                break;
            case 14:
                setVector(&work->bursts[i].xform.position, i * 150 - 75, 50, 0);
                break;
            case 15:
                setVector(&work->bursts[i].xform.position, 0, 100, -(work->direction * 70));
                break;
            case 18:
                setVector(&work->bursts[i].xform.position, 0, 200, 0);
                break;
            }
            work->bursts[i].xform.position.vy -= 350;
            work->bursts[i].xform.position.vz += work->direction * 450;
        }
        work->delay = gModelVariant647_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 17; j++) {
                angle = (i << 7) + (j << 8);
                setVector(&work->inner[j][i], work->radius * (i * 2 + 2) / 4 * rsin(angle) / 4096,
                    work->radius * (i * 2 + 2) / 4 * rcos(angle) / 4096, 0);
                setVector(&work->outer[j][i], work->radius * (i * 6 + 6) / 4 * rsin(angle) / 4096,
                    work->radius * (i * 6 + 6) / 4 * rcos(angle) / 4096, work->height * (i - 4) / 4);
            }
        }
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 17; j++) {
                turn = (i << 7) + (j << 8);
                setVector(&work->top[j][i], work->radius * (i * 2 + 2) / 8 * rsin(turn) / 4096,
                    work->radius * (i * 2 + 2) / 8 * rcos(turn) / 4096, 0);
                turn = ((i + 1) << 7) + (j << 8);
                setVector(&work->bottom[j][i], work->radius * ((i + 1) << 3) / 8 * rsin(turn) / 4096,
                    work->radius * ((i + 1) << 3) / 8 * rcos(turn) / 4096,
                    work->height * -((8 - i) * 2) / 8 - work->height);
            }
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 16:
        case 17:
        case 18:
            if (work->head.elapsed == 0) {
                for (i = 0; i < work->count; i++) {
                    work->bursts[i].head.active = 1;
                }
                result = 1;
            }
            break;
        case 15:
            if (work->head.elapsed == 0) {
                work->bursts[0].head.active = 1;
                result = 4;
            }
            if (work->flags[0] == 0 && work->head.elapsed >= 41) {
                result = work->flags[0] = work->bursts[1].head.active = 1;
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->bursts[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                size = work->bursts[i].head.elapsed * 0x6000 / work->bursts[i].head.duration;
                setVector(&work->bursts[i].xform.scale, size, size, size);
                ScaleMatrix(&unit.matrix, &work->bursts[i].xform.scale);
                RotMatrixX(work->bursts[i].xform.rotation.vx, &unit.matrix);
                RotMatrixY(work->bursts[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].xform.position.vx;
                unit.matrix.t[1] = work->bursts[i].xform.position.vy;
                unit.matrix.t[2] = work->bursts[i].xform.position.vz;
                unit.super = work->bursts[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                duration = work->bursts[i].head.duration;
                if (duration / 2 < work->bursts[i].head.elapsed) {
                    fade = (duration - work->bursts[i].head.elapsed) * 2;
                    work->bursts[i].head.color[0] = gModelVariant647_aConfigs[work->head.command].red * fade / duration;
                    work->bursts[i].head.color[1] = gModelVariant647_aConfigs[work->head.command].green * fade / duration;
                    work->bursts[i].head.color[2] = gModelVariant647_aConfigs[work->head.command].blue * fade / duration;
                }
                for (k = 0; k < 4; k++) {
                    for (j = 0; j < 16; j++) {
                        setPolyGT4(&work->quad);
                        work->quad.tpage = work->tpage[0];
                        work->quad.clut = work->clut[0];
                        setSemiTrans(&work->quad, 1);
                        setUV4(&work->quad, work->bursts[i].head.u, work->bursts[i].head.v, work->bursts[i].head.u + 31, work->bursts[i].head.v,
                            work->bursts[i].head.u, work->bursts[i].head.v + 127, work->bursts[i].head.u + 31, work->bursts[i].head.v + 127);
                        setRGB0(&work->quad, 0, 0, 0);
                        setRGB1(&work->quad, 0, 0, 0);
                        setRGB2(&work->quad, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        setRGB3(&work->quad, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        depth = RotAverage4(&work->inner[j][k], &work->inner[j + 1][k],
                            &work->outer[j][k], &work->outer[j + 1][k],
                            (long *)&work->quad.x0, (long *)&work->quad.x1,
                            (long *)&work->quad.x2, (long *)&work->quad.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                for (k = 0; k < 8; k++) {
                    for (j = 0; j < 16; j++) {
                        setPolyGT3(&work->triangle);
                        work->triangle.tpage = work->tpage[0];
                        work->triangle.clut = work->clut[0];
                        setSemiTrans(&work->triangle, 1);
                        setUV3(&work->triangle, work->bursts[i].head.u, work->bursts[i].head.v, work->bursts[i].head.u + 31, work->bursts[i].head.v,
                            work->bursts[i].head.u + 16, work->bursts[i].head.v + 127);
                        setRGB2(&work->triangle, 0, 0, 0);
                        setRGB0(&work->triangle, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        setRGB1(&work->triangle, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                        depth = RotAverage3(&work->top[j][k], &work->top[j + 1][k],
                            &work->bottom[j][k],
                            (long *)&work->triangle.x0, (long *)&work->triangle.x1,
                            (long *)&work->triangle.x2, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->triangle, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                work->bursts[i].head.elapsed += step;
                if (work->bursts[i].head.duration < work->bursts[i].head.elapsed) {
                    work->bursts[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        if (inactive == work->count) {
            result = 2;
        }
    }
    return result;
}
