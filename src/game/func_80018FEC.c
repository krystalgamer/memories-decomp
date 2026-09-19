/*
 * Stages the five-card Exodia presentation, sparkle phases, centre burst, and
 * result handoff. The typed DuelEffectObject accumulator view, typed
 * DisplayObject cursor, and cross-path reuse of `side` preserve the retail
 * scheduling and register allocation. The first position store keeps a signed
 * address-of-member view so GCC materializes -0x40 like retail. The two
 * request-completion byte tests retain the global byte pointer but derive
 * their index from DuelEffectRequest::field_1D.
 */
#define gDuel_bEffectRequestStatus_IN_DATA
#define D_8009B369_IN_DATA
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#define ANIMATED_BATTLE_MODEL_PROPERTIES_AS_HALFWORDS
#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/duel_effect_request.h"
#include "../game/duel_card_pick_cursor.h"
#include "../game/duel_selection_layout.h"
#include "../game/duel_card_layout.h"
#include "../game/duel_card_staging.h"
#include "../game/duel_scene_card_placement.h"
#include "../game/duel_scene_exodia_result.h"
#include "../game/duel_scene_state.h"
#include "../game/duel_selection_layout.h"
#include "../game/duel_screen_tables.h"
#include "../game/rand_get_interval.h"
#include "../game/fade.h"
#include "../game/display_object_motion.h"
#include "../game/display_object_work_slots.h"
#include "../game/display_object_core.h"
#include "../game/display_object_helpers.h"
#define D_8009B214_AS_BYTE_POINTER
#define D_8009B21C_AS_BYTE_POINTER
#include "../game/duel_init_scene.h"
#define D_8009B269_AS_SCALAR_DATA
#define D_8009B26C_AS_SCALAR_DATA
#include "../game/duel_effect.h"
#include "../game/duel_effect_allocate_request.h"
#include "../unmatched.h"

#define DUEL_EFFECT_REQUEST_FIELD_1D_OFFSET \
    ((u32)&((DuelEffectRequest *)0)->field_1D)
#include "../game/sound_output.h"
#include "../game/sound.h"
#include "../game/model_scene_states.h"
#include "../game/main_modes.h"
#include "../game/model.h"
#include "../psyq/rand.h"
#include "../game/main_mode_state.h"

#define DUEL_EFFECT_OBJECT_VIEW(object) ((DuelEffectObject *)(object))
#define DISPLAY_OBJECT_VIEW(object) ((DisplayObject *)(object))

extern s32 D_800E9F04[];

