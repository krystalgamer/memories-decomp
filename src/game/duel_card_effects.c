#define gDuel_bEffectRequestStatus_IN_DATA
#define DUEL_FIELD_GRID_ALIASES
#include "../types.h"
#include "file_transfer.h"
#include "duel_trap_resolution.h"
#include "duel_select_trap_by_card_id.h"
#include "duel_card_object_helpers.h"
#include "../unmatched.h"
#include "duel_side_state.h"
#include "card_constants.h"
#include "duel_action_lock.h"
#include "duel_effect_request.h"
#include "duel_monster_removal_rules.h"
#include "sound.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "duel_card.h"
#include "card_presentation.h"
#include "display_object_start_alpha_transition.h"
#include "duel_card_record_lifecycle.h"
#include "duel_card_display_objects.h"
#include "duel_effect.h"
#include "view_state.h"
#include "duel_view_tween.h"
#include "../unmatched.h"
#include "duel_field_card_render_data.h"
#include "duel_card_effects.h"
#include "display_object_helpers.h"
#define gDuel_sEffectWorkCounter D_8009B20C[1]
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))
#define DUEL_FIELD_EFFECT_OBJECT_VIEW(object) \
    ((DuelFieldEffectObject *)(object))

/* Small data at 0x8009AF30, owned here: the recovery handler scales the
   first table by 100 and the direct-damage handler scales the second by 10,
   one entry per card in each family. */
u8 gDuel_abLifePointRecoveryUnits[DUEL_LIFE_POINT_EFFECT_COUNT] = {
    DUEL_MOOYAN_CURRY_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_RED_MEDICINE_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_GOBLINS_SECRET_REMEDY_RECOVERY /
        DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_SOUL_OF_THE_PURE_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
    DUEL_DIAN_KETO_RECOVERY / DUEL_LIFE_POINT_RECOVERY_SCALE,
};
u8 gDuel_abDirectDamageUnits[DUEL_LIFE_POINT_EFFECT_COUNT] = {
    DUEL_SPARKS_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_HINOTAMA_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_FINAL_FLAME_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_OOKAZI_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
    DUEL_TREMENDOUS_FIRE_DAMAGE / DUEL_DIRECT_DAMAGE_SCALE,
};

/* Runs the table-driven LP change phases. Recovery values are scaled by 100,
   added to the selected side's life points, and capped at its maximum; the
   alternate path subtracts the same values and floors the result at zero. */
void DuelEffect_ApplyLifePointRecovery(void) {
    s32 s0;
    s32 s1;
    s32 flag;
    u16 v1;
    DuelEffectRequest *obj;

    s0 = gDuel_wEffectCardID;
    s1 = s0 - DUEL_LIFE_POINT_RECOVERY_FIRST_CARD_ID;
    if (DuelEffect_MarkInitialized() == 0) {
        if (Duel_SelectTrapByCardId(DUEL_BAD_REACTION_TO_SIMOCHI_CARD_ID) != 0) {
            s1 = s0 - (DUEL_LIFE_POINT_RECOVERY_FIRST_CARD_ID -
                       DUEL_LIFE_POINT_EFFECT_COUNT);
        }
        obj = DuelEffect_CreateRequest(5);
        obj->field_00 = 0xA0;
        obj->field_02 = 0x78;
        obj->phase = s1;
        SD_SEPlayFull(0x14);
        return;
    }
    flag = gDuel_wCardEffectFlags;
    if (!(flag & 0x40)) {
        gDuel_wCardEffectFlags = flag | 0x60;
        if (gDuel_wSelectedTrapCardId == 0) {
            u8 *p = &gDuel_abLifePointRecoveryUnits[s1];
            v1 = gDuel_pActiveSideState->life_points.unsigned_value +
                 (*p) * DUEL_LIFE_POINT_RECOVERY_SCALE;
            gDuel_pActiveSideState->life_points.unsigned_value = v1;
            if (gDuel_pActiveSideState->max_life_points < (s16) v1) {
                gDuel_pActiveSideState->life_points.unsigned_value =
                    (u16) gDuel_pActiveSideState->max_life_points;
            }
            goto block_14;
        }
        gDuel_wTrapPresentationMode = 0;
        goto block_9;
    }
block_9:
    if (gDuel_wCardEffectFlags & 0x20) {
        if (Duel_UpdateTrapPresentation() == 0) {
            gDuel_wCardEffectFlags &= 0xFFDF;
            obj = DuelEffect_CreateRequest(9);
            obj->field_00 = 0xA0;
            obj->field_02 = 0x78;
            obj->phase = s1;
        }
    } else {
        u8 *p = &gDuel_abLifePointRecoveryUnits[s1];
        v1 = gDuel_pActiveSideState->life_points.unsigned_value -
             (*p) * DUEL_LIFE_POINT_RECOVERY_SCALE;
        gDuel_pActiveSideState->life_points.unsigned_value = v1;
        if ((s16) v1 < 0) {
            gDuel_pActiveSideState->life_points.unsigned_value = 0;
        }
block_14:
        gDuel_wCardEffectFlags = 0;
    }
}

