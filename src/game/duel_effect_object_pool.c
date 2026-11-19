#include "../types.h"
#ifdef VERSION_JAPAN
/* Declares the printf of the Japanese selector trace below. */
#include "../psyq/stdio.h"
#endif

#include "duel_effect_object_pool.h"
#include "../psyq/libgte.h"
#include "ordering_tables.h"
#include "mem_card.h"
#include "../overlays/duel_effects/dispatch.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "view_state_orbit.h"
#include "duel_scene.h"
#include "duel_card.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"
#include "card_presentation.h"
#include "display_object.h"
#include "display_object_work_slots.h"

/* Zero-initialized storage this unit owns. Each build's generated
 * duel_effect_object_pool_commons subsegments place it at the retail
 * addresses. */
u8 gDuel_bEffectRequestStatus;
u8 gDuelEffect_bRequestFinished;
DuelEffectRequest *gDuelEffect_pActiveRequest;
DuelEffectRequest gDuelEffect_aRequests[8];

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
DuelEffectRequest *DuelEffect_AllocateRequest(s32 id)
{
    DuelEffectRequest *p = DuelEffect_FindFreeRequest();

    if (p != 0) {
        u8 *q;
        GsOT **t;
        GsOT *b;

        q = gRuntime_pSharedWorkBase;
        p->flags = DUEL_EFFECT_REQUEST_FLAG_ACTIVE;
        t = gGraphics_apOrderingTables;
        p->id = id;
        p->phase = 0;
        p->progress = 0;
        p->buffer = q + 0x3800;
        p->ordering_table_2 = t[2];
        b = t[1];
        p->base_priority = 8;
        p->x = 0;
        p->y = 0;
        p->z = 0;
        p->number = 0;
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
            rec->ordering_table_1 = gGraphics_apOrderingTables[1];
            gDuelEffect_pActiveRequest = rec;
            rec->ordering_table_2 = gGraphics_apOrderingTables[2];
            DuelEffect_DispatchRequest(rec->id, savedF1A, rec->buffer, rec);
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

    p = gDuel_awRitualData.recipe_words;
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
        for (i = 0;;) {
            card = (c = first[i]);
            if (card == 0 || card->card_id != q[0]) {
                i++;
                if (i < DUEL_FIELD_ROW_SIZE) {
                    continue;
                }
                return 0;
            }
            *dst++ = card;
            first[i] = 0;
            q++;
            break;
        }
    }

    if (out != 0) {
        for (i = 0; i < DUEL_RITUAL_TRIBUTE_COUNT; i++) {
            out->tribute_objects[i] = found[i]->object;
        }
        out->terminator = 0;
    }
    return p[DUEL_RITUAL_TRIBUTE_COUNT];
}

void Duel_CollectFieldRowCardObjects(DisplayObject **output, s32 back_row)
{
    s32 base =
        gDuel_bActiveSide ? DUEL_FIELD_ROW_SIZE : DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    DuelCardRecord *entry;

    if (back_row != 0) {
        base += DUEL_FIELD_ROW_SIZE;
    }
    entry = &gDuel_aCardRecords[base];
    base = 0;
    for (; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
        if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) != 0) {
            *output++ = entry->object;
        }
    }
    *output = 0;
}

void Duel_CollectMatchingFieldCardObjects(DisplayObject **output, s32 selector)
{
    DuelCardRecord *entry;
    s32 base;
    DuelCardRecord *other;

    if (selector < 0) {
        entry = gDuel_aFieldCardRecords;
        other = entry + DUEL_CARD_SIDE_RECORD_COUNT;
        for (
            base = 0;
            base < DUEL_FIELD_SIDE_ZONE_COUNT;
            base++, entry++, other++
        ) {
            if (entry->flags & DUEL_CARD_FLAG_OCCUPIED)
                *output++ = entry->object;
            if (other->flags & DUEL_CARD_FLAG_OCCUPIED)
                *output++ = other->object;
        }
        *output = 0;
        return;
    }

    base = gDuel_bActiveSide ?
        DUEL_FIELD_ROW_SIZE : DUEL_FIELD_SIDE_GRID_SLOT_COUNT;
    entry = &gDuel_aCardRecords[base];
    if (selector >= 21) {
#ifdef VERSION_JAPAN
        /* The Japanese build still carries a debug print of the selector. */
        printf("Ex %d\n", selector);
#endif
        for (base = 0; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
            if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) &&
                (u16)Duel_CalcCardStats(entry) >= selector)
                *output++ = entry->object;
        }
    } else {
        for (base = 0; base < DUEL_FIELD_ROW_SIZE; base++, entry++) {
            if ((entry->flags & DUEL_CARD_FLAG_OCCUPIED) &&
                (selector < 0 ||
                 entry->object->resource_branch_index == selector))
                *output++ = entry->object;
        }
    }
    *output = 0;
}

void DisplayObject_CopyWorkSlots(s32 *destination)
{
    s32 i;
    DisplayObject **source;

    i = 0;
    source = gDisplayObject_apWorkSlots.slots;
    while (i < DISPLAY_OBJECT_WORK_SLOT_COUNT) {
        *destination++ = (s32)*source++;
        i++;
    }
    *destination = 0;
}