void DuelScene_UpdateExodiaResult(void)
{
    u16 flags;
    DisplayObject *obj;
    u8 *pose;
    u8 *rec;
    DuelEffectObject *fx;
    s32 i;
    s32 anim;
    s32 n;
    s32 r;
    s32 t14;
    u32 sum;
    u8 *cards;
    u8 *poses;
    DisplayObject **objs;
    s32 fnv;
    DuelCardReplayRecordBlock *g;
    s32 py;
    s32 t;
    s32 k;
    DisplayObject **slot;
    s8 side;
    DuelSideState *other;
    DisplayObject *d;

    flags = D_8009B23A;
    if ((flags & DUEL_SCENE_FLAG_INITIALIZED) == 0) {
        i = 0;
        cards = D_8015C424;
        poses = (u8 *)&D_80090918;
        r = (s32)func_8001EC70;
        fnv = r;
        objs = D_800E9EF0;
        D_8009B23A = flags | DUEL_SCENE_FLAG_INITIALIZED;
        obj = DISPLAY_OBJECT_VIEW(D_8009B214);
        side = D_8009B1D5;
        D_8009B1B4 =
            (DuelCardPickCursor *)&D_800E9F10[
                (u8)side * DUEL_SELECTION_SIDE_SIZE
            ];
        *(s16 *)&obj->position.h.field_28 = -0x40;
        obj->field_2C.h.field_2C = 0x10;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = obj->field_30.h.field_32;
        obj = DISPLAY_OBJECT_VIEW(D_8009B21C);
        obj->position.h.field_28 = 0x180;
        obj->field_2C.h.field_2C = 0x10;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = obj->field_30.h.field_32;
        rec = (u8 *)D_800EA030;
next_obj:
        obj = *(DisplayObject **)rec;
        /* Keep the typed record offset left of the staging base. */
        g = (DuelCardReplayRecordBlock *)(
            (u32)&((DuelCardRecord *)0)[obj->field_6A] +
            (u32)cards + DUEL_CARD_STAGING_REPLAY_BASE_OFFSET
        );
        anim = g->record.card_id - 0x11;
        pose = (u8 *)(anim * 3 + (u32)poses);
        obj->position.h.field_28 = pose[1] - 0x1A;
        py = pose[2];
        obj->field_2C.h.field_2C = 0xB4;
        obj->field_6C = 1;
        obj->update = (DisplayObjectCallback)fnv;
        obj->position.h.field_2A = py - 0x1E;
        objs[pose[0]] = obj;
        *(u8 **)rec = 0;
        i++;
        rec += sizeof(DuelHandSlot);
        if (i < DISPLAY_OBJECT_WORK_SLOT_COUNT) {
            goto next_obj;
        }
        D_800E9F04[0] = 0;
        DisplayObject_ReleaseIfPresent(
            ((DuelSelectionRecord *)D_8009B1B4)->cursor_object);
        D_8009B162 = 8;
        D_8009B1D0 = 0;
        D_8009B1B9 = 0;
        D_8009B17C = 0;
        D_8009B23A = D_8009B23A | 0x4000;
        func_80015C84();
        D_800E9ECF[0] = 2;
        SD_BGMFadeOutWithStep(2);
        return;
    }
    if (flags & 0x4000) {
        if (flags & 0x2000) {
            if ((gDuel_bEffectRequestStatus & 1) != 0) {
                return;
            }
            D_8009B23A = flags & 0xBFFF;
            D_8009B1B9 = 0;
            D_8009B1D0 = 0;
            return;
        }
        if (D_8009B17C != 0) {
            if (D_8009B17C[DUEL_EFFECT_REQUEST_FIELD_1D_OFFSET] == 0) {
                return;
            }
            SD_SEPlayFull(0x1D);
            if (DisplayObject_FindAllocatedByTag(1) == 0) {
                D_8009B23A = D_8009B23A | 0x2000;
                return;
            }
        }
        fx = DUEL_EFFECT_OBJECT_VIEW(DuelEffect_AllocateRequest(0x13));
        D_8009B17C = (u8 *)fx;
        fx->x = (rand() & 0xFF) + 0x20;
        r = Rand_GetInterval(0xB0);
        n = D_8009B1B9;
        t14 = fx->field_14;
        fx->y = r + 0x20;
        fx->field_14 = t14 + ((n & 3) << 13);
        D_8009B1B9 = n + 1;
        return;
    }
    if (flags & 0x2000) {
        t = D_8009B1D0 - 1;
        D_8009B1D0 = t;
        if ((s16)t > 0) {
            return;
        }
        if ((s8)D_8009B1B9 >= DISPLAY_OBJECT_WORK_SLOT_COUNT) {
            D_8009B23A = (flags & 0xDFFF) | 0x1000;
            fx = DUEL_EFFECT_OBJECT_VIEW(DuelEffect_AllocateRequest(0x13));
            fx->x = 0xA0;
            fx->y = 0x78;
            D_8009B17C = (u8 *)fx;
            fx->field_14 = fx->field_14 + 0x8000;
            return;
        }
        D_8009B1D0 = 4;
        fx = DUEL_EFFECT_OBJECT_VIEW(DuelEffect_AllocateRequest(0));
        k = (s8)D_8009B1B9;
        slot = &D_800E9EF0[k];
        d = *slot;
        fx->x = d->field_30.h.field_30 + 0x1A;
        d = *slot;
        fx->y = d->field_30.h.field_32 + 0x1E;
        sum = (k << 12) + 0xA000;
        anim = fx->field_14 + sum;
        fx->field_14 = anim;
        fx->field_1A = 9;
        SD_SEPlayFull(0x17);
        D_8009B1B9 = D_8009B1B9 + 1;
        return;
    }
    if (flags & 0x1000) {
        if (D_8009B17C[DUEL_EFFECT_REQUEST_FIELD_1D_OFFSET] == 0) {
            return;
        }
        D_8009B23A = flags & 0xEFFF;
        SD_SEPlayFull(0x1D);
        DuelEffect_CreateRequest(0x18);
        func_8003FF88(0x8021);
        return;
    }
    func_800156DC();
    D_800EF658[0] = MODEL_SPECIAL_BATTLE_ID;
    side = D_8009B1D5;
    gDuel_bWinnerSide = side;
    D_800E9FF0[(u8)side].rank.result_adjustment = 0x28;
    other = &D_800E9FF0[D_8009B1D5 ^ 1];
    other->life_points.unsigned_value = 0;
    other->displayed_life_points = 0;
    func_800472A8(0x7310);
    Model_SetPresentationBgmCommand(0x7310);
    D_8009B369 = 1;
    D_8009B269 = 3;
    D_8009B26C = 1;
}
