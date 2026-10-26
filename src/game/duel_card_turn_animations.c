#include "../types.h"
#include "duel_side_state.h"
#include "display_object_layout.h"
#include "duel_card_layout.h"
#include "duel_card.h"
#include "card_presentation.h"
#include "duel_card_turn_animations.h"
#include "duel_card_display_state.h"
#include "duel_card_display_objects.h"
#include "display_object_lifecycle.h"
#include "view_state.h"
#include "duel_view_tween.h"
#include "../unmatched.h"
#include "view_state_current.h"
#include "duel_selection_layout.h"
#include "duel_selection_update_linked_object.h"
#include "display_object_core.h"

/* The mirror of Duel_UpdateCardToFieldView: the card turn-back animation step. Picks
 * the mode (+0x2E) from the card record's flags when DisplayObject_MarkInitialized says
 * the object is idle, then runs one of three mode arms -- a countdown that
 * sets the record's 0x400 flag (mode 0), and two rotate-then-settle arms
 * (modes 1 and 2) stepping the angle at +0x2A by +0x28 until the +0x60
 * counter runs out -- and resets through Duel_UpdateCardDisplayState. The arms are laid
 * out as goto targets in retail's own address order and the shared reset
 * block is reached by goto from all three. */
void Duel_UpdateCardToSelectionView(DuelCardTurnObject *p) {
    DuelCardRecord *e;
    s32 m;
    s32 v;
    s16 c;
    s32 f;

    e = &gDuel_aCardRecords[p->record_index];
    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)p) == 0) {
        m = e->flags;
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        p->mode = v;
    }
    v = p->mode;
    if (v == 1) {
        goto m1;
    }
    if (v >= 2) {
        goto ge2;
    }
    if (v == 0) {
        goto m0;
    }
    return;
ge2:
    if (v == 2) {
        goto m2;
    }
    return;
m0:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->counter = 8;
        e->flags = e->flags | 0x400;
        p->tint.packed = 0x8000;
        p->flags =
            p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
        if ((e->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
            p->tint.channel[2] = 0xC0;
        }
    }
    c = p->counter - 1;
    p->counter = c;
    if (c == 0) {
        return;
    }
reset:
    p->phase = 0;
    p->field_24 = 0;
    Duel_UpdateCardDisplayState((DuelCardDisplayObject *)p);
    return;
m1:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->tint.packed = 0;
        p->counter = 4;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt1;
    }
    if (gDuel_bActiveSide != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    e->flags = e->flags | 0x400;
    p->tint.packed = 0x4000;
    p->angle.signed_value = 0x4080;
    p->counter = 4;
    p->flags = p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
    return;
alt1:
    p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    p->tint.channel[1] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[1] = 0;
    goto reset;
m2:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->tint.packed = 0xC000;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->counter = 4;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt2;
    }
    if (gDuel_bActiveSide != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    e->flags = e->flags | 0x400;
    p->angle.signed_value = -0x3F80;
    p->tint.packed = 0xC000C0;
    p->counter = 4;
    p->flags = p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
    return;
alt2:
    p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    p->tint.channel[0] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[0] = 0;
    goto reset;
}

/* Card flip/turn animation step on a duel card object: picks the animation
 * mode (+0x2E) from the card's record flags when DisplayObject_MarkInitialized says the
 * object is idle, then runs one of three mode arms -- a plain countdown
 * (mode 0), and two rotate-then-settle arms (modes 1 and 2) that step the
 * angle at +0x2A by +0x28 until the +0x60 counter runs out, mirror it
 * according to gDuel_bActiveSide, and reset through Duel_ApplyCardObjectFlags.
 * The arms are laid out as goto targets in retail's own address order and
 * the shared reset block is reached by goto from both. */
void Duel_UpdateCardToFieldView(DuelCardTurnObject *p) {
    DuelCardRecord *e;
    s32 m;
    s32 v;
    u32 s1;
    u32 s2;
    s32 d;
    s16 c;
    s32 f;

    e = &gDuel_aCardRecords[p->record_index];
    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)p) == 0) {
        m = e->flags;
        v = 0;
        if ((m & DUEL_CARD_FLAG_FACE_DOWN) == 0) {
            v = 1;
            if ((m & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                v = 2;
            }
        }
        p->mode = v;
    }
    v = p->mode;
    if (v == 1) {
        goto m1;
    }
    if (v >= 2) {
        goto ge2;
    }
    if (v == 0) {
        goto m0;
    }
    return;
ge2:
    if (v == 2) {
        goto m2;
    }
    return;
m0:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->counter = 8;
    }
    c = p->counter - 1;
    p->counter = c;
    if (c > 0) {
        return;
    }
    e->flags = e->flags & 0xFBFF;
    p->tint.channel[2] = 0x80;
    p->tint.channel[0] = 0;
    p->tint.channel[1] = 0;
    p->tint.packed = 0x800000;
    if ((e->flags & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
        p->tint.channel[1] = 0xC0;
    }
reset:
    p->phase = 0;
    p->field_24 = 0;
    Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)p);
    return;