/* Applies one of the five direct-damage cards. The selected table value is
   scaled by 10 and taken off the target side's life points, clamped at zero. */
void DuelEffect_ApplyDirectDamage(void) {
    s32 unit;
    s32 flags;
    u16 remaining;
    DuelEffectRequest *obj;
    DuelSideState *p;

    unit = gDuel_wEffectCardID - DUEL_DIRECT_DAMAGE_FIRST_CARD_ID;
    if (DuelEffect_MarkInitialized() == 0) {
        if (Duel_SelectTrapByCardId(DUEL_GOBLIN_FAN_CARD_ID) != 0) {
            unit = DUEL_LIFE_POINT_EFFECT_COUNT;
        }
        obj = DuelEffect_CreateRequest(6);
        obj->field_00 = 0xA0;
        obj->field_02 = 0x78;
        obj->phase = unit;
        SD_SEPlayFull(0x1C);
        return;
    }
    flags = gDuel_wCardEffectFlags;
    if (!(flags & 0x40)) {
        gDuel_wCardEffectFlags = flags | 0x60;
        if (gDuel_wSelectedTrapCardId == 0) {
            p = &gDuel_aSideStates[gDuel_bActiveSide ^ 1];
            goto apply;
        }
        gDuel_wTrapPresentationMode = 0;
    }
    if (gDuel_wCardEffectFlags & 0x20) {
        if (Duel_UpdateTrapPresentation() == 0) {
            gDuel_wCardEffectFlags &= 0xFFDF;
            obj = DuelEffect_CreateRequest(7);
            obj->field_00 = 0xA0;
            obj->field_02 = 0x78;
            obj->phase = unit;
            SD_SEPlayFull(0x1C);
        }
    } else {
        p = &gDuel_aSideStates[gDuel_bActiveSide];
apply:
        remaining = p->life_points.unsigned_value -
                    gDuel_abDirectDamageUnits[unit] *
                        DUEL_DIRECT_DAMAGE_SCALE;
        p->life_points.unsigned_value = remaining;
        if ((s16) remaining < 0) {
            p->life_points.unsigned_value = 0;
        }
        gDuel_wCardEffectFlags = 0;
    }
}

