#include "../../types.h"
#include "variant634_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 634's entry: tilted rings of gouraud bands open one a frame in front
 * of the slot and grow, while a cluster of sparks flashes through four frames
 * of the second texture. Both fade over the second half of their lifetime.
 * The setup also prints the work-area size. */

/* The quad corners' negations go through an int parameter, so the front end
 * does not narrow them to halfword arithmetic: that is retail's three loads
 * of the radius and the zero-extension before the third negation. */
static inline s32 Model634_Negate(s32 value)
{
    return -value;
}

s32 func_8013B004(u8 *context, s32 command)
{
    Model634State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model634ViewSnapshot view;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, angle;
    u32 tex;
    PSXLONG depth;

    work = (Model634State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model634State));
        printf("E_DATA size = %x byte\n", sizeof(Model634State));
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013C394[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013C394[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
        case 2:
        case 3:
        case 5:
        case 6:
            work->ring_count = 16;
            work->spark_count = 32;
            break;
        case 1:
            work->ring_count = 8;
            work->spark_count = 32;
            break;
        case 4:
            work->ring_count = 20;
            work->spark_count = 32;
            break;
        }
        work->radius = D_8013C3CC[command].radius;
        work->unknown_0C = D_8013C3CC[command].unknown_04;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].duration = 48;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].red = D_8013C3CC[command].red;
            work->rings[i].green = D_8013C3CC[command].green;
            work->rings[i].blue = D_8013C3CC[command].blue;
            work->rings[i].u = 0;
            work->rings[i].v = 32;
            setVector(&work->rings[i].scale, 0, 0, 0);
            setVector(&work->rings[i].unknown_3C, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->rings[i].rotation, (i % 2 * 2 - 1) * (i / 8 * 0x71000 / 3 * (i % 8) / 8 + 398),
                    ((i % 8 << 9) + 398) + work->slot * 2048, i % 8 << 9);
                setVector(&work->rings[i].position, 0, 0, 0);
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                setVector(&work->rings[i].rotation, (i % 2 * 2 - 1) * (i / 8 * 0x71000 / 3 * (i % 8) / 8 + 398),
                    ((i % 8 << 9) + 398) + work->slot * 2048, i % 8 << 9);
                setVector(&work->rings[i].position, 0, 0, 0);
                break;
            }
            work->rings[i].position.vy -= 350;
            work->rings[i].position.vz += (work->slot * 2 - 1) * 450;
        }
        for (i = 0; i < work->spark_count; i++) {
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            if (command == 4) {
                work->sparks[i].duration = 80;
            } else {
                work->sparks[i].duration = 48;
            }
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].red = D_8013C3CC[command].spark_red;
            work->sparks[i].green = D_8013C3CC[command].spark_green;
            work->sparks[i].blue = D_8013C3CC[command].spark_blue;
            work->sparks[i].u = 0;
            work->sparks[i].v = 0;
            setVector(&work->sparks[i].scale, 4096, 4096, 4096);
            setVector(&work->sparks[i].unknown_3C, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->sparks[i].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sparks[i].position,
                    work->radius / 2 * (i % 3 + 1) * rsin(i * 4096 / work->spark_count) / 4096,
                    work->radius / 2 * ((work->spark_count / 2 - i) % 3) * (i % 2 * 2 - 1),
                    work->radius / 2 * (i % 3 + 1) * rcos(i * 4096 / work->spark_count) / 4096 * (i % 2 * 2 - 1));
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                setVector(&work->sparks[i].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sparks[i].position,
                    work->radius / 2 * (i % 3 + 1) * rsin(i * 4096 / work->spark_count) / 4096,
                    work->radius / 2 * ((work->spark_count / 2 - i) % 3) * (i % 2 * 2 - 1),
                    work->radius / 2 * (i % 3 + 1) * rcos(i * 4096 / work->spark_count) / 4096 * (i % 2 * 2 - 1));
                break;
            }
            work->sparks[i].position.vy -= 350;
            work->sparks[i].position.vz += (work->slot * 2 - 1) * 450;
        }
        work->delay = D_8013C3CC[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 17; i++) {
            angle = i * 64 + 0x600;
            setVector(&work->ring[i][0], work->radius * rsin(angle) / 4096, work->radius * rcos(angle) / 4096, 0);
            setVector(&work->ring[i][1], work->radius * (rsin(angle) << 2) / 4096,
                work->radius * (rcos(angle) << 2) / 4096, 0);
        }
        setVector(&work->square[0], Model634_Negate(work->radius), Model634_Negate(work->radius), 0);
        setVector(&work->square[1], work->radius, Model634_Negate(work->radius), 0);
        setVector(&work->square[2], Model634_Negate(work->radius), work->radius, 0);
        setVector(&work->square[3], work->radius, work->radius, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (work->flags[0] < work->ring_count) {
                work->rings[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            if (work->elapsed == 0) {
                for (i = 0; i < work->spark_count; i++) {
                    work->sparks[i].active = 1;
                }
            }
            break;
        }
        inactive = 0;
        PushMatrix();
        view = *(Model634ViewSnapshot *)Model_GetViewMetricsBuffer();
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixY(work->rings[i].rotation.vy, &unit.matrix);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].rotation.vx, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setVector(&work->rings[i].scale, work->rings[i].elapsed * 0x6000 / work->rings[i].duration,
                    work->rings[i].elapsed * 0x6000 / work->rings[i].duration, work->rings[i].elapsed * 0x6000 / work->rings[i].duration);
                if (work->rings[i].duration / 2 < work->rings[i].elapsed) {
                    work->rings[i].red = D_8013C3CC[work->command].red
                        * ((work->rings[i].duration - work->rings[i].elapsed) << 1) / work->rings[i].duration;
                    work->rings[i].green = D_8013C3CC[work->command].green
                        * ((work->rings[i].duration - work->rings[i].elapsed) << 1) / work->rings[i].duration;
                    work->rings[i].blue = D_8013C3CC[work->command].blue
                        * ((work->rings[i].duration - work->rings[i].elapsed) << 1) / work->rings[i].duration;
                }
                for (j = 0; j < 16; j++) {
                    setPolyGT4(&work->quad);
                    work->quad.tpage = work->tpage[0];
                    work->quad.clut = work->clut[0];
                    setSemiTrans(&work->quad, 1);
                    angle = j << 7;
                    setUV4(&work->quad, work->rings[i].u, work->rings[i].v + j * 127 / 16,
                        work->rings[i].u, work->rings[i].v + (j + 1) * 127 / 16,
                        work->rings[i].u + 127, work->rings[i].v + j * 127 / 16,
                        work->rings[i].u + 127, work->rings[i].v + (j + 1) * 127 / 16);
                    setRGB0(&work->quad, work->rings[i].red * rsin(angle) / 4096,
                        work->rings[i].green * rsin(angle) / 4096, work->rings[i].blue * rsin(angle) / 4096);
                    setRGB2(&work->quad, work->rings[i].red * rsin(angle) / 4096,
                        work->rings[i].green * rsin(angle) / 4096, work->rings[i].blue * rsin(angle) / 4096);
                    setRGB1(&work->quad, work->rings[i].red * rsin((j + 1) << 7) / 4096,
                        work->rings[i].green * rsin((j + 1) << 7) / 4096,
                        work->rings[i].blue * rsin((j + 1) << 7) / 4096);
                    setRGB3(&work->quad, work->rings[i].red * rsin((j + 1) << 7) / 4096,
                        work->rings[i].green * rsin((j + 1) << 7) / 4096,
                        work->rings[i].blue * rsin((j + 1) << 7) / 4096);
                    depth = RotAverage4(&work->ring[j][0], &work->ring[j + 1][0],
                        &work->ring[j][1], &work->ring[j + 1][1],
                        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                    }
                }
                work->rings[i].elapsed += step;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->spark_count; i++) {
            if (work->sparks[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
                RotMatrixZ(work->sparks[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i].rotation.vx
                    - (*(s16 *)&view.bytes[4] - 0x1000) * (work->slot * 2 - 1), &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->sparks[i].rotation.vy + angle - *(s16 *)&view.bytes[2], &unit.matrix);
                unit.matrix.t[0] = work->sparks[i].position.vx;
                unit.matrix.t[1] = work->sparks[i].position.vy;
                unit.matrix.t[2] = work->sparks[i].position.vz;
                unit.super = work->sparks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sparks[i].duration / 2 < work->sparks[i].elapsed) {
                    work->sparks[i].red = D_8013C3CC[work->command].spark_red
                        * ((work->sparks[i].duration - work->sparks[i].elapsed) << 1) / work->sparks[i].duration;
                    work->sparks[i].green = D_8013C3CC[work->command].spark_green
                        * ((work->sparks[i].duration - work->sparks[i].elapsed) << 1) / work->sparks[i].duration;
                    work->sparks[i].blue = D_8013C3CC[work->command].spark_blue
                        * ((work->sparks[i].duration - work->sparks[i].elapsed) << 1) / work->sparks[i].duration;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[1];
                work->flat.clut = work->clut[1];
                setSemiTrans(&work->flat, 1);
                setRGB0(&work->flat, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                setUV4(&work->flat, work->sparks[i].u, work->sparks[i].v, work->sparks[i].u + 31, work->sparks[i].v,
                    work->sparks[i].u, work->sparks[i].v + 31, work->sparks[i].u + 31, work->sparks[i].v + 31);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                work->sparks[i].u += 32;
                if (work->sparks[i].u == 128) {
                    work->sparks[i].u = 0;
                }
                work->sparks[i].elapsed += step;
                if (work->sparks[i].duration < work->sparks[i].elapsed) {
                    work->sparks[i].active = 0;
                    if (work->flags[4] == 0) {
                        work->flags[4] = 1;
                    }
                }
                if (work->flags[4] == 0) {
                    result = 4;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        if (work->flags[4] == 1) {
            result = 1;
            work->flags[4] = 2;
        }
        work->elapsed += step;
        if (inactive == work->ring_count + work->spark_count) {
            result = 2;
        }
    }
    return result;
}
