#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_1.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "drawing_semantic_views.h"
#include "layered_semantic_views.h"
#include "fusion_attack_semantic_views.h"

void func_80157794(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    SVECTOR translation;
    VECTOR scale;
    SVECTOR quad[4];
    CVECTOR color_copy;
    DuelEffect1Work *work;
    s32 i;
    s32 frame_step;

    memset(&rotation, 0, sizeof(rotation));
    memset(&translation, 0, sizeof(translation));
    scale = gDuelEffect_FusionInitialScale;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_FusionConfigs[phase];
            Model_SetFrameStepOverride(1);
            DuelEffect_InitRandomVectorsUnscaled(12, work->rotations);
            DuelEffect_InitParticleVectors(work->config->particle_speed, work->positions,
                         work->velocities, 64);
            DuelEffect_SetGrayColor((u8 *)&work->base_color, 1);
            DuelEffect_SetGrayColor((u8 *)&work->beam_color, 0);
            DuelEffect_SetGrayColor((u8 *)&work->screen_color, 0);
            work->rotation_step = work->config->initial_rotation_step;
            work->frame = 0;
            work->tick = 0;
            work->stage = 0;
            work->growth = 0;
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
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
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->base_color)) {
            scale.vx = 4096 + (work->growth >> 4);
            scale.vy = 4096 + (work->growth >> 4);
            scale.vz = 4096;
            setVector(&rotation, 0, 0,
                      (work->rotation_step * work->tick) & 0xFFF);
            DuelEffect_SetTransformMatrix(&world, &saved, &translation, &rotation, &scale, 0);
            DuelEffect_DrawFixedTileQuad((u8 *)&work->base_color);
            if (work->frame < work->config->first_end) {
                DuelEffect_StepColorToTarget((u8 *)&work->base_color, 128, 128, 128, 8);
                work->rotation_step += work->config->rotation_acceleration;
            } else if (work->frame < work->config->second_end) {
                DuelEffect_StepColorToTarget((u8 *)&work->beam_color, work->config->color.r,
                             work->config->color.g, work->config->color.b, 15);
                if (work->growth < 0x7000) {
                    work->growth += 0x400;
                }
            } else {
                if (work->stage == 0) {
                    work->stage = 1;
                    DuelEffect_SetGrayColor((u8 *)&work->screen_color, 255);
                    work->rotation_step = work->config->initial_rotation_step;
                }
                DuelEffect_DarkenColor((u8 *)&work->base_color, 15);
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->beam_color)) {
            scale.vx = 4096 + (work->growth >> 2);
            scale.vy = 4096 + (work->growth >> 2);
            scale.vz = 4096;
            setVector(&rotation, 0, 0,
                      (work->config->beam_rotation_step * work->tick) & 0xFFF);
            DuelEffect_SetTransformMatrix(&world, &saved, &translation, &rotation, &scale, 0);
            color_copy = work->beam_color;
            color_copy.r >>= 2;
            color_copy.g >>= 2;
            color_copy.b >>= 2;
            DuelEffect_DrawLayeredTileQuad((u8 *)&color_copy, work->config->sprite_size,
                         0, &translation);
            if (work->stage == 0) {
                scale.vx = 4096;
                scale.vy = 4096 + (work->growth >> 3);
                scale.vz = 4096;
                for (i = 0; i < 12; i++) {
                    DuelEffect_SetTransformMatrix(&world, &saved, &translation, &work->rotations[i],
                                 &scale, 2);
                    /* The strip still uses the original color after this copy. */
                    color_copy = work->beam_color;
                    color_copy.r >>= 2;
                    color_copy.g >>= 2;
                    color_copy.b >>= 2;
                    DuelEffect_DrawLayeredGradientTrapezoid((u8 *)&work->beam_color, work->config->widths,
                                 work->config->height, 0);
                    applyVector(&work->rotations[i],
                                work->config->beam_rotation_step,
                                work->config->beam_rotation_step,
                                work->config->beam_rotation_step, +=);
                }
            } else {
                setVector(&scale, 4096, 4096, 4096);
                setVector(&rotation, 0, 0,
                          (work->rotation_step * (work->tick << 1)) & 0xFFF);
                DuelEffect_SetTransformMatrix(&world, &saved, &translation, &rotation, &scale, 0);
                for (i = 0; i < 64; i++) {
                    DuelEffect_InitOffsetQuadVertices(quad, &work->positions[i]);
                    DuelEffect_DrawRandomTileQuad((u8 *)&work->beam_color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
            }
            if (work->stage != 0) {
                DuelEffect_DarkenColor((u8 *)&work->beam_color, 8);
            }
        }
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->screen_color)) {
            DuelEffect_DrawFadingFullscreenColor((u8 *)&work->screen_color, 15);
            gDuelEffect_pActiveRequest->field_1D = 1;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->base_color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->beam_color) &&
            (u16)DuelEffect_IsColorBlack((u8 *)&work->screen_color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