void DuelEffect_ApplyMonsterRemoval(void) {
    DuelEffectRequest *p;
    DuelCardRecord *e;
    u8 *tb;
    u8 *cb;
    u8 *rb;
    s16 *sp;
    s32 i;
    s32 n;
    s16 m;
    s32 x;
    s32 ix;

    if (DuelEffect_MarkInitialized() == 0) {
        p = DuelEffect_CreateRequest(0xF);
        p->field_00 = 0;
        p->field_02 = 0;
        p->field_04 = 0;
        SD_SEPlayFull(0x22);
        i = 0;
        cb = gDuel_abMonsterRemovalRules;
        x = gDuel_wEffectCardID;
        while (1) {
            if (*(u8 *)(i + (s32)cb) +
                    DUEL_MONSTER_REMOVAL_CARD_ID_BASE == x) {
                break;
            }
            i += DUEL_MONSTER_REMOVAL_RULE_SIZE;
        }
        p->phase = i / DUEL_MONSTER_REMOVAL_RULE_SIZE;
        m = gDuel_abMonsterRemovalRules[
            i + DUEL_MONSTER_REMOVAL_SELECTOR_OFFSET
        ];
        n = m;
        gDuel_wMonsterRemovalCriterion = m;
        if (n >= 0x15) {
            gDuel_wMonsterRemovalCriterion = n * CARD_STAT_SCALE;
            gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 1;
        }
        gDuel_wMonsterRemovalZone = 5;
        return;
done:
        gDuel_wCardEffectFlags = 0;
        return;
    }

    tb = gDuel_abFieldGridCardRecordIndices;
    sp = &gDuel_wMonsterRemovalZone;
    rb = (u8 *)gDuel_aCardRecords;
    goto head;

arm:
    if ((Duel_CalcCardStats(e) & 0xFFFF) >=
        gDuel_wMonsterRemovalCriterion) {
        goto hit;
    }
next:
    gDuel_wMonsterRemovalZone = gDuel_wMonsterRemovalZone + 1;
    if (gDuel_wMonsterRemovalZone >= DUEL_FIELD_SIDE_ZONE_COUNT) {
        goto done;
    }
head:
    ix = gDuel_wMonsterRemovalZone + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    e = (DuelCardRecord *)(*(u8 *)(ix + (s32)tb) * DUEL_CARD_RECORD_SIZE +
        (s32)rb);
    if ((e->flags & DUEL_CARD_FLAG_OCCUPIED) == 0) {
        goto next;
    }
    if ((gDuel_wCardEffectFlags & 1) != 0) {
        goto arm;
    }
    if (DISPLAY_OBJECT_VIEW(e->object)->field_68 != sp[-1]) {
        goto next;
    }

hit:
    e = &gDuel_aCardRecords[gDuel_abFieldGridCardRecordIndices[
        gDuel_wMonsterRemovalZone + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
    ]];
    p = DuelEffect_CreateRequest(0xB);
    p->field_00 = DISPLAY_OBJECT_VIEW(e->object)->field_30.h.field_30;
    p->field_02 = DISPLAY_OBJECT_VIEW(e->object)->field_30.h.field_32;
    p->field_04 = *(u16 *)&DISPLAY_OBJECT_VIEW(e->object)->field_34;
    p->phase = Duel_GetCardEffectVariant((CardObject *)e->object);
    DuelCard_RemoveFromField(e);
    SD_SEPlayFull(0x1F);
}

