#include "../../types.h"
#include "variant802_entry.h"

/* Header 802's entry: the opposing slot glides over and back while a cloud
 * of particles rises around it, the slot scale swelling and shrinking. */

static const VECTOR D_8013C2CC = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

#define DRAW_PARTICLES()                                                                          \
    for (i = 0; i < work->count; i++) {                                                           \
        if (work->ages[i] < 24) {                                                                 \
            setPolyFT4(&packet);                                                                  \
            setRGB0(&packet, 128, 128, 128);                                                      \
            packet.tpage = work->texture[0] >> 16;                                                \
            packet.clut = work->texture[0];                                                       \
            setVector(&vertices[0], -work->config->size, -work->config->size, 0);                 \
            setVector(&vertices[1], work->config->size, -work->config->size, 0);                  \
            setVector(&vertices[2], -work->config->size, work->config->size, 0);                  \
            setVector(&vertices[3], work->config->size, work->config->size, 0);                   \
            setVector(&position, 0, 0, 0);                                                        \
            setVector(&rotation, 0, 0, 0);                                                        \
            scale.vx = 4096;                                                                      \
            scale.vy = 4096;                                                                      \
            scale.vz = 4096;                                                                      \
            GsSetLsMatrix(&base);                                                                 \
            RotTrans(&work->particles[i], (VECTOR *)matrix.t, &flag);                             \
            RotMatrix(&rotation, &matrix);                                                        \
            ScaleMatrix(&matrix, &scale);                                                         \
            GsSetLsMatrix(&matrix);                                                               \
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],           \
                (PSXLONG *)&packet.x0, (PSXLONG *)&packet.x1, (PSXLONG *)&packet.x2,              \
                (PSXLONG *)&packet.x3, &interpolation, &flag);                                    \
            setUV4(&packet, work->ages[i] / 3 * 32, 0, work->ages[i] / 3 * 32 + 31, 0,            \
                work->ages[i] / 3 * 32, 31, work->ages[i] / 3 * 32 + 31, 31);                     \
            if (depth >= 0) {                                                                     \
                Graphics_SubmitDrawModePacket((u32 *)&packet, ot, (u16)depth, 1);                 \
            }                                                                                     \
            work->ages[i]++;                                                                      \
            work->particles[i].vy -= work->speeds[i];                                             \
        }                                                                                         \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model802State *work;
    MATRIX base, matrix;
    POLY_FT4 packet;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    GsOT *ot;
    s32 step;
    s32 slot, side;
    s32 i;
    PSXLONG depth;
    GsCOORDUNIT *unit;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C2CC;
    work = (Model802State *)context;
    slot = Model_GetActiveSlotIndex();
    side = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013C2F8[command];
        Model_CopySlotPosition(slot, (u16 *)&position);
        Model_CopySlotPosition(slot ^ 1, (u16 *)&rotation);
        work->offset.vx = 0;
        work->offset.vy = 0;
        work->offset.vz = 0;
        work->velocity.vx = 0;
        work->velocity.vy = 350 / work->config->frames;
        work->velocity.vz = (rotation.vz - position.vz) / work->config->frames;
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&position);
        for (i = 0; i < work->config->count; i++) {
            setVector(&work->particles[i], rotation.vx + RANDOM_SPREAD(work->config->spread),
                rotation.vy - work->config->spread * (rand() % 4096) / 4096,
                rotation.vz + RANDOM_SPREAD(work->config->spread));
            work->speeds[i] = work->config->speed + (rand() - rand()) % (work->config->speed >> 1);
            work->ages[i] = 0;
        }
        for (i = 0; i < 1; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C2DC[i]);
        }
        work->scale = 4096;
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->hold_end) {
        Model_QueueCameraOrbitFromPreset(slot, 13, &rotation, -8);
    } else if (side > 0) {
        Model_QueueCameraOrbitFromPreset(2, 9, &rotation, 16);
    } else {
        Model_QueueCameraOrbitFromPreset(2, 11, &rotation, 16);
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
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    scale.vx = work->scale;
    scale.vy = work->scale;
    scale.vz = work->scale;
    Model_SetSlotScale(slot, (ModelSlotS32Quad *)&scale);
    if (work->elapsed < work->config->approach_end) {
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        work->elapsed += step;
        unit = (GsCOORDUNIT *)Model_GetCurrentDataEntry(slot);
        TransMatrix(&unit->matrix, &work->offset);
        Model_UpdateCoordinateUnitMatrix(unit, &scale);
        work->offset.vx += work->velocity.vx;
        work->offset.vy += work->velocity.vy;
        work->offset.vz += work->velocity.vz;
        PopMatrix();
        return 0;
    }
    if (work->elapsed < work->config->hold_end) {
        work->elapsed += step;
        PopMatrix();
        return 0;
    }
    if (work->elapsed < work->config->grow_end) {
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        DRAW_PARTICLES();
        work->elapsed += step;
        if (work->scale < 8191) {
            work->scale += work->config->scale_step;
        } else {
            work->count++;
            if (work->config->count < work->count) {
                work->count = work->config->count;
            }
        }
        PopMatrix();
        return 4;
    }
    if (work->phase == 0) {
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        DRAW_PARTICLES();
        if (work->scale > 4096) {
            work->count++;
            if (work->config->count < work->count) {
                work->count = work->config->count;
            }
            work->scale -= work->config->scale_step;
        }
        work->phase = 1;
        work->elapsed += step;
        PopMatrix();
        return 1;
    }
    if (work->elapsed < work->config->settle_end) {
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        DRAW_PARTICLES();
        if (work->scale > 4096) {
            work->count++;
            if (work->config->count < work->count) {
                work->count = work->config->count;
            }
            work->scale -= work->config->scale_step;
        }
        work->elapsed += step;
        PopMatrix();
        return 0;
    }
    if (work->elapsed < work->config->return_end) {
        DRAW_PARTICLES();
        unit = (GsCOORDUNIT *)Model_GetCurrentDataEntry(slot);
        TransMatrix(&unit->matrix, &work->offset);
        Model_UpdateCoordinateUnitMatrix(unit, &scale);
        work->phase = 2;
        work->offset.vx -= work->velocity.vx;
        work->offset.vy -= work->velocity.vy;
        work->offset.vz -= work->velocity.vz;
        work->elapsed += step;
        PopMatrix();
        return 0;
    }
    work->elapsed += step;
    PopMatrix();
    return 2;
}
