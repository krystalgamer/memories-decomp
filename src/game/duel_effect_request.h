#ifndef MEMORIES_DECOMP_DUEL_EFFECT_REQUEST_H
#define MEMORIES_DECOMP_DUEL_EFFECT_REQUEST_H

#include "../types.h"
#include "duel_effect_allocate_request.h"

#define DUEL_EFFECT_REQUEST_OFFSET(member) \
    ((u32)&(((DuelEffectRequest *)0)->member))
#define DUEL_EFFECT_REQUEST_COUNT 8
#define DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING 0x20
#define DUEL_EFFECT_REQUEST_FLAG_INITIALIZED 0x40
#define DUEL_EFFECT_REQUEST_FLAG_ACTIVE 0x80

#define DUEL_EFFECT_REQUEST_STATUS_BLOCKING 0x01
#define DUEL_EFFECT_REQUEST_STATUS_ACTIVE 0x80

/* The record DuelEffect_CreateRequest hands a duel effect handler.
 *
 * DuelEffect_AllocateRequest is where the layout comes from: it takes a free entry from
 * DuelEffect_FindFreeRequest and fills every field below, so each one is a store in the
 * function rather than a guess. It zeroes +0x00, +0x02, +0x04 and
 * +0x12, writes 8 at +0x10, the effect id at +0x18, zero at +0x1A, the shared
 * buffer D_80010000 + 0x3800 at +0x14, two words copied out of D_800E9D90 at
 * +0x08 and +0x0C, 0x80 at +0x1C and zero at +0x1D.
 *
 * The three halfwords at +0x00, +0x02 and +0x04 are the request's payload and
 * their meaning is the effect id's business, not the record's, which is why
 * they are not named here. Two callers disagree about them and both are
 * right for their own id: DuelEffect_StartSwords (id 0x12) writes 0xA0/0x78
 * into +0x00 and +0x02, the centre of a 320x240 screen, while
 * DuelScene_UpdateResume (id 0xB) and duel_trap_resolution.c (id 8) copy a card
 * record's +0x30/+0x32/+0x34 triple straight into all three, and
 * DuelEffect_ApplyHarpiesFeatherDuster (id 0x17) reads +0x04 back as a y
 * coordinate. Naming the pair
 * x/y, which two of the private copies did, asserts the first reading over
 * the others. */
typedef struct {
    s16 field_00;   /* 0x00 */
    s16 field_02;   /* 0x02 */
    s16 field_04;   /* 0x04 */
    u8 pad_06[2];   /* 0x06 */
    s32 field_08;   /* 0x08 */
    s32 field_0C;   /* 0x0C */
    s16 field_10;   /* 0x10 */
    s16 field_12;   /* 0x12 */
    void *buffer;   /* 0x14 */
    s16 id;         /* 0x18 */
    s16 field_1A;   /* 0x1A */
    u8 flags;       /* 0x1C */
    u8 field_1D;    /* 0x1D */
} DuelEffectRequest;

typedef char DuelEffectRequest_size_must_be_0x20[
    sizeof(DuelEffectRequest) == 0x20 ? 1 : -1
];
typedef char DuelEffectRequest_field_04_offset_must_be_0x4[
    DUEL_EFFECT_REQUEST_OFFSET(field_04) == 0x4 ? 1 : -1
];
typedef char DuelEffectRequest_field_08_offset_must_be_0x8[
    DUEL_EFFECT_REQUEST_OFFSET(field_08) == 0x8 ? 1 : -1
];
typedef char DuelEffectRequest_buffer_offset_must_be_0x14[
    DUEL_EFFECT_REQUEST_OFFSET(buffer) == 0x14 ? 1 : -1
];
typedef char DuelEffectRequest_id_offset_must_be_0x18[
    DUEL_EFFECT_REQUEST_OFFSET(id) == 0x18 ? 1 : -1
];
typedef char DuelEffectRequest_field_1A_offset_must_be_0x1A[
    DUEL_EFFECT_REQUEST_OFFSET(field_1A) == 0x1A ? 1 : -1
];
typedef char DuelEffectRequest_flags_offset_must_be_0x1C[
    DUEL_EFFECT_REQUEST_OFFSET(flags) == 0x1C ? 1 : -1
];

