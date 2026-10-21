#include "../types.h"
#include "view_state.h"
#include "duel_effect_request.h"
#include "duel_effect_object_pool.h"
#include "../psyq/libgte.h"
#include "ordering_tables.h"
#include "save_data.h"
#include "../external_funcs.h"
#define D_8009B264_VISIBLE
#include "../unmatched.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

int func_8002C570(int offset)
{
    u8 *object;

    object = (u8 *)((u32)D_801D0000 + offset);
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
 * flag byte 0x80, the id at +0x18, the buffer pointer D_80010000 + 0x3800 at
 * +0x14, two words copied from gGraphics_apOrderingTables, and the zeroed fields. Returns the
 * entry or 0 when none was free. */
DuelEffectRequest *DuelEffect_AllocateRequest(s32 arg0)
{
    DuelEffectRequest *p = DuelEffect_FindFreeRequest();

    if (p != 0) {
        u8 *q;
        GsOT *G32 *t;
        s32 b;

        q = D_80010000;
        p->flags = DUEL_EFFECT_REQUEST_FLAG_ACTIVE;
        t = gGraphics_apOrderingTables;
        p->id = arg0;
        p->field_1A = 0;
        p->field_1D = 0;
        p->buffer = q + 0x3800;
        p->field_08 = (s32)t[2];
        b = (s32)t[1];
        p->field_10 = 8;
        p->field_00 = 0;
        p->field_02 = 0;
        p->field_04 = 0;
        p->field_12 = 0;
        p->field_0C = b;
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
   nonblocking; clears D_8009B261; if the initialized flag is not set yet,
   sets it and resets field_1A to -1; copies gGraphics_apOrderingTables[1]/[2] into
   field_0C/field_08, stashes the request at D_8009B264, and calls
   func_801462B0(id, the ORIGINAL field_1A read before the possible reset,
   buffer, request). It then restores the geometry state and clears flags
   when D_8009B261 ends up 1. Returns the blocking status.

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
            D_8009B261 = 0;
            flags2 = rec->flags;
            savedF1A = rec->field_1A;
            if (!(flags2 & DUEL_EFFECT_REQUEST_FLAG_INITIALIZED)) {
                rec->flags =
                    flags2 | DUEL_EFFECT_REQUEST_FLAG_INITIALIZED;
                rec->field_1A = -1;
            }
            rec->field_0C = (s32)gGraphics_apOrderingTables[1];
            D_8009B264 = rec;
            rec->field_08 = (s32)gGraphics_apOrderingTables[2];
            func_801462B0(rec->id, savedF1A, (s32)rec->buffer, rec);
            SetGeomOffset(0, 0);
            SetGeomScreen(D_800F2848.projection);
            if (D_8009B261 == 1) {
                rec->flags = 0;
            }
        }
        rec++;
    }

    return gDuel_bEffectRequestStatus &
           DUEL_EFFECT_REQUEST_STATUS_BLOCKING;
}
