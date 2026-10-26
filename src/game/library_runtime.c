#define D_8009B118_IN_DATA
#define D_8009B0C0_IN_DATA_VOLATILE
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "text_box_blocking_build.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "model_get_slot_readiness.h"
#include "camera_view.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "image_transfer_rects.h"
#include "library_runtime.h"
#include "library_reference_y.h"
#include "model_copy_slot_position.h"
#include "model_update_view_metrics.h"
#include "sound_voice_data.h"
#include "view_state.h"
#include "../psyq/rand.h"
#include "model_slot_queries.h"
#include "../unmatched.h"
#include "fade_state_bytes.h"
#include "file_transfer_buffer.h"
#include "model_slot_control.h"
#include "library_grid_cursor.h"
#include "library_card_grid.h"
#include "library_refresh_selected_card_text.h"
#include "save_data.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_core.h"
#include "card_grid.h"
#include "display_object_layout.h"
#include "duel_effect_set_entry_range_layout.h"
#include "main_services.h"
#include "text_box_lifecycle.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "display_object_helpers.h"
#include "duel_deck_lookup.h"
#include "library_create_grid_section_sprites.h"
#include "sound.h"
#include "text_render_state.h"
#include "text_staging.h"
#include "text_message_staging.h"
#include "library_update_grid_scroll.h"
#include "text_box_runtime.h"
#include "display_object_config.h"
#include "display_object.h"
#include "display_object_interpolation.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_resource_setup.h"
#include "fade.h"
#include "card_preview_objects.h"
#include "library_draw_wireframe_globe.h"
#include "library_fading_card_outline.h"
#include "text_box_update_channels.h"
#include "model_set_slot_shadow_enabled.h"
#include "input.h"
#include "model_effect_coefficients.h"
#include "model.h"
#include "model_load_step.h"
#include "model_scene_controllers.h"
#include "model_slot_properties.h"
#include "model_slot_state_updates.h"
#include "model_slot_support.h"
#include "model_state_setters.h"
#include "../psyq/libgs.h"
#include "view_state_current.h"
#include "graphics_packet_submission.h"
#include "display_object_projection.h"
#include "gpu_packets.h"

/* The draw callback Library_CreateFadingCardOutline installs at 0x4C on the object it
   builds: it fades the object out over its own field_60 countdown, projects
   it, and emits a four-vertex flat polyline (one colour word, length 6)
   and then, with the length cut to 3 and x3/y3 copied into x1/y1, a single
   flat line from the first vertex to the last, both through Graphics_SubmitDrawModePacket.
   The record is the canonical DisplayObject.

   The scratchpad side is typed as well: a LINE_F4 at 0x1F8002A0, the four
   SVECTOR corners RotAverage4 projects into its x0..x3, and the
   ProjectionOut DisplayObject_ProjectRotationFacing leaves at 0x1F800398. That
   is load-bearing. Written through byte pointers, those stores are not struct
   references, and
   GCC floats the object's struct-member reads of 0x32 and 0x3E across them
   where the target does not. With both sides typed the plain member reads
   keep their place.

   b0[1].vx and b0[2].vy are b1->vx and b2->vy: the target addresses those
   two stores from the first corner's base, and spelling them through b1 and
   b2 changes the code. */
void Library_DrawFadingCardOutline(DisplayObject *o, GsOT *ot)
{
    s32 sp28;
    s32 sp2C;
    LINE_F4 *q;
    struct ProjectionOut *r;
    SVECTOR *b0;
    SVECTOR *b1;
    SVECTOR *b2;
    SVECTOR *b3;
    s32 n;
    s32 e;
    s32 m;
    s32 z;
    s32 k;
    s32 v;
    s32 w;

    n = *(u16 *)&o->field_60 - 0x10;
    o->field_60 = n;
    if ((s16)n <= 0) {
        DisplayObject_ReleaseIfPresent(o);
        return;
    }

    r = (struct ProjectionOut *)0x1F800398;
    ((u8 *)&o->rgb)[2] = *(u8 *)&o->field_60;
    DisplayObject_ProjectRotationFacing(o,
                  *(s16 *)&o->field_30.h.field_30 + *(s16 *)&o->field_18,
                  *(s16 *)&o->field_30.h.field_32 + *(s16 *)&o->field_1A,
                  r);

    q = (LINE_F4 *)0x1F8002A0;
    b0 = (SVECTOR *)0x1F800300;
    m = 0x55555555;
    b2 = (SVECTOR *)0x1F800310;
    b3 = (SVECTOR *)0x1F800318;
    b1 = (SVECTOR *)0x1F800308;

    z = *(s32 *)&o->rgb;
    setlen(q, 6);
    q->pad = m;
    *(s32 *)&q->r0 = z;
    q->code = 0x4C;

    v = o->field_30.h.field_30 - (u16)r->x;
    b2->vx = v;
    b0->vx = v;
    v += o->field_3C.h.field_3C;
    b3->vx = v;
    b0[1].vx = v;

    w = o->field_30.h.field_32 - (u16)r->y;
    b1->vy = w;
    b0->vy = w;
    e = o->field_3C.h.field_3E;
    b3->vz = 0;
    b2->vz = 0;
    b1->vz = 0;
    b0->vz = 0;

    w += e;
    b3->vy = w;
    b0[2].vy = w;

    RotAverage4(
        b0, b1, b2, b3,
        (PSXLONG *)&q->x0, (PSXLONG *)&q->x1,
        (PSXLONG *)&q->x3, (PSXLONG *)&q->x2,
        (PSXLONG *)&sp28, (PSXLONG *)&sp2C
    );

    Graphics_SubmitDrawModePacket((u32 *)q, ot, *(u16 *)&o->ot_depth, 1);
    setlen(q, 3);
    q->code = 0x40;
    k = *(u16 *)&o->ot_depth;
    *(s32 *)&q->x1 = *(s32 *)&q->x3;
    Graphics_SubmitDrawModePacket((u32 *)q, ot, (u16)k, 1);
}

