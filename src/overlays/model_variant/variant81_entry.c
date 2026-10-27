#include "../../types.h"
#include "variant81_entry.h"

/* Header 81's entry: a ribbon of gradient quads twists from a model part
 * towards a point in front of the opposing slot, one segment longer every
 * two frames.  Four textured cylinders then open around the opposing slot
 * one after another, sweeping their quads in while they turn, and finally
 * grow and fade out; at the end the opposing slot's tint target is reset. */

static const VECTOR D_8013C9B0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model81State *work;
    MATRIX light;
    MATRIX matrix;
    POLY_GT4 gt4;
    SVECTOR rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR ends[2];
    DVECTOR screen[66];
    u16 depths[66];
    u16 projections[66];
    u16 flags[66];
    PSXLONG p;
    PSXLONG flag;
    Model81Config *config;
    GsOT *ot;
    s32 direction;
    s32 i, j, time, size, otz;
    u8 red, green, blue;
    SVECTOR *bottom, *top;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C9B0;
    work = (Model81State *)context;
    direction = Model_GetActiveSlotIndex() * 2 - 1;
    if (command >= 0) {
        config = work->config = &D_8013C9DC[command];
        for (i = 0; i < config->count; i++) {
            work->offsets[i] = config->wave_amplitude * csin(config->wave_period * i / config->segments) / 4096;
            work->angles[i] = config->twist * i / config->count;
        }
        bottom = work->bottom;
        top = work->top;
        for (i = 0; i < 33; bottom++, top++, i++) {
            bottom->vx = config->ring_radius * ccos(i * 128) / 4096;
            bottom->vy = -config->ring_height / 2 + config->ring_height / 4 * csin(i * 256) / 4096;
            bottom->vz = config->ring_radius * csin(i * 128) / 4096;
            top->vx = config->ring_radius * ccos(i * 128) / 4096;
            top->vy = config->ring_height / 2 + config->ring_height / 4 * csin(i * 256) / 4096;
            top->vz = config->ring_radius * csin(i * 128) / 4096;
        }
        work->frames[0] = 0;
        for (i = 1; i < config->count; i++) {
            work->frames[i] = rand() % 5 + 2;
        }
        work->texture = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C9C0);
        for (i = 0; i < config->segments / 2; i++) {
            work->colours[i].r = 0;
            work->colours[i].g = i * 2 * 255 / config->segments;
            work->colours[i].b = (config->segments - i * 2) * 255 / config->segments;
            work->colours[i + config->segments / 2].r = i * 2 * 255 / config->segments;
            work->colours[i + config->segments / 2].g = (config->segments - i * 2) * 255 / config->segments;
            work->colours[i + config->segments / 2].b = 0;
        }
        work->colours[config->segments].r = 0;
        work->colours[config->segments].g = 0;
        work->colours[config->segments].b = 0;
        work->done = 0;
        work->elapsed = -config->start_delay;
        return 0;
    }
    config = work->config;
    ot = func_80058F10();
    if (work->elapsed < 0) {
        work->elapsed += Model_GetFrameStep();
        return 0;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    SetPolyGT4(&gt4);
    gt4.tpage = work->texture >> 16;
    gt4.clut = work->texture;
    GsSetLsMatrix(&light);
    GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), config->part), &matrix);
    i = work->elapsed / 2;
    i = i < 0 ? 0 : (i > config->segments ? config->segments : i);
    j = (config->segments - i + work->elapsed / 2) % config->count;
    ends[0].vx = -matrix.t[0] * i / config->segments
        - config->swirl_radius * ccos(work->angles[j]) / 4096 * i / config->segments;
    ends[0].vy = matrix.t[1] + work->offsets[j] * i / config->segments
        + (-350 - matrix.t[1]) * i / config->segments
        - config->swirl_radius * csin(work->angles[j]) / 4096 * i / config->segments;
    ends[0].vz = matrix.t[2] + (direction * 900 - matrix.t[2]) * i / config->segments;
    ends[1].vx = -matrix.t[0] * i / config->segments
        + config->swirl_radius * ccos(work->angles[j]) / 4096 * i / config->segments;
    ends[1].vy = matrix.t[1] + work->offsets[j] * i / config->segments
        + (-350 - matrix.t[1]) * i / config->segments
        + config->swirl_radius * csin(work->angles[j]) / 4096 * i / config->segments;
    ends[1].vz = matrix.t[2] + (direction * 900 - matrix.t[2]) * i / config->segments;
    GsSetLsMatrix(&light);
    RotTransPers(&ends[0], (PSXLONG *)&gt4.x0, &p, &flag);
    RotTransPers(&ends[1], (PSXLONG *)&gt4.x2, &p, &flag);
    for (; i > 0; i--) {
        j = (config->segments - i + work->elapsed / 2) % config->count;
        ends[0].vx = -matrix.t[0] * i / config->segments
            - config->swirl_radius * ccos(work->angles[j]) / 4096 * i / config->segments;
        ends[0].vy = matrix.t[1] + work->offsets[j] * i / config->segments
            + (-350 - matrix.t[1]) * i / config->segments
            - config->swirl_radius * csin(work->angles[j]) / 4096 * i / config->segments;
        ends[0].vz = matrix.t[2] + (direction * 900 - matrix.t[2]) * i / config->segments;
        ends[1].vx = -matrix.t[0] * i / config->segments
            + config->swirl_radius * ccos(work->angles[j]) / 4096 * i / config->segments;
        ends[1].vy = matrix.t[1] + work->offsets[j] * i / config->segments
            + (-350 - matrix.t[1]) * i / config->segments
            + config->swirl_radius * csin(work->angles[j]) / 4096 * i / config->segments;
        ends[1].vz = matrix.t[2] + (direction * 900 - matrix.t[2]) * i / config->segments;
        GsSetLsMatrix(&light);
        otz = RotTransPers(&ends[0], (PSXLONG *)&gt4.x1, &p, &flag);
        otz += RotTransPers(&ends[1], (PSXLONG *)&gt4.x3, &p, &flag);
        otz /= 2;
        if (work->frames[j] == 0 || work->frames[j] == 6) {
            setUV4(&gt4, work->frames[j] * 32, 0, work->frames[j] * 32 + 63, 0,
                work->frames[j] * 32, 63, work->frames[j] * 32 + 63, 63);
        } else {
            setUV4(&gt4, work->frames[j] * 32, 0, work->frames[j] * 32 + 31, 0,
                work->frames[j] * 32, 63, work->frames[j] * 32 + 31, 63);
        }
        gt4.r0 = work->colours[i].r;
        gt4.g0 = work->colours[i].g;
        gt4.b0 = work->colours[i].b;
        gt4.r1 = work->colours[i - 1].r;
        gt4.g1 = work->colours[i - 1].g;
        gt4.b1 = work->colours[i - 1].b;
        gt4.r2 = work->colours[i].r;
        gt4.g2 = work->colours[i].g;
        gt4.b2 = work->colours[i].b;
        gt4.r3 = work->colours[i - 1].r;
        gt4.g3 = work->colours[i - 1].g;
        gt4.b3 = work->colours[i - 1].b;
        SetSemiTrans(&gt4, 1);
        if (work->elapsed + (config->segments - i) < config->trail_end && otz >= 0 && flag >= 0) {
            GsSortPoly(&gt4, ot, otz);
        }
        gt4.x0 = gt4.x1;
        gt4.y0 = gt4.y1;
        gt4.x2 = gt4.x3;
        gt4.y2 = gt4.y3;
    }
    for (j = 0; j < 4; j++) {
        time = work->elapsed - (config->segments * 2 + j * config->spin_duration);
        if (time >= 0 && time < config->spin_duration + config->grow_duration) {
            GsSetLsMatrix(&light);
            Model_CopySlotPosition(Model_GetActiveSlotIndex() ^ 1, (u16 *)&position);
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            rotation.vy = time < config->spin_duration ? (time << 12) / config->spin_duration : 0;
            size = time >= config->spin_duration ? ((time - config->spin_duration) << 12) / config->grow_duration + 4096 : 4096;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            GsSetLsMatrix(&light);
            RotTrans(&position, (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            MulMatrix2(&light, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            RotTransPersN(work->bottom, screen, depths, projections, flags, 66);
            if (time < config->spin_duration) {
                size = (time << 5) / config->spin_duration;
            } else {
                size = 32;
            }
            for (i = 0; i < size; i++) {
                gt4.x0 = screen[i].vx;
                gt4.y0 = screen[i].vy;
                gt4.x1 = screen[i + 1].vx;
                gt4.y1 = screen[i + 1].vy;
                gt4.x2 = screen[i + 33].vx;
                gt4.y2 = screen[i + 33].vy;
                gt4.x3 = screen[i + 34].vx;
                gt4.y3 = screen[i + 34].vy;
                SetSemiTrans(&gt4, 1);
                otz = AverageZ4(depths[i], depths[i + 1], depths[i + 33], depths[i + 34]);
                flag = (flags[i] | flags[i + 1] | flags[i + 33] | flags[i + 34]) & 0x20;
                setUV4(&gt4, i * 8, 0, i * 8 + 7, 0, i * 8, 63, i * 8 + 7, 63);
                if (time < config->spin_duration) {
                    gt4.r0 = work->colours[i].r;
                    gt4.g0 = work->colours[i].g;
                    gt4.b0 = work->colours[i].b;
                    gt4.r1 = work->colours[i + 1].r;
                    gt4.g1 = work->colours[i + 1].g;
                    gt4.b1 = work->colours[i + 1].b;
                    gt4.r2 = work->colours[i].r;
                    gt4.g2 = work->colours[i].g;
                    gt4.b2 = work->colours[i].b;
                    gt4.r3 = work->colours[i + 1].r;
                    gt4.g3 = work->colours[i + 1].g;
                    gt4.b3 = work->colours[i + 1].b;
                } else {
                    red = work->colours[i].r * (config->grow_duration + config->spin_duration - time) / config->grow_duration;
                    green = work->colours[i].g * (config->grow_duration + config->spin_duration - time) / config->grow_duration;
                    blue = work->colours[i].b * (config->grow_duration + config->spin_duration - time) / config->grow_duration;
                    setRGB0(&gt4, red, green, blue);
                    setRGB2(&gt4, red, green, blue);
                    red = work->colours[i + 1].r * (config->grow_duration + config->spin_duration - time) / config->grow_duration;
                    green = work->colours[i + 1].g * (config->grow_duration + config->spin_duration - time) / config->grow_duration;
                    blue = work->colours[i + 1].b * (config->grow_duration + config->spin_duration - time) / config->grow_duration;
                    setRGB1(&gt4, red, green, blue);
                    setRGB3(&gt4, red, green, blue);
                }
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(&gt4, ot, otz);
                }
            }
        }
    }
    PopMatrix();
    work->elapsed += Model_GetFrameStep();
    if (work->elapsed < config->segments * 2) {
        return 0;
    }
    if (work->elapsed < config->segments * 2 + config->spin_duration * 4 + config->grow_duration) {
        return 4;
    }
    Model_SetSlotTintTarget(~Model_GetActiveSlotIndex() & 1, 0, 128, 128, 128);
    if (work->done == 0) {
        work->done++;
        return 1;
    } else {
        return 2;
    }
}