void DuelEffect_ApplyStopDefense(void) {
    DuelCardRecord *r;
    DuelEffectRequest *p;
    DuelFieldPosition *e;
    u8 *t;
    s32 f;
    s32 c;
    s32 v;
    s32 n;
    s32 w;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = -1;
    }

    f = gDuel_wCardEffectFlags;

    if ((f & 0x40) != 0) {
        if ((f & 0x20) == 0) {
            if (gDuelEffect_pTrackedRequest->progress != 0) {
                n = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                    DUEL_FIELD_ROW_SIZE;
                c = gDuel_abFieldGridCardRecordIndices[gDuel_sEffectWorkCounter + n];
                r = &gDuel_aCardRecords[c];
                if ((*(s32 *)&r->terrain_modifier & 0x88000000) ==
                    0x88000000) {
                    DisplayObject_StartAlphaTransition(DISPLAY_OBJECT_VIEW(r->object), 0xC0, 0, 6);
                    r->flags &= ~DUEL_CARD_FLAG_DEFENSE_POSITION;
                }
                gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x20;
            }
        }

        if ((gDuelEffect_pTrackedRequest->flags &
             DUEL_EFFECT_REQUEST_FLAG_ACTIVE) != 0) {
            return;
        }
        gDuel_wCardEffectFlags = gDuel_wCardEffectFlags & 0xFF9F;
        return;
    }

    v = *(u16 *)&gDuel_sEffectWorkCounter + 1;
    gDuel_sEffectWorkCounter = v;
    if ((s16)v >= DUEL_FIELD_ROW_SIZE) {
        gDuel_wCardEffectFlags = 0;
        return;
    }

    p = DuelEffect_AllocateRequest(0xC);
    t = (u8 *)gDuel_aFieldGridGroundPositions;
    e = (DuelFieldPosition *)((
        (gDuel_sEffectWorkCounter + DUEL_FIELD_ROW_SIZE) *
            sizeof(DuelFieldPosition) +
        gDuel_bActiveSide * DUEL_FIELD_SIDE_POSITION_BYTES
    ) + t);
    w = (u16)e->x;
    gDuelEffect_pTrackedRequest = p;
    p->field_02 = 0;
    p->field_00 = w;
    p->field_04 = (u16)e->y;
    SD_SEPlayFull(0x20);

    gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x40;
}