DisplayObject*Library_CreateFadingCardOutline(DisplayObject*src,int add){DisplayObject*o=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),6);o->field_48.word=src->field_48.word;*(s32*)&o->field_18=*(s32*)&src->field_18;o->field_60=0x100;*(s16*)&o->field_3C.h.field_3C=*(s16*)&o->field_18*2;*(s16*)&o->field_3C.h.field_3E=*(s16*)&o->field_1A*2;o->field_30.word=src->field_30.word;o->field_20.word=src->field_20.word;o->field_44.word=src->field_44.word;o->rgb=0;DisplayObject_SelectOrderingTable1(o);DisplayObject_SetDepthOffset(o,(signed char)(*(u8*)&src->depth_offset+add));o->field_10=1;o->field_4C=(s32)Library_DrawFadingCardOutline;o->attribute=(o->attribute|(GsALON | GsAONE))&~GsROTOFF;return o;}

/* The seven contiguous functions of the Library screen: the card-view state
   handler Library_UpdateCardView, the trivial state handlers for states 0 and 3, the
   per-frame dispatcher Library_UpdateScreen, the package-transfer callback, the
   owned-card pass and the screen entry Library_InitializeScreen.

   The state handlers were recorded at gcc_2_8_1_g8. They compile to
   identical objects at gcc_2_8_1_g8_split, which the dispatcher and entry
   need, so they build there with the rest of the screen. */

/* Library screen state 2, the card view. It opens the view, slides the model
   and text box in, rotates the model light with the view angle and walks the
   close sequence back to the grid. Owned state and stored pointers use their
   canonical members; remaining scalar/object views retain measured accesses.

   What the retail object depends on:
   - the two slide states are separate arms; cross-jumping merges their
     settle and Widget_SlideSine tails;
   - state 0 updates the object's flag words through `bits` and `attr`, each
     assigned twice, so sched1 does not glue their loads to their uses, and
     reads the object's y origin into the reused `id` ahead of the position
     stores;
   - the selected card ID is stored and read straight back as the stats index;
   - gGraphics_bSyncVBlankTarget is volatile in this unit, which keeps the delay slot of the
     close path's DisplayObject_SetResourceVariant call empty, and the pad
     word is volatile because state 5 loads it twice;
   - the typed card-table flag lvalues must retain the retail base-first
     address sum. */
#define B(p, o) (*((u8 *)(p) + (o)))
#define H(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

#ifndef LIBRARY_CARD_VIEW_BOX_FIELD_54
#define LIBRARY_CARD_VIEW_BOX_FIELD_54 0
#endif
#ifndef LIBRARY_CARD_VIEW_X
#define LIBRARY_CARD_VIEW_X 0x94
#endif
#ifndef LIBRARY_CARD_VIEW_BOX_WIDTH
#define LIBRARY_CARD_VIEW_BOX_WIDTH 0xA8
#endif
/* Regional values (US defaults): the card view's y (0xE; European 6), the
   loading icon's y (0xCD; 0xDD), the card text box's height (0xC0; 0xD0),
   the zoomed card's y (4; 0) and its smallest scale byte (0xD8; 0xD7). */
#ifndef LIBRARY_CARD_VIEW_Y
#define LIBRARY_CARD_VIEW_Y 0xE
#endif
#ifndef LIBRARY_LOADING_ICON_Y
#define LIBRARY_LOADING_ICON_Y 0xCD
#endif
#ifndef LIBRARY_CARD_VIEW_BOX_HEIGHT
#define LIBRARY_CARD_VIEW_BOX_HEIGHT 0xC0
#endif
#ifndef LIBRARY_CARD_ZOOM_Y
#define LIBRARY_CARD_ZOOM_Y 4
#endif
#ifndef LIBRARY_CARD_ZOOM_MIN_SCALE
#define LIBRARY_CARD_ZOOM_MIN_SCALE 0xD8
#endif
#ifndef LIBRARY_EFFECT_CHANNEL_STRIDE
#define LIBRARY_EFFECT_CHANNEL_STRIDE sizeof(DuelEffectChannel)
#endif
#ifndef LIBRARY_CARD_VIEW_CANCEL_BUTTON
#define LIBRARY_CARD_VIEW_CANCEL_BUTTON PAD_BUTTON_CIRCLE
#endif
#ifndef LIBRARY_CARD_VIEW_ACTION_MASK
#define LIBRARY_CARD_VIEW_ACTION_MASK 0x20C0
#endif

