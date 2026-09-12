#include "../types.h"
#include "view_state.h"
#include "duel_effect_request.h"
#include "func_8002C6C8.h"
#include "../psyq/libgte.h"
#include "ordering_tables.h"

/* Clears D_8009B260's bit 0x1, then walks the eight D_800EAD88 requests.
   For each active request: raises D_8009B260's bit 0 unless the request is
   nonblocking; clears D_8009B261; if the request is not initialized yet,
   marks it initialized and resets field_1A to -1; copies D_800E9D90[1]/[2]
   into field_0C/field_08, stashes the request at D_8009B264, and calls
   func_801462B0(id, the ORIGINAL field_1A read before the possible reset,
   buffer, request). It then restores the geometry state and clears flags
   when D_8009B261 ends up 1. Returns D_8009B260's bit 0.

   savedF1A must be s32, not the field's true s16 type -- declaring it s16
   makes gcc emit an unsigned reload + manual sign-extend at the call site
   instead of the target's plain signed halfword load. */
extern u8 D_8009B261;
extern DuelEffectRequest *D_8009B264;
extern void func_801462B0(
    s16 a0, s16 a1, s32 a2, DuelEffectRequest *a3
);

s32 func_8002C6C8(void) {
    DuelEffectRequest *rec;
    s32 i;

    D_8009B260 = D_8009B260 & 0xFE;

    rec = &D_800EAD88[0];
    for (i = DUEL_EFFECT_REQUEST_COUNT; i != 0; i--) {
        u8 flags1 = rec->flags;
        if (flags1 & DUEL_EFFECT_REQUEST_FLAG_ACTIVE) {
            u8 flags2;
            s32 savedF1A;

            if (!(flags1 & DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING)) {
                D_8009B260 = D_8009B260 | 1;
            }
            D_8009B261 = 0;
            flags2 = rec->flags;
            savedF1A = rec->field_1A;
            if (!(flags2 & DUEL_EFFECT_REQUEST_FLAG_INITIALIZED)) {
                rec->flags =
                    flags2 | DUEL_EFFECT_REQUEST_FLAG_INITIALIZED;
                rec->field_1A = -1;
            }
            rec->field_0C = (s32)D_800E9D90[1];
            D_8009B264 = rec;
            rec->field_08 = (s32)D_800E9D90[2];
            func_801462B0(rec->id, savedF1A, (s32)rec->buffer, rec);
            SetGeomOffset(0, 0);
            SetGeomScreen(D_800F2848.projection);
            if (D_8009B261 == 1) {
                rec->flags = 0;
            }
        }
        rec++;
    }

    return D_8009B260 & 1;
}
