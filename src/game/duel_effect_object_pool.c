#define D_8009B264_VISIBLE
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#define D_8009B1D5_IS_ABSOLUTE_SCALAR
#include "../types.h"
#include "view_state.h"
#include "duel_effect_request.h"

/* Zero-initialized storage this unit owns at 0x800EAD88-0x800EAE88, defined in
 * address order ahead of the headers that declare it (#2602). */
DuelEffectRequest gDuelEffect_aRequests[8];

/* Zero-initialized storage this unit owns at 0x8009B260-0x8009B268, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 gDuel_bEffectRequestStatus;
u8 gDuelEffect_bRequestFinished;
DuelEffectRequest *G32 gDuelEffect_pActiveRequest;

#include "duel_effect_object_pool.h"
#include "../psyq/libgte.h"
#include "ordering_tables.h"
#include "save_data.h"
#include "../external_funcs.h"
#include "../unmatched.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "view_state_current.h"
#include "save_data_workspace.h"
#include "duel_check_ritual.h"
#include "duel_side_state.h"
#include "duel_card.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

int Library_CheckCardInChest(int offset)
{
    u8 *object;

    object = (u8 *)((u32)gSaveData_Workspace + offset);
    if (object[SAVE_DATA_HEADER_SIZE + SAVE_DATA_CARD_QUANTITIES_OFFSET -
               CARD_ID_FIRST]) {
        return 1;
    }
    return -1;
}

void DuelEffect_ResetRequestPool(void)
{
    DuelEffectRequest *entry;
    int count;
    int fill;

    gDuel_bEffectRequestStatus = 0;
    count = DUEL_EFFECT_REQUEST_COUNT;
    fill = -1;
    entry = gDuelEffect_aRequests;
    do {
        entry->flags = 0;
        entry->id = fill;
        entry++;
    } while (--count != 0);
}

DuelEffectRequest *DuelEffect_FindFreeRequest(void)
{
    DuelEffectRequest *entry = gDuelEffect_aRequests;
    int count = DUEL_EFFECT_REQUEST_COUNT;

    for (;;) {
        if (!(entry->flags & DUEL_EFFECT_REQUEST_FLAG_ACTIVE)) {
            return entry;
        }
        if (--count == 0) {
            return 0;
        }
        entry++;
    }
}

/* Allocates a request entry through DuelEffect_FindFreeRequest and fills it:
 * flag byte 0x80, the id at +0x18, the buffer pointer gRuntime_pSharedWorkBase + 0x3800 at
 * +0x14, two words copied from gGraphics_apOrderingTables, and the zeroed fields. Returns the
 * entry or 0 when none was free. */
DuelEffectRequest *DuelEffect_AllocateRequest(s32 arg0)
{
    DuelEffectRequest *p = DuelEffect_FindFreeRequest();

    if (p != 0) {
        u8 *q;
        GsOT *G32 *t;
        s32 b;

        q = gRuntime_pSharedWorkBase;
        p->flags = DUEL_EFFECT_REQUEST_FLAG_ACTIVE;
        t = gGraphics_apOrderingTables;
        p->id = arg0;
        p->phase = 0;
        p->progress = 0;
        p->buffer = q + 0x3800;
        p->ordering_table_2 = (s32)t[2];
        b = (s32)t[1];
        p->base_priority = 8;
        p->field_00 = 0;
        p->field_02 = 0;
        p->field_04 = 0;
        p->field_12 = 0;
        p->ordering_table_1 = b;
    }

    return p;
}

/* Hands out one request: DuelEffect_AllocateRequest allocates it for the
 * caller's effect id, and bit 7 of the pool's status byte records that one is
 * out. The id
 * reaches the allocator in $a0 untouched, so forwarding it costs nothing;
 * this definition used to take no parameter and leave the id ambient, which
 * compiles to the same instructions. */
DuelEffectRequest *DuelEffect_CreateRequest(s32 id)
{
    DuelEffectRequest *value = DuelEffect_AllocateRequest(id);

    if (value != 0) {
        gDuel_bEffectRequestStatus |= DUEL_EFFECT_REQUEST_STATUS_ACTIVE;
    }
    return value;
}