#define LIBRARY_EFFECT_CHANNEL(index) \
    ((DuelEffectChannel *)((u8 *)gText_aChannels + \
                           (index) * LIBRARY_EFFECT_CHANNEL_STRIDE))

void Library_UpdateCardView(LibraryState *state)
{
    DuelEffectResourceRecord *rec;
    DisplayObject *o;
    ModelSlotLightEntry *light;
    DuelEffectChannel *box;
    s32 flags;
    s32 sel;
    s32 mode;
    s32 id;
    s32 i;
    s32 b;
    s32 a;
    s32 c;
    s32 layout;
    GsRVIEW2 *rv;
    SVECTOR *p;
    ViewState *vs;
    s32 bits;
    s32 attr;

    rec = gCardPreview_Resources;
    flags = state->motion.state;
    if (!(flags & 0x80)) {
        state->motion.state = flags | 0x80;
        state->motion.card_view_phase = 0;
        state->motion.card_view_slide_phase = 0;
        state->motion.model_load_phase = 1;
        Model_ResetSceneView();
        Model_ResetSceneState();
        gViewState_Current.field_00 = 0x334;
        gViewState_Current.angle = 0x400;
        gViewState_Current.field_04 = 0xC4;
        gViewState_Current.field_0C = 0;
        gViewState_Current.view.rz = 0;
        gViewState_Current.view.super = 0;
        gViewState_Current.projection = 0x12C;
        GsSetProjection(0x12C);
        rv = &gViewState_Current.view;
        rv->vrx = 0;
        gViewState_Current.field_06 = 0;
        rv->vry = 0;
        gViewState_Current.field_08 = 0;
        rv->vrz = 0;
        gViewState_Current.field_0A = 0;
        ViewState_ApplyOrbit();
        CardPreview_RequestResourceTransfer(0, (u16)state->motion.selected_card_id);
        Fade_StartOutKeepOverlay();
        gFade_bStep[0] = 6;
        state->loading_icon = 0;
        return;
    }
    TextBox_UpdateChannels();
    mode = state->motion.model_load_phase;
    if ((mode & 0xF) && ((gFile_dwTransferState_abs & 0x02000030) | gFile_dwCancelRequestState_abs) == 0) {
        if (mode == 1) {
            state->motion.model_load_phase = 0;
            id = (u16)state->motion.selected_card_id - 1;
            if (((gDuel_adwCardStats[id] >> 0x1A) & 0x1F) < 0x14) {
                Model_LoadMonsterMerge(0, id, 0, 0, 0, 0, 4);
                state->motion.animation_countdown = 0x12C;
                state->motion.model_load_phase = 2;
            }
        } else {
            Model_AdvanceSlotLoad(0);
            if (Model_GetSlotReadiness(0) == 1) {
                state->motion.model_load_phase = 0;
                o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
                DisplayObject_ConfigureSpriteAtPosition(o, 0x130, LIBRARY_LOADING_ICON_Y, 3, 0, 2, 0xB, 0x20C);
                o->flags |= 0x28;
                DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(o));
                DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(o), 0xA);
                state->loading_icon = DISPLAY_OBJECT_VIEW(o);
            }
        }
    }

    flags = state->motion.card_view_slide_phase;
    switch (flags & 0xF) {
    case 1:
        o = state->card_view;
        if (!(flags & 0x80)) {
            state->motion.card_view_slide_phase = flags | 0x80;
            S(o, 0x30) = 0x148;
            S(o, 0x32) = LIBRARY_CARD_VIEW_Y;
            DisplayObject_SavePosition((void *)o);
            o->field_60 = -0x400;
        }
        H(o, 0x60) += 0x33;
        if (o->field_60 >= 0) {
            S(o, 0x30) = LIBRARY_CARD_VIEW_X;
            S(o, 0x32) = LIBRARY_CARD_VIEW_Y;
            state->motion.card_view_slide_phase = 0;
        } else {
            Widget_SlideSine(
                (void *)o, LIBRARY_CARD_VIEW_X, LIBRARY_CARD_VIEW_Y, o->field_60);
        }
        TextBox_SetPos(&gText_aChannels[0], S(o, 0x30), S(o, 0x32));
        break;
    case 2:
        o = state->card_view;
        if (!(flags & 0x80)) {
            state->motion.card_view_slide_phase = flags | 0x80;
            DisplayObject_SavePosition((void *)o);
            o->field_60 = 0x400;
        }
        H(o, 0x60) -= 0x33;
        if (o->field_60 <= 0) {
            S(o, 0x30) = 0x148;
            S(o, 0x32) = LIBRARY_CARD_VIEW_Y;
            state->motion.card_view_slide_phase = 0;
        } else {
            Widget_SlideSine((void *)o, 0x148, LIBRARY_CARD_VIEW_Y, o->field_60);
        }
        TextBox_SetPos(&gText_aChannels[0], S(o, 0x30), S(o, 0x32));
        break;
    }

    switch (state->motion.card_view_phase & 0x1F) {
    case 0:
        state->cards[(u16)state->motion.selected_card_id].flags &= 0x7F;
        o = (DisplayObject *)CardPreview_CreateResourceObjects(0, -1, -1);
        bits = o->flags;
        bits |= 4;
        o->flags = bits;
        attr = o->attribute;
        attr &= 0xF7FFFFFF;
        o->attribute = attr;
        id = H(o, 0x4A);
        S(o, 0x30) = state->motion.rest_x - gGraphics_sViewportX - H(o, 0x48);
        S(o, 0x32) = state->motion.rest_y - gGraphics_sViewportY - id;
        o->field_44.h.field_44 = o->field_44.h.field_46 = 0;
        DisplayObject_SavePosition((void *)o);
        o->field_60 = 0;
        SD_SEPlayFull(0x32);
        W(rec->secondary_object, 4) |= 0x80000000;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(o, 0x148, LIBRARY_CARD_VIEW_Y, 0, 2, 0, 0x1B, 0x107);
        B(o, 0x5F) = 0x80;
        o->field_60 = -0x400;
        o->flags |= 8;
        DisplayObject_SelectOrderingTable1(DISPLAY_OBJECT_VIEW(o));
        DisplayObject_SetDepthOffset(DISPLAY_OBJECT_VIEW(o), 4);
        state->card_view = DISPLAY_OBJECT_VIEW(o);
        state->motion.card_view_slide_phase = 1;
        gDuel_wSelectedCardID = (u16)state->motion.selected_card_id;
        layout = 3;
        if (((gDuel_adwCardStats[gDuel_wSelectedCardID - 1] >> 0x1A) & 0x1F) >= 0x14) {
            layout = 4;
        }
        box = TextBox_Create(
            0, layout, LIBRARY_CARD_VIEW_X, LIBRARY_CARD_VIEW_Y,
            LIBRARY_CARD_VIEW_BOX_WIDTH, LIBRARY_CARD_VIEW_BOX_HEIGHT);
#ifndef VERSION_EUROPE
        /* The European build does not write color_index_54 here. */
        box->color_index_54 = LIBRARY_CARD_VIEW_BOX_FIELD_54;
#endif
        box->glyph_delay = 1;
        box->depth_offset = 4;
        state->motion.card_view_phase = 1;
        return;
    case 1:
        o = rec->primary_object;
        Library_CreateFadingCardOutline(DISPLAY_OBJECT_VIEW(o), -1);
        if (!(o->attribute & 0x08000000)) {
            o->field_20.b.field_22 += 0xC;
            o->field_20.b.field_21 += 6;
            H(o, 0x46) = H(o, 0x44) += 0xCC;
            H(o, 0x60) += 0x66;
            DisplayObject_InterpolatePositionCosine((void *)o, 2, LIBRARY_CARD_ZOOM_Y, o->field_60);
            if (o->field_60 >= 0x800) {
                o->field_20.word = 0x8000;
                o->field_30.word = (LIBRARY_CARD_ZOOM_Y << 16) | 2;
                o->field_44.word = 0x10001000;
                W(rec->secondary_object, 4) &= 0x7FFFFFFF;
                o->attribute |= 0x08000000;
            }
            if (!(o->attribute & 0x08000000)) {
                break;
            }
        }
        if (state->motion.card_view_slide_phase == 0) {
            state->motion.card_view_phase = 2;
        }
        return;
    case 2:
        if (state->motion.model_load_phase != 1 && !(gFade_bFlags[0] & 0x80)) {
            gMain_apFrameServiceCallbacks[3] = 0;
            i = 0;
            do {
                state->motion.display_objects.all[i]->flags &= 0xFFBF;
                i++;
            } while (i < 9);
            H(LIBRARY_EFFECT_CHANNEL(3)->render_object, 8) &= 0xFFBF;
            state->motion.card_view_phase = 3;
            SD_SEPlayFull(0x31);
    case 3:
            o = rec->primary_object;
            o->field_20.b.field_21 += 8;
            if (!(o->field_20.b.field_21 & 0xFF)) {
                o->flags &= 0xFFFB;
                state->motion.card_view_phase = 5;
                return;
            }
        }
        break;
    case 4:
        light = (ModelSlotLightEntry *)Model_GetFlatLight(0, 2);
        vs = &gViewState_Current;
        a = -vs->field_00;
        b = a * rcos(vs->field_04) / 4096;
        c = a * rsin(vs->field_04) / 4096;
        a = b * rsin(vs->angle + 0x800) / 4096;
        b = b * rcos(vs->angle + 0x800) / 4096;
        light->direction_x = b;
        light->direction_y = c;
        light->direction_z = a;
        flags = state->motion.card_view_phase;
        if (!(flags & 0x80)) {
            state->motion.card_view_phase = flags | 0x80;
            state->motion.card_view_subphase = 0;
            gGraphics_bSyncVBlankTarget = 1;
            gMain_apFrameServiceCallbacks[2] = (void (*)(void))Library_DrawWireframeGlobe;
            flags = state->motion.state;
            state->motion.globe_radius = 0;
            if (!(flags & 0x40)) {
                state->motion.state = flags | 0x40;
                light = (ModelSlotLightEntry *)Model_GetFlatLight(0, 0);
                light[0].direction_y = -0x1000;
                light[0].direction_x = 0;
                light[0].direction_z = 0;
                W(light, 0xC) = 0xFFFFFF;
                light[1].direction_x = 0;
                light[1].direction_y = 0x1000;
                light[1].direction_z = 0;
                W(light, 0x1C) = 0x404040;
                W(light, 0x2C) = 0xC0C0C0;
                Model_SetSlotAmbient(0, 0x400, 0x400, 0x400);
                Model_CopySlotPosition(0, (u16 *)0x80181000);
                p = (SVECTOR *)0x80181000;
                gViewState_Current.view.vrx = p->vx;
                gViewState_Current.view.vry = p->vy;
                gViewState_Current.view.vrz = p->vz;
                gViewState_Current.field_00 = Model_AdjustCameraDistanceForSlot(0, 0x334);
                Model_SetSlotShadowEnabled(0, 1);
                Model_SetScreenYOverride(0);
            }
            Model_ControlSlotAnimation(0, 0, 1);
            Model_SeekSlotAnimation(0, 1, 0);
            state->motion.animation_countdown = 0x12C;
            state->motion.card_view_slide_phase = 2;
            o = rec->primary_object;
            o->flags |= 4;
            DisplayObject_SavePosition((void *)o);
            o->field_60 = 0;
            SD_SEPlayFull(0x34);
            Fade_StartInKeepOverlay();
            return;
        }
        sel = state->motion.card_view_subphase;
        switch (sel) {
        case 0:
            o = rec->primary_object;
            if (o->field_60 < 0x800) {
                o->field_20.b.field_21 -= 4;
                if (o->field_20.b.field_21 < LIBRARY_CARD_ZOOM_MIN_SCALE) {
                    o->field_20.b.field_21 = LIBRARY_CARD_ZOOM_MIN_SCALE;
                }
                H(o, 0x60) += 0xCC;
                DisplayObject_InterpolatePositionCosine((void *)o, -0x16, LIBRARY_CARD_ZOOM_Y, o->field_60);
                if (o->field_60 >= 0x800) {
                    o->field_20.b.field_21 = LIBRARY_CARD_ZOOM_MIN_SCALE;
                    S(o, 0x30) = -0x16;
                    S(o, 0x32) = LIBRARY_CARD_ZOOM_Y;
                }
            }
            state->motion.globe_radius = (u16)state->motion.globe_radius + 0x2A;
            if (state->motion.globe_radius >= 0x400) {
                state->motion.globe_radius = 0x400;
                if (!(gFade_bFlags[0] & 0x80) && state->motion.card_view_slide_phase == 0) {
                    state->motion.card_view_subphase = 1;
                    DisplayObject_SetResourceVariant((void *)state->loading_icon, 1);
                    return;
                }
            }
            break;
        case 1:
            if (gInput_wPad1Pressed & 0x80E0) {
                Fade_StartOutKeepOverlay();
                state->motion.card_view_slide_phase = 1;
                state->motion.card_view_subphase = 2;
                SD_SEPlayFull(0x34);
                o = rec->primary_object;
                DisplayObject_SavePosition((void *)o);
                o->field_60 = 0;
                return;
            }
            break;
        case 2:
            o = rec->primary_object;
            if (o->flags & 4) {
                o->field_20.b.field_21 += 4;
                H(o, 0x60) += 0xCC;
                DisplayObject_InterpolatePositionCosine((void *)o, 2, LIBRARY_CARD_ZOOM_Y, o->field_60);
                if (o->field_60 >= 0x800) {
                    o->field_20.b.field_21 = 0;
                    S(o, 0x30) = 2;
                    S(o, 0x32) = LIBRARY_CARD_ZOOM_Y;
                    o->flags &= 0xFFFB;
                }
                if (o->flags & 4) {
                    break;
                }
            }
            if (!(gFade_bFlags[0] & 0x80) && state->motion.card_view_slide_phase == 0) {
                SD_KeyOffVoiceSlots();
                gMain_apFrameServiceCallbacks[2] = 0;
                {
                    void *p = (void *)state->loading_icon;

                    gGraphics_bSyncVBlankTarget = 0;
                    DisplayObject_SetResourceVariant(p, 2);
                }
                state->motion.card_view_phase = 5;
                return;
            }
            break;
        }
        break;
    case 5:
        if (gInput_wPad1Pressed & LIBRARY_CARD_VIEW_CANCEL_BUTTON) {
            H(rec->primary_object, 8) |= 4;
            state->motion.card_view_phase = 6;
            state->motion.model_load_phase = 0;
            DisplayObject_ReleaseIfPresent((void *)state->loading_icon);
            File_CancelTransfers();
            SD_SEPlayFull(0x31);
            return;
        }
        if (state->motion.model_load_phase == 0 &&
            (gInput_wPad1Pressed & LIBRARY_CARD_VIEW_ACTION_MASK)
            && ((gDuel_adwCardStats[(u16)state->motion.selected_card_id - 1] >> 0x1A) & 0x1F) < 0x14
            && state->motion.card_view_slide_phase == 0) {
            state->motion.card_view_phase = 4;
            return;
        }
        break;
    case 6:
        o = rec->primary_object;
        o->field_20.b.field_21 += 8;
        if ((s8)o->field_20.b.field_21 < 0) {
            W(rec->secondary_object, 4) |= 0x80000000;
            o->attribute &= 0xF7FFFFFF;
            DisplayObject_SavePosition((void *)o);
            i = 0;
            o->field_60 = 0;
            do {
                state->motion.display_objects.all[i]->flags |= 0x40;
                i++;
            } while (i < 9);
            H(LIBRARY_EFFECT_CHANNEL(3)->render_object, 8) |= 0x40;
            gMain_apFrameServiceCallbacks[3] = Library_DrawCardGrid;
            Fade_StartInKeepOverlay();
            state->motion.card_view_slide_phase = 2;
            state->motion.card_view_phase = 7;
            SD_SEPlayFull(0x33);
            return;
        }
        break;
    case 7:
        o = rec->primary_object;
        if (o != 0) {
            Library_CreateFadingCardOutline(DISPLAY_OBJECT_VIEW(o), 1);
            o->field_20.b.field_22 += 0xC;
            o->field_20.b.field_21 += 6;
            H(o, 0x60) += 0x66;
            DisplayObject_InterpolatePositionCosine((void *)o,
                state->motion.x - o->field_48.h.field_48 - gGraphics_sViewportX,
                state->motion.y - o->field_48.h.field_4A - gGraphics_sViewportY,
                o->field_60);
            H(o, 0x46) = H(o, 0x44) -= 0xCC;
            if (o->field_60 >= 0x800) {
                DuelEffect_ReleaseResourceObjects(0);
                state->cards[(u16)state->motion.selected_card_id].flags |= 0x80;
                return;
            }
        } else if (state->motion.card_view_slide_phase == 0) {
            DisplayObject_ReleaseIfPresent((void *)state->card_view);
            state->card_view = 0;
            TextBox_Destroy(&gText_aChannels[0]);
            state->motion.card_view_phase = 8;
            return;
        }
        break;
    case 8:
        state->motion.state = 1;
        break;
    }
}

