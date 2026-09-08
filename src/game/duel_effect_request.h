#ifndef MEMORIES_DECOMP_DUEL_EFFECT_REQUEST_H
#define MEMORIES_DECOMP_DUEL_EFFECT_REQUEST_H

#include "../types.h"

#define DUEL_EFFECT_REQUEST_OFFSET(member) \
    ((u32)&(((DuelEffectRequest *)0)->member))

/* The record func_8002C68C hands a duel effect handler.
 *
 * func_8002C604.c is where the layout comes from: it takes a free entry from
 * func_8002C5CC and fills every field below, so each one is a store in a
 * matched function rather than a guess. It zeroes +0x00, +0x02, +0x04 and
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

/* Allocates a request for the given effect id, marks D_8009B260 and returns
 * it, or 0 when the pool is full.
 *
 * func_8002C68C.c deliberately does not include this header: the image's own
 * definition takes no argument and returns void *, and the id every caller
 * passes reaches func_8002C604 in $a0 through that prototype-free call. This
 * is the declaration the callers need; the definition keeps the one the
 * instructions have. */
DuelEffectRequest *func_8002C68C(s32 id);

#endif