#undef DUEL_EFFECT_REQUEST_OFFSET

/* The request the running effect handler is working on. Four handlers store
 * the entry DuelEffect_AllocateRequest hands them on their first call, under the
 * DuelEffect_MarkInitialized guard; DuelEffect_ApplyStopDefense stores each
 * marker it allocates. They read +0x1C and +0x1D from it afterwards, and
 * DuelEffect_ApplyTerrain writes +0x1A; DuelEffect_ApplySwords and
 * duel_card_effects.c read +0x1D through their own display-object
 * views and cast at the global. u8 * is DuelEffect_AllocateRequest's return type. Retail
 * reaches it gp-relative
 * at every site, 17 lw and 14 sw in nine functions, four of them
 * (DuelScene_UpdateExodiaResult, DuelScene_UpdateCardPlacement, DuelScene_UpdateBattle, DuelEffect_ApplyRitual) still
 * assembly. */
extern u8 *D_8009B17C;

/* Request flags are a complete lifecycle contract. DuelEffect_AllocateRequest
 * raises ACTIVE when it allocates an entry and DuelEffect_FindFreeRequest uses
 * that bit to find a free one. DuelEffect_UpdateRequests raises INITIALIZED
 * immediately before the first handler call and clears the whole byte when the
 * handler reports completion.
 * NONBLOCKING excludes long-running field effects from the pool's blocking
 * status while still dispatching them normally.
 *
 * The request pool's status byte below is separate. DuelEffect_CreateRequest
 * raises bit 7 when it hands out a request and DuelEffect_ResetRequestPool
 * clears the byte; DuelEffect_UpdateRequests clears bit 0, raises it again
 * while walking blocking records and returns it; DuelScene_Update drops bit 7
 * unless bit 0 is set;
 * DuelEffect_ApplyRaigeki, DuelEffect_ApplyStatPenalty and
 * DuelEffect_ApplyDarkPiercingLight test bit 0. Read lbu
 * everywhere and one byte wide (c_symbols.ld names D_8009B261 next). Retail
 * reaches it through $gp in DuelEffect_UpdateRequests,
 * DuelEffect_CreateRequest and DuelEffect_ResetRequestPool, and through
 * %hi/%lo in DuelScene_Update, DuelEffect_ApplyRaigeki,
 * DuelEffect_ApplyStatPenalty,
 * DuelEffect_ApplyDarkPiercingLight and
 * DuelScene_UpdateExodiaResult (still assembly); duel_scene_update.c and
 * duel_card_effects.c define the .data arm below for that. DuelEffect_CreateRequest.c
 * takes the plain arm. */
#ifdef gDuel_bEffectRequestStatus_IN_DATA
extern u8 gDuel_bEffectRequestStatus __attribute__((section(".data")));
#else
extern u8 gDuel_bEffectRequestStatus;
#endif

/* Allocates a request for the given effect id, marks the pool active, and
 * returns it, or 0 when the pool is full. */
DuelEffectRequest *DuelEffect_CreateRequest(s32 id);
DuelEffectRequest *DuelEffect_FindFreeRequest(void);
s32 DuelEffect_UpdateRequests(void);

/* The pool those requests live in: the eight records this header already
 * fixes the shape and count of, DUEL_EFFECT_REQUEST_COUNT of them at 0x20
 * bytes each, both asserted above.
 *
 * The bound is kept rather than dropped. Both declarers already spelled it
 * `[DUEL_EFFECT_REQUEST_COUNT]`, and an array's declared size is a -G input
 * for this toolchain, so leaving it unsized would not be a neutral tidy-up.
 *
 * All three accessors are matching C and all three take its address:
 * DuelEffect_ResetRequestPool and DuelEffect_FindFreeRequest walk it from the base, and DuelEffect_UpdateRequests
 * starts from `&D_800EAD88[0]` -- its own comment calls that "the eight
 * D_800EAD88 requests". */
extern DuelEffectRequest D_800EAD88[DUEL_EFFECT_REQUEST_COUNT];

#endif