#undef B
#undef H
#undef S
#undef W
#undef LIBRARY_EFFECT_CHANNEL

void Library_EnterGridState(LibraryState *state)
{
    state->motion.state = 1;
}

void func_8002BAAC(u8 *value)
{
}

void Library_UpdateScreen(void)
{
    LibraryState *state;
    ViewState *model;
    s32 one;
    s32 r;
    s32 a;
    s32 step;
    s32 cur;
    s32 target;
    s32 count;
    s32 mode;
    GsRVIEW2 *dst;
    ViewState *p;

    state = &gLibrary_State;
    one = 1;
    mode = gLibrary_State.motion.state;
    mode &= 0xF;
    switch (mode) {
    case 0:
        Library_EnterGridState(state);
        break;
    case 1:
        Library_UpdateGridCursor(state);
        break;
    case 2:
        Library_UpdateCardView(state);
        model = &gViewState_Current;
        r = Model_GetSlotReadiness(0);
        if (r == one) {
            if (Model_GetSlotAnimationIndex(0) != r) {
                Model_CopySlotPosition(0, (u16 *)0x80181010);
                r = gLibrary_sSampledReferenceY - gLibrary_sCapturedReferenceY;
                a = r >= 0 ? r : -r;
                step = a / 24;
                step += 1;
                cur = model->view.vry;
                if (gLibrary_sSampledReferenceY < cur) {
                    model->view.vry = cur - step;
                    cur = *(volatile PSXLONG *)&model->view.vry;
                }
                if (cur < gLibrary_sSampledReferenceY) {
                    model->view.vry = cur + step;
                }
            } else {
                r = model->view.vry;
                target = gLibrary_sCapturedReferenceY;
                if (r != target) {
                    if (target < r) {
                        r -= 8;
                        if (r < target) {
                            r = target;
                        }
                    } else {
                        r += 8;
                        if (target < r) {
                            r = target;
                        }
                    }
                    model->view.vry = r;
                }
                count = state->motion.animation_countdown - 1;
                state->motion.animation_countdown = count;
                if (count <= 0) {
                    state->motion.animation_countdown = 0x12C;
                    r = 2;
                    if (rand() & 1) {
                        r = 7;
                    }
                    Model_ControlSlotAnimation(0, r, 1);
                }
            }
        }
        p = &gViewState_Current;
        p->angle += 0xC;
        ViewState_ApplyOrbit();
        dst = Model_GetCameraViewBuffer();
        ((LibraryViewQuad *)dst)[0] = *(LibraryViewQuad *)&model->view.vpx;
        ((LibraryViewQuad *)dst)[1] = *(LibraryViewQuad *)&model->view.vry;
        Model_UpdateViewMetrics(0);
        break;
    case 3:
        func_8002BAAC((u8 *)state);
        break;
    }
}

