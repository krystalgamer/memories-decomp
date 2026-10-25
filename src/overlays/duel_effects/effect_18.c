#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_18.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "contour_burst_semantic_views.h"
#include "fallback_semantic_views.h"

#ifdef VERSION_EUROPE
#define EFFECT_18_RING_HEIGHT 106
#else
#define EFFECT_18_RING_HEIGHT 98
#endif

void DuelEffect_RunParticleContourBurst(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX working;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX saved;
    SVECTOR quad[4];
    CVECTOR color_copy;
    DuelEffect18Work *work;
    s32 i;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_ParticleContourBurstInitialScale;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_ParticleContourBurstConfigs[phase];
            for (i = 0; i < 3; i++) {
                DuelEffect_InitCircle32Vertices(work->config->radii[i], work->rings[i]);
            }
            for (i = 0; i < 2; i++) {
                if (work->config->variant == 1) {
                    DuelEffect_InitHalfStepXYRingVertices(24 + i * 6, 28 + i * 6, work->quads[i], 4);
                } else {
                    DuelEffect_InitHalfStepXYRingVertices(70 + i * 6, EFFECT_18_RING_HEIGHT + i * 6, work->quads[i], 4);
                }
            }
            DuelEffect_InitRandomVectorsUnscaled(16, work->rotations);
            DuelEffect_InitParticleVectors(work->config->particle_speed, work->positions,
                         work->velocities, 32);
            for (i = 0; i < 3; i++) {
                DuelEffect_InitCircle32Vertices(work->config->outer_radii[i], work->outer_rings[i]);
            }
            DuelEffect_SetGrayColor((u8 *)&work->color, 1);
            work->scale = 4096;
            work->frame = 0;
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            DuelEffect_DrawErrorCross(1);
        } else {
            DuelEffect_DrawErrorCross(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            gDuelEffect_bRequestFinished = 1;
        }
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        DuelEffect_InitBaseMatrix(&world);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color)) {
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            DuelEffect_SetTransformMatrix(&world, &saved, &position, &rotation, &scale, 1);
            DuelEffect_DrawQuadContour((u8 *)&work->color, work->quads[0], work->quads[1], 0, 1);
            if (work->frame >= work->config->delay) {
                gDuelEffect_pActiveRequest->progress = 1;
                for (i = 0; i < 32; i++) {
                    DuelEffect_InitOffsetQuadVertices(quad, &work->positions[i]);
                    DuelEffect_DrawRandomTileQuad((u8 *)&work->color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
                scale.vx = work->scale >> 3;
                scale.vy = work->scale >> 3;
                scale.vz = 4096;
                working = saved;
                ScaleMatrix(&working, &scale);
                GsSetLsMatrix(&working);
                DuelEffect_DrawGradientRingStrip((u8 *)&work->color, work->rings[0],
                             work->rings[1], work->rings[2], 0);
                color_copy = work->color;
                color_copy.r >>= 1;
                color_copy.g >>= 1;
                color_copy.b >>= 1;
                if (work->config->variant == 1) {
                    DuelEffect_DrawTexturedRingStrip((u8 *)&color_copy, work->outer_rings[0],
                                 work->outer_rings[1], work->outer_rings[2], 0);
                }
                scale.vx = work->scale;
                scale.vy = work->scale;
                scale.vz = 4096;
                working = saved;
                ScaleMatrix(&working, &scale);
                GsSetLsMatrix(&working);
                DuelEffect_DrawLayeredTileQuad((u8 *)&work->color, work->config->sprite_size,
                             0, &position);
                scale.vx = 4096;
                scale.vy = work->scale;
                scale.vz = 4096;
                for (i = 0; i < 16; i++) {
                    DuelEffect_SetTransformMatrix(&world, &saved, &position,
                                 &work->rotations[i], &scale, 2);
                    DuelEffect_DrawLayeredGradientTrapezoid((u8 *)&work->color, work->config->widths,
                                 work->config->height, 0);
                    applyVector(&work->rotations[i], 128, 128, 128, +=);
                }
                DuelEffect_DarkenColor((u8 *)&work->color, 15);
                if (work->scale < 0x7000) {
                    work->scale += 0x1000;
                }
            } else {
                DuelEffect_StepColorToTarget((u8 *)&work->color, work->config->color.r,
                             work->config->color.g, work->config->color.b, 31);
            }
        }
        work->frame += frame_step;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
