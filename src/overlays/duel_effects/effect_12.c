#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_12.h"
#include "geometry_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "render_state_semantic_views.h"
#include "field_burst_semantic_views.h"

void DuelEffect_RunStopDefenseBurst(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR sprite_rotation;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR quad[4];
    CVECTOR color_copy;
    DuelEffect12Work *work;
    s32 i;
    s32 frame_step;

    memset(&sprite_rotation, 0, sizeof(sprite_rotation));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_StopDefenseBurstInitialScale;
    work = buffer;
    if (phase >= 0) {
        work->config = &gDuelEffect_StopDefenseBurstConfig;
        for (i = 0; i < 3; i++) {
            DuelEffect_InitXZRingVertices(work->config->radii[i], work->config->radii[i],
                         work->config->ring_height * i, work->rings[i], 32);
        }
        copyVector(&work->origin, &gDuelEffect_DrawOffset);
        DuelEffect_InitParticleVectorsNegatedY(work->config->particle_speed, work->positions,
                     work->velocities, 32);
        work->scale = 4096;
        work->frame = 0;
        DuelEffect_SetRgbColor((u8 *)&work->color, work->config->color.r,
                     work->config->color.g, work->config->color.b);
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        DuelEffect_SetTransformMatrix(&world, &saved, &work->origin, &rotation, &scale, 3);
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color)) {
            for (i = 0; i < 32; i++) {
                DuelEffect_InitOffsetQuadVertices(quad, &work->positions[i]);
                DuelEffect_DrawRandomTileQuad((u8 *)&work->color, quad, 1);
                addVector(&work->positions[i], &work->velocities[i]);
            }
            color_copy = work->color;
            color_copy.r >>= 1;
            color_copy.g >>= 1;
            color_copy.b >>= 1;
            scale.vx = work->scale;
            scale.vy = 4096;
            scale.vz = work->scale;
            DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
            DuelEffect_DrawTexturedRingStrip((u8 *)&color_copy, work->rings[0],
                         work->rings[1], work->rings[2], 1);
            scale.vx = work->scale >> 1;
            scale.vy = 6144;
            scale.vz = work->scale >> 1;
            DuelEffect_SetScaledLocalScreenMatrix(&saved, &scale);
            DuelEffect_DrawTexturedRingStrip((u8 *)&work->color, work->rings[0],
                         work->rings[1], work->rings[2], 1);
            scale.vx = work->scale >> 1;
            scale.vy = work->scale >> 1;
            scale.vz = work->scale >> 1;
            DuelEffect_DrawLayeredTileQuad((u8 *)&color_copy, work->config->sprite_size,
                         64, &sprite_rotation);
            if (work->scale < 0x4000) {
                work->scale += 0x1000;
            } else {
                gDuelEffect_pActiveRequest->progress = 1;
                DuelEffect_DarkenColor((u8 *)&work->color, 15);
            }
        }
        work->frame += frame_step;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