m1:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->tint.packed = 0;
        p->counter = 4;
        p->flags =
            p->flags | DISPLAY_OBJECT_FLAG_CLIP_TEST;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt1;
    }
    s1 = p->angle.unsigned_value + p->angle_step;
    p->angle.unsigned_value = s1;
    p->tint.channel[1] = s1 >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    d = gDuel_bActiveSide;
    e->flags = e->flags & 0xFBFF;
    if (d != 0) {
        p->tint.packed = 0xC00000;
        p->angle.signed_value = -0x3F80;
    } else {
        p->tint.packed = 0x400000;
        p->angle.signed_value = 0x4080;
    }
    p->counter = 4;
    return;
alt1:
    if (gDuel_bActiveSide != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[2] = 0;
    goto reset;
m2:
    f = p->phase;
    if ((f & 0x40) == 0) {
        p->phase = f | 0x40;
        p->tint.packed = 0xC00000;
        p->angle_step = 0x1000;
        p->angle.unsigned_value = 0x80;
        p->counter = 4;
    }
    if ((p->phase & 0x20) != 0) {
        goto alt2;
    }
    s2 = p->angle.unsigned_value - p->angle_step;
    p->angle.unsigned_value = s2;
    p->tint.channel[0] = s2 >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->phase = p->phase | 0x20;
    d = gDuel_bActiveSide;
    e->flags = e->flags & 0xFBFF;
    if (d != 0) {
        p->tint.packed = 0xC0C000;
        p->angle.signed_value = -0x3F80;
    } else {
        p->tint.packed = 0x40C000;
        p->angle.signed_value = 0x4080;
    }
    p->counter = 4;
    return;
alt2:
    if (gDuel_bActiveSide != 0) {
        p->angle.unsigned_value = p->angle.unsigned_value + p->angle_step;
    } else {
        p->angle.unsigned_value = p->angle.unsigned_value - p->angle_step;
    }
    p->tint.channel[2] = p->angle.unsigned_value >> 8;
    c = p->counter - 1;
    p->counter = c;
    if (c != 0) {
        return;
    }
    p->tint.channel[0] = 0;
    goto reset;
}

void Duel_StartViewTween(
    s32 frames, s32 distance, s32 orbit_angle, s32 heading, s32 target_z)
{
    s16 current_distance = gViewState_Current.field_00;
    s32 distance_step = ((distance - current_distance) << 16) / frames;
    s16 current_orbit_angle = gViewState_Current.field_04;
    s32 orbit_step = ((orbit_angle - current_orbit_angle) << 16) / frames;
    s16 current_heading = gViewState_Current.angle;
    s32 heading_step = ((heading - current_heading) << 16) / frames;
    s32 current_target_z = gViewState_Current.view.vrz;
    s32 target_z_step = ((target_z - current_target_z) << 16) / frames;

    gDuel_wViewTweenFramesRemaining = frames;
    gDuel_wViewTweenDistanceTarget = distance;
    gDuel_wViewTweenOrbitAngleTarget = orbit_angle;
    gDuel_wViewTweenHeadingTarget = heading;
    gDuel_wViewTweenReferenceZTarget = target_z;
    gDuel_dwViewTweenDistanceAccumulator = (current_distance << 16) | 0x8000;
    gDuel_dwViewTweenOrbitAngleAccumulator = (current_orbit_angle << 16) | 0x8000;
    gDuel_dwViewTweenHeadingAccumulator = (current_heading << 16) | 0x8000;
    gDuel_dwViewTweenReferenceZAccumulator = (current_target_z << 16) | 0x8000;
    gDuel_dwViewTweenDistanceStep = distance_step;
    gDuel_dwViewTweenOrbitAngleStep = orbit_step;
    gDuel_dwViewTweenHeadingStep = heading_step;
    gDuel_dwViewTweenReferenceZStep = target_z_step;
}

void DuelSelection_UpdateLinkedObject(DisplayObject *object)
{
    u8 *base = gDuel_abSelectionSides;
    s32 offset =
        (s16)object->field_2C.h.field_2C * DUEL_SELECTION_RECORD_SIZE;
    DisplayObject *parent;

    offset += gDuel_bActiveSide * DUEL_SELECTION_SIDE_SIZE;
    parent = ((DuelSelectionRecord *)(base + offset))->field_00;
    if (parent == 0) {
        DisplayObject_ReleaseIfPresent(object);
    } else {
        object->field_30.h.field_30 =
            parent->field_30.h.field_30 + object->position.h.field_28;
        object->field_30.h.field_32 =
            parent->field_30.h.field_32 + object->position.h.field_2A;
        if (parent->field_6C == 0) {
            object->field_6C = 0;
            object->update = 0;
        }
    }
}