/* The latter three Library functions in address order: the package-transfer
   callback, the pass that marks every owned card in the screen's state, and
   Library_InitializeScreen, which sets the screen up, installs that callback and hands
   control to the dispatcher above.

   The owned-card pass was recorded at gcc_2_8_1_g0_split. It compiles to an
   identical object at gcc_2_8_1_g8_split, which the entry needs, so the
   unit builds there. */

#define gStageRect (gGraphics_aImageTransferRects[0])

#ifdef VERSION_EUROPE
void Library_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    /* The European build loads one more stage, case 2: a 64x16 image picked
       by the language (gText_bLanguageIndex) into (896, 352). The US cases 2-6 follow
       as 3-7, case 1 transfers five sectors rather than four, and the image
       rows sit at x 0x280 (y 0xE0 and 0xE6). */
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = 5 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        gStageRect.x = 0x380;
        gStageRect.y = 0x160;
        gStageRect.w = 0x40;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect,
                   (u32 *)(gFile_TransferBufferBase + gText_bLanguageIndex * FILE_SECTOR_SIZE));
        object->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 3:
        gStageRect.x = 0x280;
        gStageRect.y = 0xE0;
        gStageRect.w = 0x100;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect, (u32 *)gFile_TransferBufferBase);
        object->field_30.h.counter = 0x240;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 5:
        gStageRect.x = 0x280;
        gStageRect.y = 0xE6;
        gStageRect.w = 0x100;
        gStageRect.h = 2;
        LoadImage2(&gStageRect, (u32 *)gFile_TransferBufferBase);
        object->transfer_mode = 3;
        object->field_30.word = 0x26810;
        object->phase_size = 20 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 4:
    case 6:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 7:
        SD_LoadSequenceBankPair(1, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#else
void Library_LoadPackageStage(FileTransferDescriptor *object, s32 mode)
{
    switch (mode) {
    case 0:
        object->field_30.h.counter = 0x300;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 64 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 1:
        object->phase_size = 4 * FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 2:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF0;
        gStageRect.w = 0x100;
        gStageRect.h = 0x10;
        LoadImage2(&gStageRect, (u32 *)gFile_TransferBufferBase);
        object->field_30.h.counter = 0x240;
        object->field_30.h.field_32 = 0x100;
        object->w = 0x40;
        object->h = 0x10;
        gFile_dwTransferState_abs &= 0xFFDDFFFF;
        gFile_dwTransferState_abs |= 0x10000;
        object->transfer_mode = 2;
        object->phase_size = 48 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 4:
        gStageRect.x = 0x100;
        gStageRect.y = 0xF6;
        gStageRect.w = 0x100;
        gStageRect.h = 2;
        LoadImage2(&gStageRect, (u32 *)gFile_TransferBufferBase);
        object->transfer_mode = 3;
        object->field_30.word = 0x26810;
        object->phase_size = 20 * FILE_SECTOR_SIZE;
        object->sector_destination = gFile_TransferBufferBase;
        object->alternate_sector_destination = gFile_TransferBufferBase + FILE_SECTOR_SIZE;
        break;

    case 3:
    case 5:
        object->phase_size = FILE_SECTOR_SIZE;
        gFile_dwTransferState_abs &= 0xFFDCFFFF;
        object->alternate_sector_destination = gFile_TransferBufferBase;
        object->sector_destination = gFile_TransferBufferBase;
        object->transfer_mode = 1;
        break;

    case 6:
        SD_LoadSequenceBankPair(1, (u32 *)gFile_TransferBufferBase);
        break;
    }
}
#endif

void Library_MarkOwnedCards(void)
{
    u8 *p = gLibrary_abCardChest;
    s32 i = 0;
    u16 *q;
    do {
        if (*p != 0)
            Library_UpdateCardUsedFlag(i + (CAMPAIGN_FLAG_LIBRARY_CARD_BASE + CARD_ID_FIRST));
        i++;
        p++;
    } while (i < CARD_COUNT);
    q = gDuel_awPlayerDeck;
    i = 0;
    do {
        if (*q != 0)
            Library_UpdateCardUsedFlag(*q + CAMPAIGN_FLAG_LIBRARY_CARD_BASE);
        i++;
        q++;
    } while (i < DECK_SIZE);
}

/* The Library package's first sector and length; the Japanese and European
   WA.MRG place it elsewhere, so a regional build supplies its own. */
#ifndef LIBRARY_PACKAGE_FIRST_SECTOR
#define LIBRARY_PACKAGE_FIRST_SECTOR 0x1DCD
#endif
#ifndef LIBRARY_PACKAGE_SECTOR_COUNT
#define LIBRARY_PACKAGE_SECTOR_COUNT 0x8A
#endif
/* The resource record's field_2C (0x200 in the US build, 0x280 in the
   European one) and the y of the sprite at x 0x10 (0xD8, 0xE8). */
#ifndef LIBRARY_RESOURCE_FIELD_2C
#define LIBRARY_RESOURCE_FIELD_2C 0x200
#endif
#ifndef LIBRARY_SELECTOR_SPRITE_Y
#define LIBRARY_SELECTOR_SPRITE_Y 0xD8
#endif

void Library_InitializeScreen(void) {
    s16 *q;
    DuelEffectResourceRecord *b;
    LibraryState *r;
    DisplayObject *o;
    u8 *e;
    DuelEffectChannel *m;
    s32 n;
    s32 x;
    s32 y;
    s32 v;
    s32 c;
    s32 d;
    s32 k;
    s32 off;
    /* Retain the retail index-controlled palette loop and register frame. */
    s32 rb;

    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    Model_SetFogDrawFlags(0);
    DuelEffect_ClearResourceObjectPointers(0);
    n = CARD_COUNT - 1;
    q = (s16 *)0x801805A2;
    b = &gCardPreview_Resources[0];
    b->src_x = 0x100;
    b->src_y = 0x100;
    b->field_2C = LIBRARY_RESOURCE_FIELD_2C;
    b->field_2E = 0xF0;
    do {
        *q = n + 1;
        n--;
        q--;
    } while (n >= 0);
    File_RequestAsyncTransfer(0, (u8 *)0, LIBRARY_PACKAGE_FIRST_SECTOR,
                              LIBRARY_PACKAGE_SECTOR_COUNT,
                              (FileTransferCallback)Library_LoadPackageStage, 0, 0);
    File_WaitForTransfers();
    Library_MarkOwnedCards();
    gMain_apFrameServiceCallbacks[3] = Library_DrawCardGrid;
    Library_CreateGridSectionSprites();
    r = &gLibrary_State;
    gLibrary_State.motion.state = 0;
    c = gCardGrid_bCursorColumn;
    d = c;
    if (c >= CARD_GRID_SECTION_SIDE_LENGTH) {
        x = (c % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xAE;
    } else {
        x = (d % CARD_GRID_SECTION_SIDE_LENGTH) * 0xE + 0xE;
    }
    r->motion.rest_x = x;
    r->motion.x = x;
    k = gCardGrid_bCursorRow;
    y = (k / CARD_GRID_SECTION_SIDE_LENGTH) * 0xB2
        + (k % CARD_GRID_SECTION_SIDE_LENGTH) * 0x10 + 0xE;
    r->motion.rest_y = y;
    r->motion.y = y;
    Library_UpdateGridScroll(&r->motion);
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(o, 0x10, LIBRARY_SELECTOR_SPRITE_Y,
                                            0, 2, 1, 0x1B, 0x127);
    ((u8 *)&o->field_5E)[1] = 0x80;
    DisplayObject_SelectOrderingTable1(o);
    n = CARD_ID_FIRST;
    o->flags = o->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    r->selector_sprite = o;
    o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        o, r->motion.x, r->motion.y,
        0, 2, 2, 0x1B, 0x147);
    rb = (s32)r;
    ((u8 *)&o->field_5E)[1] = 0x80;
    r->motion.display_objects.by_role.render = o;
    r->cards[0].flags = 0;
    r->cards[0].clut_x = 0;
    do {
        ((LibraryState *)rb)->cards[n].flags = 0;
        r->cards[0].clut_x = 0;
        off = n * 4 - 4;
        v = (*(s32 *)((u8 *)gDuel_adwCardStats + off) >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
        switch (v) {
        case CARD_TYPE_MAGIC:
        case CARD_TYPE_EQUIP:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_MAGIC_EQUIP;
            break;
        case CARD_TYPE_TRAP:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_TRAP;
            break;
        case CARD_TYPE_RITUAL:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_RITUAL;
            break;
        default:
            ((LibraryState *)rb)->cards[n].clut_x = LIBRARY_CARD_SELECTOR_DEFAULT;
            break;
        }
        n++;
    } while (n < CARD_ID_END);
    gText_StagedValues[0].library_count = 0;
    n = CARD_ID_FIRST;
    do {
        if (Campaign_TestStoryFlag(n + CAMPAIGN_FLAG_LIBRARY_CARD_BASE) != 0) {
            gText_StagedValues[0].library_count += 1;
            r->cards[n].flags = LIBRARY_CARD_FLAG_KNOWN;
            if (Library_CheckCardOwned(n) < 0) {
                r->cards[n].flags |= LIBRARY_CARD_FLAG_UNOWNED;
            }
        }
        n++;
    } while (n < CARD_ID_END);
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    /* The Japanese and European builds make neither this call nor the two
       field stores below. */
    func_8003B6AC(3, 1);
#endif
#ifdef VERSION_EUROPE
    /* The European build creates the box with flags 3. */
    m = TextBox_CreateFlagged(3, 0xF8, 0x58, -0x18, 0x90, 0x10, 3);
#else
    m = TextBox_Create(3, 0xF8, 0x58, -0x18, 0x90, 0x10);
#endif
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    m->glyph_advance_x = 0x10;
    m->line_advance_y = 0x10;
#endif
    TextBox_BuildBlocking(m);
    DisplayObject_SelectOrderingTable3(m->render_object);
    m->render_object->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    Library_RefreshSelectedCardText((const LibraryState *)r);
    SD_BGMPlay(0x72D0);
}