void DuelEffect_ApplyBoardDestruction(void) {
    DuelEffectRequest *e;
    DuelCardRecord *p;
    DuelCardRecord *q;
    s32 i;
    s32 f;
    s32 g;

    if (DuelEffect_MarkInitialized() == 0) {
        SD_CdSoundPlay(0x8020);
        return;
    }

    f = gDuel_wCardEffectFlags;

    if ((f & 0x20) == 0) {
        if ((gFile_wTransferStepFlags_abs & 0x4000) == 0) {
            return;
        }
        gDuel_wCardEffectFlags = f | 0x20;
        e = DuelEffect_CreateRequest(0x11);
        if (gDuel_wEffectCardID == DUEL_DRAGON_CAPTURE_JAR_CARD_ID) {
            g = gDuel_wCardEffectFlags;
            e->phase = 1;
            gDuel_wCardEffectFlags = g | 0x40;
        }
        e->field_00 = 0;
        e->field_02 = 0;
        e->field_04 = 0;
        return;
    }

    if ((gFile_wTransferStepFlags_abs & 0x4000) != 0) {
        return;
    }

    if ((f & 0x40) != 0) {
        i = DUEL_FIELD_ROW_SIZE;
        while (1) {
            p = (DuelCardRecord *)(gDuel_abFieldGridCardRecordIndices[
                i + gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT
            ] * DUEL_CARD_RECORD_SIZE + (u8 *)gDuel_aCardRecords);
            if ((p->flags & DUEL_CARD_FLAG_OCCUPIED) != 0 &&
                DISPLAY_OBJECT_VIEW(p->object)->field_68 == 0) {
                DuelCard_RemoveFromField(p);
            }
            i++;
            if (i >= DUEL_FIELD_SIDE_ZONE_COUNT) {
                break;
            }
        }
    } else {
        p = gDuel_aFieldCardRecords;
        q = p + DUEL_CARD_SIDE_RECORD_COUNT;
        i = 0;
        do {
            if ((p->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                DuelCard_RemoveFromField(p);
            }
            if ((q->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
                DuelCard_RemoveFromField(q);
            }
            i++;
            p++;
            q++;
        } while (i < DUEL_FIELD_SIDE_ZONE_COUNT);
    }

    gDuel_wCardEffectFlags = 0;
}

void DuelEffect_ApplyRaigeki(void) {
    DuelEffectRequest *p;
    DuelFieldPosition *e;
    DuelCardRecord *r;
    DuelEffectRequest *q;
    s32 a;
    u8 *t;
    s32 v;
    s32 w;
    s32 n;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = 0;
        q = DuelEffect_AllocateRequest(0x10);
        t = (u8 *)gDuel_aFieldGridGroundPositions;
        e = (DuelFieldPosition *)((
            (gDuel_sEffectWorkCounter + DUEL_FIELD_ROW_SIZE) *
                sizeof(DuelFieldPosition) +
            gDuel_bActiveSide * DUEL_FIELD_SIDE_POSITION_BYTES
        ) + t);
        w = (u16)e->x;
        p = q;
        gDuelEffect_pTrackedRequest = p;
        p->field_02 = 0;
        p->field_00 = w;
        a = 0x15;
        p->field_04 = (u16)e->y;
        goto call;
    }

    if ((gDuel_bEffectRequestStatus & 1) == 0) {
        gDuel_wCardEffectFlags = 0;
        return;
    }

    if (gDuelEffect_pTrackedRequest->progress ==
        gDuel_sEffectWorkCounter + 1) {
        SD_SEPlayFull(0x15);
        n = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
            DUEL_FIELD_ROW_SIZE;
        r = (DuelCardRecord *)((u8 *)gDuel_aCardRecords +
            gDuel_abFieldGridCardRecordIndices[gDuel_sEffectWorkCounter + n] * DUEL_CARD_RECORD_SIZE);
        v = r->flags & 0x8000;
        gDuel_sEffectWorkCounter = *(u16 *)&gDuel_sEffectWorkCounter + 1;
        if (v != 0) {
            q = DuelEffect_AllocateRequest(0xB);
            q->buffer = (u8 *)q->buffer + gDuel_sEffectWorkCounter * 0x3000;
            q->field_00 = DISPLAY_OBJECT_VIEW(r->object)->field_30.h.field_30;
            q->field_02 = DISPLAY_OBJECT_VIEW(r->object)->field_30.h.field_32;
            q->field_04 = *(u16 *)&DISPLAY_OBJECT_VIEW(r->object)->field_34;
            q->phase = Duel_GetCardEffectVariant((CardObject *)r->object);
            DuelCard_RemoveFromField(r);
            a = 0x1F;
call:
            SD_SEPlayFull(a);
        }
    }
}

#define DUEL_FIELD_EFFECT_TIMER_STEP 8
#define DUEL_FIELD_EFFECT_MARK_THRESHOLD 40
#define DUEL_FIELD_EFFECT_TIMER_LIMIT 64

/* The reveal path indexes by side and slot; the later status step walks the
   same storage from a flat base. */

void DuelEffect_UpdateRevealCard(DuelFieldEffectObject *o)
{
    o->timer += DUEL_FIELD_EFFECT_TIMER_STEP;
    if (!(o->active & 0x80) &&
        o->timer >= DUEL_FIELD_EFFECT_MARK_THRESHOLD) {
        o->active |= 0x80;
        gDuel_aCardRecords[o->index].flags &= ~DUEL_CARD_FLAG_DISPLAY_MARKER;
        o->mark = 0;
    }
    if (o->timer < DUEL_FIELD_EFFECT_TIMER_LIMIT) {
        o->timer = 0;
        o->active = 0;
        o->callback = 0;
        gDuel_aCardRecords[o->index].flags &= ~0x3400;
        Duel_ApplyCardObjectFlags((DuelCardDisplayObject *)o);
    }
}

/* Opens the duel-side effect prompt and, once acknowledged, hands every
   occupied slot to the shared reveal animation. */
void DuelEffect_ApplyDarkPiercingLight(void)
{
    DuelEffectRequest *object;
    DuelFieldEffectObject *target;
    DuelCardRecord *record;
    u16 flags;
    s32 i;

    if (DuelEffect_MarkInitialized() == 0) {
        object = DuelEffect_AllocateRequest(0x13);
        object->field_00 = 0xA0;
        gDuelEffect_pTrackedRequest = object;
        object->field_02 = 0x68;
        SD_SEPlayFull(0x13);
        return;
    }
    flags = gDuel_wCardEffectFlags;
    if ((flags & 0x40) == 0 &&
        gDuelEffect_pTrackedRequest->progress != 0) {
        gDuel_wCardEffectFlags = flags | 0x40;
        SD_SEPlayFull(0x1D);
        for (i = DUEL_FIELD_ROW_SIZE; i < DUEL_CARD_SIDE_RECORD_COUNT; i++) {
            record = &gDuel_aCardRecords[D_800907D8_2d[gDuel_bActiveSide][i]];
            /* The retail code tests the two halfwords at +0x14 as one
               word; 0x90000000 selects bits 0x9000 of flags at +0x16. */
            if ((*(u32 *)&record->terrain_modifier & 0x90000000) ==
                0x90000000) {
                target = DUEL_FIELD_EFFECT_OBJECT_VIEW(record->object);
                target->callback = DuelEffect_UpdateRevealCard;
                target->active = 1;
            }
        }
    }
    if ((gDuel_bEffectRequestStatus & 1) == 0 && DisplayObject_FindAllocatedByTag(1) == 0) {
        gDuel_wCardEffectFlags = 0;
    }
}

/* Companion field-wide stat-penalty sweep. It advances one occupied slot of
 * the acting side's second row per countdown, spawns the effect at that card,
 * and waits on the same request-completion state as the transition above. */
void DuelEffect_ApplyStatPenalty(void) {
    DuelCardRecord *record;
    DuelEffectRequest *object;
    DisplayObject *card;
    s32 timer;
    s32 base_slot;

    if (DuelEffect_MarkInitialized() == 0) {
        gDuel_sEffectWorkCounter = 0;
        gDuel_wStepCounter = 0;
    }

    if ((gDuel_wCardEffectFlags & 0x40) != 0) {
        if ((gDuel_bEffectRequestStatus & 1) == 0) {
            gDuel_wCardEffectFlags = 0;
        }
        return;
    }

    timer = *(u16 *)&gDuel_sEffectWorkCounter - 1;
    gDuel_sEffectWorkCounter = timer;
    if ((s16)timer > 0) {
        return;
    }
    gDuel_sEffectWorkCounter = 0x10;

    base_slot = gDuel_bActiveSide * DUEL_FIELD_SIDE_GRID_SLOT_COUNT +
                DUEL_FIELD_ROW_SIZE;
    record = &gDuel_aCardRecords[D_800907D8_flat[(s16)gDuel_wStepCounter + base_slot]];
    if ((record->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
        card = DISPLAY_OBJECT_VIEW(record->object);
        object = DuelEffect_AllocateRequest(0xD);
        object->field_00 = (s16)card->field_30.h.field_30;
        object->field_02 = (s16)card->field_30.h.field_32;
        object->field_04 = (s16)*(u16 *)&card->field_34;
        object->buffer =
            (void *)((s32)object->buffer + ((s16)gDuel_wStepCounter << 14));
        if (gDuel_wEffectCardID == DUEL_SPELLBINDING_CIRCLE_CARD_ID) {
            object->phase = 2;
            record->stat_modifier =
                record->stat_modifier - DUEL_STAT_PENALTY_PER_LEVEL;
            object->field_12 = -DUEL_STAT_PENALTY_PER_LEVEL;
        } else {
            object->phase = 1;
            record->stat_modifier =
                record->stat_modifier - 2 * DUEL_STAT_PENALTY_PER_LEVEL;
            object->field_12 = -2 * DUEL_STAT_PENALTY_PER_LEVEL;
        }
        SD_SEPlayFull(0x21);
    }

    gDuel_wStepCounter = gDuel_wStepCounter + 1;
    if ((s16)gDuel_wStepCounter >= DUEL_FIELD_ROW_SIZE) {
        gDuel_wCardEffectFlags = gDuel_wCardEffectFlags | 0x40;
    }
}
