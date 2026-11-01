#include "../types.h"
#include "duel_scene_card_placement.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_record_lifecycle.h"
#include "duel_side_state.h"
#include "duel_scene_state.h"
#include "display_object_layout.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "duel_card_capture_rounded_texture.h"
#include "duel_create_card_effect_overlay.h"
#include "duel_field_effects.h"
#include "card_preview_objects.h"
#include "duel_effect_resource_setup.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
#include "duel_scene_card_use.h"
#include "display_object_work_slots.h"

/* Regional value: the object's y (0x16 -> 0xE). The European build
 * (src/game/european/) defines its own. */
#ifndef DUEL_CARD_USE_OBJECT_Y
#define DUEL_CARD_USE_OBJECT_Y 0x16
#endif

/* The COMMON definition preserves the GP-relative store and its load delay.
   The linker resolves it to the existing global without allocating storage. */
u16 gDuel_wPlacementCardId;

void DuelScene_UpdateCardUse(void)
{
    DisplayObject *p;
    DuelCardRecord *slot;
    DisplayObject *q0;
    DisplayObject *q1;
    u8 state;
    u16 flags;
    u16 f2;
    u16 f3;
    u8 st;
    s32 v1;
    s32 v2;
    s32 v3;
    s32 value;
    s32 fld;

    p = gDisplayObject_apWorkSlots[0];
    flags = gDuel_wSceneStateFlags;
    if ((flags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        gDuel_wSceneStateFlags = flags | 0xC000;
        slot = &gDuel_aCardRecords[p->field_6A];
        value = slot->card_id;
        gDuel_wPlacementCardId = *(u16 *)&slot->card_id;
        CardPreview_RequestResourceTransfer(0, value);
        if (p->field_68 == CARD_TYPE_MAGIC) {
            gDuel_pActiveSideState->rank.pure_magic_used =
                gDuel_pActiveSideState->rank.pure_magic_used + 1;
        }
        DuelCard_DeactivateRecord(slot);
        gDuel_bSceneSubStep = 1;
    }
    state = gDuel_bSceneSubStep;
    switch (state & 0xF) {
    case 1:
        if ((state & 0x80) == 0) {
            if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
                 gFile_dwCancelRequestState_abs) != 0) {
                return;
            }
            f2 = p->flags;
            gDuel_bSceneSubStep = state | 0x80;
            p->flags = f2 | DISPLAY_OBJECT_FLAG_CLIP_TEST;
            p = (DisplayObject *)CardPreview_CreateResourceObjects(0, -1, -1);
            p->field_30.h.field_30 = 0x5A;
            p->field_30.h.field_32 = DUEL_CARD_USE_OBJECT_Y;
            p->field_20.b.rotation_y = 0xC0;
            DisplayObject_SetDepthOffset(p, -0xA);
            p->flags =
                (p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST) &
                ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            gDisplayObject_apWorkSlots[1] = p;
            return;
        }
        if ((state & 0x40) == 0) {
            v1 = p->field_20.b.rotation_y + 6;
            p->field_20.b.rotation_y = v1;
            if ((u8)v1 < 0x40) {
                return;
            }
            gDuel_bSceneSubStep = gDuel_bSceneSubStep | 0x40;
            DisplayObject_ReleaseIfPresent(p);
            q1 = gDisplayObject_apWorkSlots[1];
            gDisplayObject_apWorkSlots[0] = 0;
            q1->flags = q1->flags | DISPLAY_OBJECT_FLAG_RENDERABLE;
            return;
        }
        p = gDisplayObject_apWorkSlots[1];
        if ((state & 0x20) == 0) {
            v2 = p->field_20.b.rotation_y + 6;
            p->field_20.b.rotation_y = v2;
            if ((s8)v2 < 0) {
                return;
            }
            p->field_60 = 0x1E;
            p->field_20.b.rotation_y = 0;
            f3 = p->flags;
            st = gDuel_bSceneSubStep | 0x20;
            p->flags = f3 & ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
            gDuel_bSceneSubStep = st;
            return;
        }
        v3 = *(u16 *)&p->field_60 - 1;
        p->field_60 = v3;
        if ((s16)v3 > 0) {
            return;
        }
        gDuel_bSceneSubStep = 2;
        return;
    case 2:
        DuelEffect_StartCardEffect((s16)gDuel_wPlacementCardId, 0);
        gDuel_bSceneSubStep = 3;
        return;
    case 3:
        if ((state & 0x80) == 0) {
            p = gDisplayObject_apWorkSlots[1];
            gDuel_bSceneSubStep = state | 0x80;
            DuelCard_CaptureRoundedTexture(p);
            gDisplayObject_apWorkSlots[0] =
                Duel_CreateCardEffectOverlay((DisplayObjectConfigView *)p);
            gDisplayObject_apWorkSlots[0]->attribute = gDisplayObject_apWorkSlots[0]->attribute | (GsALON | GsAONE);
            gDisplayObject_apWorkSlots[0]->attribute = gDisplayObject_apWorkSlots[0]->attribute & ~GsROTOFF;
            gDisplayObject_apWorkSlots[1] =
                Duel_CreateCardEffectOverlay((DisplayObjectConfigView *)p);
            DisplayObject_SetDepthOffset(gDisplayObject_apWorkSlots[1], -1);
            gDisplayObject_apWorkSlots[1]->attribute = gDisplayObject_apWorkSlots[1]->attribute | (GsALON | GsATWO);
            gDisplayObject_apWorkSlots[1]->attribute = gDisplayObject_apWorkSlots[1]->attribute & ~GsROTOFF;
            DuelEffect_ReleaseResourceObjects(0);
            return;
        }
        q0 = gDisplayObject_apWorkSlots[0];
        fld = q0->field_44.h.field_44;
        value = fld + 0x80;
        q1 = gDisplayObject_apWorkSlots[1];
        *(u16 *)&q1->field_44.h.field_46 = value;
        *(u16 *)&q1->field_44.h.field_44 = value;
        *(u16 *)&q0->field_44.h.field_46 = value;
        *(u16 *)&q0->field_44.h.field_44 = value;
        value = *(u8 *)&gDisplayObject_apWorkSlots[0]->rgb;
        value -= 4;
        if (value < 0) {
            value = 0;
        }
        value = value | ((value << 16) | (value << 8));
        gDisplayObject_apWorkSlots[0]->rgb = value;
        gDisplayObject_apWorkSlots[1]->rgb = value;
        if (value != 0) {
            return;
        }
        DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots[0]);
        DisplayObject_ReleaseIfPresent(gDisplayObject_apWorkSlots[1]);
        gDuel_bSceneSubStep = 4;
        return;
    case 4:
        if ((state & 0x80) == 0) {
            gDuel_bSceneSubStep = state | 0x80;
            DuelEffect_StartCardEffect((s16)gDuel_wPlacementCardId, 1);
            return;
        }
        gDuel_wSceneStateFlags = 5;
        return;
    }
}