/* Clears the blocking status, then walks the eight gDuelEffect_aRequests.
   For each active request: raises the blocking status unless the request is
   nonblocking; clears gDuelEffect_bRequestFinished; if the initialized flag is not set yet,
   sets it and resets phase to -1; copies gGraphics_apOrderingTables[1]/[2] into
   ordering_table_1/ordering_table_2, stashes the request at gDuelEffect_pActiveRequest, and calls
   DuelEffect_DispatchRequest(id, the ORIGINAL phase read before the possible reset,
   buffer, request). It then restores the geometry state and clears flags
   when gDuelEffect_bRequestFinished ends up 1. Returns the blocking status.

   savedF1A must be s32, not the field's true s16 type -- declaring it s16
   makes gcc emit an unsigned reload + manual sign-extend at the call site
   instead of the target's plain signed halfword load. */
s32 DuelEffect_UpdateRequests(void) {
    DuelEffectRequest *rec;
    s32 i;

    gDuel_bEffectRequestStatus =
        gDuel_bEffectRequestStatus & ~DUEL_EFFECT_REQUEST_STATUS_BLOCKING;

    rec = &gDuelEffect_aRequests[0];
    for (i = DUEL_EFFECT_REQUEST_COUNT; i != 0; i--) {
        u8 flags1 = rec->flags;
        if (flags1 & DUEL_EFFECT_REQUEST_FLAG_ACTIVE) {
            u8 flags2;
            s32 savedF1A;

            if (!(flags1 & DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING)) {
                gDuel_bEffectRequestStatus |=
                    DUEL_EFFECT_REQUEST_STATUS_BLOCKING;
            }
            gDuelEffect_bRequestFinished = 0;
            flags2 = rec->flags;
            savedF1A = rec->phase;
            if (!(flags2 & DUEL_EFFECT_REQUEST_FLAG_INITIALIZED)) {
                rec->flags =
                    flags2 | DUEL_EFFECT_REQUEST_FLAG_INITIALIZED;
                rec->phase = -1;
            }
            rec->ordering_table_1 = (s32)gGraphics_apOrderingTables[1];
            gDuelEffect_pActiveRequest = rec;
            rec->ordering_table_2 = (s32)gGraphics_apOrderingTables[2];
            DuelEffect_DispatchRequest(rec->id, savedF1A, (s32)rec->buffer, rec);
            SetGeomOffset(0, 0);
            SetGeomScreen(gViewState_Current.projection);
            if (gDuelEffect_bRequestFinished == 1) {
                rec->flags = 0;
            }
        }
        rec++;
    }

    return gDuel_bEffectRequestStatus &
           DUEL_EFFECT_REQUEST_STATUS_BLOCKING;
}

s32 Duel_CheckRitual(DuelRitualResult *out, s32 ritualId)
{
    DuelCardRecord *found[DUEL_RITUAL_TRIBUTE_COUNT];
    DuelCardRecord *cands[DUEL_FIELD_ROW_SIZE];
    DuelCardRecord *card;
    DuelCardRecord **first;
    DuelCardRecord **dst;
    DuelCardRecord **w;
    DuelCardRecord *c;
    u16 *p;
    u16 *q;
    s32 i;
    s32 j;

    p = gDuel_awRitualData;
    while (1) {
        if (p[0] == 0) {
            return 0;
        }
        if (p[0] == ritualId) {
            break;
        }
        p += DUEL_RITUAL_RECIPE_HALFWORD_COUNT;
    }

    i = DUEL_FIELD_ROW_SIZE;
    if (gDuel_bActiveSide != 0) {
        i = DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE;
    }
    c = &gDuel_aCardRecords[i];
    i = 0;
    w = cands;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        *w = 0;
        if (c->flags & DUEL_CARD_FLAG_OCCUPIED) {
            *w = c;
        }
        w++;
        c++;
    }

    p++;
    j = 0;
    first = cands;
    dst = found;
    q = p;
    for (j = 0; j < DUEL_RITUAL_TRIBUTE_COUNT; j++) {
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            card = (c = first[i]);
            if (card != 0 && card->card_id == q[0]) {
                goto matched;
            }
        }
        return 0;
matched:
        *dst++ = card;
        first[i] = 0;
        q++;
    }

    if (out != 0) {
        for (i = 0; i < DUEL_RITUAL_TRIBUTE_COUNT; i++) {
            out->tribute_objects[i] = found[i]->object;
        }
        out->field_0C = 0;
    }
    return p[DUEL_RITUAL_TRIBUTE_COUNT];
}
