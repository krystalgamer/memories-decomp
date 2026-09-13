#include "../types.h"
#include "duel_effect_object_pool.h"
#include "duel_effect_request.h"
#include "ordering_tables.h"

#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "high_memory_addresses.h"

/* Allocates a request entry through DuelEffect_FindFreeRequest and fills it: flag byte
 * 0x80, the id at +0x18, the buffer pointer D_80010000 + 0x3800 at +0x14,
 * two words copied from D_800E9D90, and the zeroed fields. Returns the entry
 * or 0 when none was free. */
u8 *DuelEffect_AllocateRequest(s32 arg0) {
    DuelEffectRequest *p = DuelEffect_FindFreeRequest();

    if (p != 0) {
        u8 *q;
        GsOT **t;
        s32 b;

        q = D_80010000;
        p->flags = DUEL_EFFECT_REQUEST_FLAG_ACTIVE;
        t = D_800E9D90;
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

    return (u8 *)p;
}
