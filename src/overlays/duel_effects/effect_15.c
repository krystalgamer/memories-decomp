#include "../../types.h"
#include "color_semantic_views.h"
#include "effect_15.h"
#include "fallback_semantic_views.h"
#include "geometry_semantic_views.h"
#include "packet_semantic_views.h"
#include "texture_state_semantic_views.h"
#include "exodia_semantic_views.h"
#include "selected_cards_semantic_views.h"

void DuelEffect_RunSelectedCardOverlay(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR translation;
    SVECTOR rotation;
    VECTOR scale;
    POLY_FT4 polygon;
    SVECTOR quad[4];
    POLY_FT4 *packet;
    DuelEffect15Work *work;
    s32 i;

    memset(&translation, 0, sizeof(translation));
    memset(&rotation, 0, sizeof(rotation));
    scale = gDuelEffect_SelectedCardOverlayInitialScale;
    packet = &polygon;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 8) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &gDuelEffect_SelectedCardOverlayConfigs[phase];
            if (work->config->selector == 999) {
                Duel_CollectFieldRowCardObjects(gDuelEffect_CardObjectScratchWords, 1);
            } else {
                Duel_CollectMatchingFieldCardObjects(gDuelEffect_CardObjectScratchWords,
                                                     work->config->selector);
            }
            work->count = 0;
            for (i = 0; gDuelEffect_CardObjectScratchWords[i] != 0; i++) {
                work->count++;
            }
            work->visible = 1;
            work->state = 0;
            work->timer = 0;
            work->scale = 4096;
            DuelEffect_SetGrayColor((u8 *)&work->color, 1);
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
        Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        scale.vx = work->scale;
        scale.vy = work->scale;
        scale.vz = work->scale;
        if ((u16)DuelEffect_HasNonzeroColor((u8 *)&work->color) && work->visible != 0) {
            setPolyFT4(packet);
            packet->tpage = gDuelEffect_TexturePageClutPairs.pairs[13][0];
            packet->clut = gDuelEffect_TexturePageClutPairs.pairs[13][1];
            setRGB0(packet, work->color.r, work->color.g, work->color.b);
            if (work->scale > 1024) {
                setUV4(packet, 32, 0, 95, 0, 32, 63, 95, 63);
            } else {
                setUV4(packet, 32, 64, 95, 64, 32, 127, 95, 127);
            }
            DuelEffect_InitSquareVertices(quad, 128);
            for (i = 0; i < work->count; i++) {
                setVector(&translation,
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_30,
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_30.h.field_32,
                    ((DisplayObject *)gDuelEffect_CardObjectScratchWords[i])->field_34.h.field_34);
                DuelEffect_SetTransformMatrix(&world, &saved, &translation, &rotation, &scale, 0);
                DuelEffect_ProjectSubmitFt4(packet, quad, 32, 1);
                DuelEffect_ProjectSubmitFt4(packet, quad, 32, 1);
            }
            if (work->scale > 1024) {
                work->scale -= 512;
            }
            if (work->state == 0) {
                work->state = DuelEffect_StepColorToTarget((u8 *)&work->color,
                    work->config->color.r, work->config->color.g,
                    work->config->color.b, 15);
            }
            if (work->state == 2 && work->scale == 1024) {
                DuelEffect_DarkenColor((u8 *)&work->color, 31);
            }
        }
        if (work->state == 1 && work->timer < 16) {
            work->timer++;
            if (!(work->timer % 4)) {
                if (work->visible == 1) {
                    work->visible = 0;
                } else {
                    work->visible = 1;
                }
            }
        } else if (work->timer >= 16) {
            work->state = 2;
        }
        PopMatrix();
        if ((u16)DuelEffect_IsColorBlack((u8 *)&work->color)) {
            gDuelEffect_bRequestFinished = 1;
        }
    }
}
