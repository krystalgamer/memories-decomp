#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_19.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "card_flash_semantic_views.h"
#include "../../game/duel_effect_object_pool.h"

void DuelEffect_RunExpandingRingBurst(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color_copy;
    SVECTOR quad[4];
    DuelEffect19Work *work;
    s32 i;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_ExpandingRingBurstInitialScale;
    work = buffer;
    if (phase >= 0) {
        work->widths[0] = 64;
        work->widths[1] = 67;
        work->widths[2] = 68;
        DuelEffect_InitRandomVectorsUnscaled(64, work->rotations);
        for (i = 0; i < 3; i++) {
            DuelEffect_InitCircle32Vertices(work->widths[i], work->rings[i]);
        }
        DuelEffect_InitParticleVectors(6, work->positions, work->velocities, 32);
        work->scale = 4096;
        work->frame = 0;
        work->fade_started = 0;
        work->spawned = 0;
        DuelEffect_SetRgbColor((u8 *)&work->color, 31, 28, 30);
    } else {
        work->widths[0] = 1;
        work->widths[1] = 2;
        Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        DuelEffect_InitBaseMatrix(&world);
        DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 1);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color)) {
            if (work->fade_started == 1) {
                for (i = 0; i < 32; i++) {
                    DuelEffect_InitOffsetQuadVertices(quad, &work->positions[i]);
                    DuelEffect_DrawRandomTileQuad((u8 *)&work->color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
            }
            scale.vx = work->scale;
            scale.vy = work->scale;
            scale.vz = 0;
            DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
            DuelEffect_DrawLayeredTileQuad((u8 *)&work->color, 12, 0, &position);
            scale.vx = work->scale >> 2;
            scale.vy = work->scale >> 2;
            scale.vz = 0;
            DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
            DuelEffect_DrawGradientRingStrip((u8 *)&work->color, work->rings[0],
                         work->rings[1], work->rings[2], 0);
            scale.vx = 4096;
            scale.vy = 4096 + (work->scale >> 2);
            scale.vz = 4096;
            if (work->fade_started == 0) {
                for (i = 0; i < work->spawned; i++) {
                    DuelEffect_SetTransformMatrix(&world, &saved, &position,
                                 &work->rotations[i], &scale, 2);
                    color_copy = work->color;
                    color_copy.r >>= 2;
                    color_copy.g >>= 2;
                    color_copy.b >>= 2;
                    DuelEffect_DrawLayeredGradientTrapezoid((u8 *)&work->color, work->widths, 48, 0);
                    work->rotations[i].vx += 32;
                    work->rotations[i].vy += 32;
                    work->rotations[i].vz += 32;
                }
                work->spawned++;
                if (work->spawned > 64) {
                    work->spawned = 64;
                }
            }
            if (work->fade_started == 0) {
                work->fade_started =
                    DuelEffect_StepColorToTarget((u8 *)&work->color, 255, 224, 240, 4);
                if (work->scale < 0x5000) {
                    work->scale += 0x100;
                } else {
                    work->scale = 0x5000;
                }
            } else {
                gDuelEffect_pActiveRequest->progress = 1;
                DuelEffect_DrawFadingFullscreenColor((u8 *)&work->color, 8);
            }
        }
        work->frame++;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
