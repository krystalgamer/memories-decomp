#include "../../types.h"
#include "variant829_entry.h"
#include "../../game/screen_projection.h"

/* Header 829's entry: a camera sequence follows the duel, then streaks of
 * light leave a model part in fours and fly towards the other slot, each one
 * bursting into an animated sprite when it arrives, while sprites rain on the
 * other slot. The ambient level dips while the streaks are emitted and comes
 * back afterwards. The line depth gets its own local so the halved sum stays
 * in the argument register. */

static const VECTOR gModelVariant829_D1140 = {4096, 4096, 4096, 0};

s32 ModelVariant829_HandleRequest(u8 *context, s32 command)
{
    Model829State *work;
    MATRIX base, matrix;
    GsGLINE line;
    POLY_FT4 burst;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX sprite;
    PSXLONG flag, interpolation;
    s32 slot, step;
    GsOT *ot;
    s32 i;
    PSXLONG depth;
    s32 otz;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant829_D1140;
    work = (Model829State *)context;
    slot = Model_GetActiveSlotIndex();
    if (command >= 0) {
        work->config = &gModelVariant829_D116C[command];
        for (i = 0; i < 256; i++) {
            setVector(&work->streaks[i].tail, 0, 0, 0);
            setVector(&work->streaks[i].head, 0, 0, 0);
            work->moves[i] = 0;
            work->arrived[i] = 0;
            work->frames[i] = 0;
        }
        setVector(&work->step, 0, 0, 0);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&position);
        for (i = 0; i < 64; i++) {
            setVector(&work->sparks[i], position.vx / 2 * ((rand() - rand()) % 4096) / 4096,
                position.vy / 2 * ((rand() - rand()) % 4096) / 4096,
                position.vz / 2 * ((rand() - rand()) % 4096) / 4096);
            work->spark_frames[i] = 0;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant829_D1150[i]);
        }
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->streak_count = 0;
        work->spark_count = 0;
        work->camera = 1;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->camera == 0) {
        if (work->elapsed < work->config->camera_times[0]) {
            Model_QueueCameraOrbitFromPreset(slot, 2, &rotation, work->config->camera_args[0]);
        } else if (work->elapsed < work->config->camera_times[1]) {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rotation, work->config->camera_args[1]);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rotation, work->config->camera_args[2]);
        }
    }
    if (work->camera == 1) {
        if (work->elapsed < work->config->camera_times[0]) {
            Model_QueueCameraOrbitFromPreset(slot, 3, &rotation, work->config->camera_args[0]);
        } else if (work->elapsed < work->config->camera_times[1]) {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rotation, work->config->camera_args[1]);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rotation, work->config->camera_args[2]);
        }
    }
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
    if (work->phase == 0) {
        Model_CopySlotPosition(slot, (u16 *)&position);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&rotation);
        work->step.vx = 0;
        work->step.vy = (rotation.vy - position.vy) / 6;
        work->step.vz = (rotation.vz - position.vz) / 6;
        work->phase = 1;
    }
    if (work->elapsed < work->config->emit_end && work->streak_count + 4 < 256) {
        GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->part), &matrix);
        for (i = 0; i < 4; i++) {
            setVector(&work->streaks[work->streak_count].tail,
                work->config->spread * ((rand() - rand()) % 4096) / 4096, 0,
                work->config->spread * ((rand() - rand()) % 4096) / 4096);
            copyVector(&work->streaks[work->streak_count].head, &work->streaks[work->streak_count].tail);
            applyVector(&work->streaks[work->streak_count].tail, matrix.t[0], matrix.t[1], matrix.t[2], +=);
            applyVector(&work->streaks[work->streak_count].head, matrix.t[0], matrix.t[1], matrix.t[2], +=);
            work->streak_count++;
        }
        work->spark_count++;
    }
    line.attribute = 0x50000000;
    setVector(&rotation, 0, 0, 0);
    setVector(&position, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    MulMatrix2(&base, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setPolyFT4(&burst);
    burst.tpage = work->texture[0] >> 16;
    burst.clut = work->texture[0];
    setRGB0(&burst, 32, 32, 32);
    for (i = 0; i < work->streak_count; i++) {
        if (!work->arrived[i]) {
            GsSetLsMatrix(&matrix);
            line.r0 = work->config->r;
            line.g0 = work->config->g;
            line.b0 = work->config->b;
            line.r1 = 0;
            line.g1 = 0;
            line.b1 = 0;
            otz = (RotTransPers(&work->streaks[i].head, (PSXLONG *)&line.x0, &interpolation, &flag)
                + RotTransPers(&work->streaks[i].tail, (PSXLONG *)&line.x1, &interpolation, &flag)) / 2;
            if (otz >= 0) {
                GsSortGLine(&line, ot, (u16)otz);
            }
            if (work->streaks[i].head.vy < 0) {
                addVector(&work->streaks[i].head, &work->step);
                if (work->moves[i]++ >= 4) {
                    addVector(&work->streaks[i].tail, &work->step);
                }
                if (work->streaks[i].head.vy >= 0) {
                    work->arrived[i] = 1;
                    work->streaks[i].head.vy = 0;
                }
            }
        }
        if (work->arrived[i]) {
            GsSetLsMatrix(&base);
            RotTrans(&work->streaks[i].head, (VECTOR *)sprite.t, &flag);
            RotMatrix(&rotation, &sprite);
            ScaleMatrix(&sprite, &scale);
            GsSetLsMatrix(&sprite);
            if (work->frames[i] < 16) {
                setVector(&vertices[0], -work->config->size, -work->config->size, 0);
                setVector(&vertices[1], work->config->size, -work->config->size, 0);
                setVector(&vertices[2], -work->config->size, work->config->size, 0);
                setVector(&vertices[3], work->config->size, work->config->size, 0);
                addVector(&vertices[0], &work->streaks[i].head);
                addVector(&vertices[1], &work->streaks[i].head);
                addVector(&vertices[2], &work->streaks[i].head);
                addVector(&vertices[3], &work->streaks[i].head);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3,
                    &interpolation, &flag);
                setUV4(&burst, work->frames[i] / 2 % 4 * 32, work->frames[i] / 8 * 32,
                    work->frames[i] / 2 % 4 * 32 + 31, work->frames[i] / 8 * 32,
                    work->frames[i] / 2 % 4 * 32, work->frames[i] / 8 * 32 + 31,
                    work->frames[i] / 2 % 4 * 32 + 31, work->frames[i] / 8 * 32 + 31);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
                }
                work->frames[i]++;
            }
        }
    }
    Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setRGB0(&burst, work->config->spark_r, work->config->spark_g, work->config->spark_b);
    for (i = 0; i < work->spark_count; i++) {
        if (work->spark_frames[i] < 16) {
            setVector(&vertices[0], -work->config->size, -work->config->size, 0);
            setVector(&vertices[1], work->config->size, -work->config->size, 0);
            setVector(&vertices[2], -work->config->size, work->config->size, 0);
            setVector(&vertices[3], work->config->size, work->config->size, 0);
            addVector(&vertices[0], &work->sparks[i]);
            addVector(&vertices[1], &work->sparks[i]);
            addVector(&vertices[2], &work->sparks[i]);
            addVector(&vertices[3], &work->sparks[i]);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3,
                &interpolation, &flag);
            setUV4(&burst, work->spark_frames[i] / 2 % 4 * 32, work->spark_frames[i] / 8 * 32,
                work->spark_frames[i] / 2 % 4 * 32 + 31, work->spark_frames[i] / 8 * 32,
                work->spark_frames[i] / 2 % 4 * 32, work->spark_frames[i] / 8 * 32 + 31,
                work->spark_frames[i] / 2 % 4 * 32 + 31, work->spark_frames[i] / 8 * 32 + 31);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
            }
            work->spark_frames[i]++;
            work->sparks[i].vy -= work->config->fall;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 3 && work->frames[work->streak_count - 1] >= 16) {
        return 2;
    }
    if (work->elapsed > work->config->emit_end) {
        if (work->phase < 2) {
            work->phase = 2;
            return 1;
        }
    } else if (work->phase < 2) {
        return 4;
    }
    return 0;
}
