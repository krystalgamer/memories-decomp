#ifndef MEMORIES_DECOMP_DUEL_EFFECT_REQUEST_H
#define MEMORIES_DECOMP_DUEL_EFFECT_REQUEST_H

#include "../types.h"

#define DUEL_EFFECT_REQUEST_OFFSET(member) \
    ((u32)&(((DuelEffectRequest *)0)->member))
#define DUEL_EFFECT_REQUEST_COUNT 8
#define DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING 0x20
#define DUEL_EFFECT_REQUEST_FLAG_INITIALIZED 0x40
#define DUEL_EFFECT_REQUEST_FLAG_ACTIVE 0x80

/* The record func_8002C68C hands a duel effect handler.
 *
 * func_8002C604 is where the layout comes from: it takes a free entry from
 * func_8002C5CC and fills every field below, so each one is a store in the
 * function rather than a guess. It zeroes +0x00, +0x02, +0x04 and
 * +0x12, writes 8 at +0x10, the effect id at +0x18, zero at +0x1A, the shared
 * buffer D_80010000 + 0x3800 at +0x14, two words copied out of D_800E9D90 at
 * +0x08 and +0x0C, 0x80 at +0x1C and zero at +0x1D.
 *
 * The three halfwords at +0x00, +0x02 and +0x04 are the request's payload and
 * their meaning is the effect id's business, not the record's, which is why
 * they are not named here. Two callers disagree about them and both are
 * right for their own id: func_80025EE0 (id 0x12) writes 0xA0/0x78 into +0x00
 * and +0x02, the centre of a 320x240 screen, while func_8001825C (id 0xB) and
 * duel_trap_resolution.c (id 8) copy a card record's +0x30/+0x32/+0x34 triple
 * straight into all three, and func_80026A3C (id 0x17) reads +0x04 back as a
 * y coordinate. Naming the pair x/y, which two of the private copies did,
 * asserts the first reading over the others. */
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
 * the entry func_8002C604 hands them on their first call, under the
 * DuelEffect_MarkInitialized guard; DuelEffect_UpdateFieldMarker stores each
 * marker it allocates. They read +0x1C and +0x1D from it afterwards, and
 * func_80024E58 writes +0x1A; src/candidates/func_80025F3C.c and
 * duel_card_effects.c read +0x1D through their own display-object
 * views and cast at the global. u8 * is func_8002C604's return type. Retail
 * reaches it gp-relative
 * at every site, 17 lw and 14 sw in nine functions, four of them
 * (func_80018FEC, func_80019D18, func_8001F55C, func_800262D4) still
 * assembly. */
extern u8 *D_8009B17C;

/* Request flags are a complete lifecycle contract. func_8002C604 raises
 * ACTIVE when it allocates an entry and func_8002C5CC uses that bit to find
 * a free one. func_8002C6C8 raises INITIALIZED immediately before the first
 * handler call and clears the whole byte when the handler reports completion.
 * NONBLOCKING excludes long-running field effects from the pool's blocking
 * status while still dispatching them normally.
 *
 * The request pool's status byte below is separate. func_8002C68C raises bit
 * 7 when it hands out a request and func_8002C598 clears the byte;
 * func_8002C6C8 clears bit 0, raises it again while walking blocking records
 * and returns it; func_80024200 drops bit 7 unless bit 0 is set;
 * func_8002596C, func_80025D30 and func_80025BEC test bit 0. Read lbu
 * everywhere and one byte wide (c_symbols.ld names D_8009B261 next). Retail
 * reaches it through $gp in func_8002C6C8, func_8002C68C and func_8002C598,
 * and through %hi/%lo in func_80024200, func_8002596C, func_80025D30,
 * func_80025BEC and func_80018FEC (still assembly); duel_scene_update.c and
 * duel_card_effects.c define the .data arm below for that. func_8002C68C.c
 * takes the plain arm. */
#ifdef D_8009B260_IN_DATA
extern u8 D_8009B260 __attribute__((section(".data")));
#else
extern u8 D_8009B260;
#endif

/* Allocates a request for the given effect id, marks D_8009B260 and returns
 * it, or 0 when the pool is full. */
DuelEffectRequest *func_8002C68C(s32 id);

/* The pool those requests live in: the eight records this header already
 * fixes the shape and count of, DUEL_EFFECT_REQUEST_COUNT of them at 0x20
 * bytes each, both asserted above.
 *
 * The bound is kept rather than dropped. Both declarers already spelled it
 * `[DUEL_EFFECT_REQUEST_COUNT]`, and an array's declared size is a -G input
 * for this toolchain, so leaving it unsized would not be a neutral tidy-up.
 *
 * All three accessors are matching C and all three take its address:
 * func_8002C598 and func_8002C5CC walk it from the base, and func_8002C6C8
 * starts from `&D_800EAD88[0]` -- its own comment calls that "the eight
 * D_800EAD88 requests". */
extern DuelEffectRequest D_800EAD88[DUEL_EFFECT_REQUEST_COUNT];

#endif
