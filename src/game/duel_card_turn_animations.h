#ifndef MEMORIES_DECOMP_DUEL_CARD_TURN_ANIMATIONS_H
#define MEMORIES_DECOMP_DUEL_CARD_TURN_ANIMATIONS_H

#include "../types.h"

/* The fields the two card turn/flip animation steps read and write on a duel
 * card display object.
 *
 * This is NOT a full display-object description: it covers only the offsets
 * these two functions touch, and the padding names record the gaps rather
 * than claiming the memory is unused. Other translation units describe the
 * same object through their own, differently shaped views.
 *
 * The three bytes at 0x20..0x22 are written both individually and as one
 * 32-bit store, so they are modelled as a union. The 32-bit stores are the
 * retail code's way of setting all three channels at once: little-endian,
 * 0x8000 sets channel[1], 0xC000C0 sets channel[0] and channel[2], and
 * 0x800000 sets channel[2]. The same bytes are separately assigned the high
 * byte of the animation angle, which is what drives the tint across a flip.
 *
 * The angle at 0x2A is likewise a union. Every read and every stepping store
 * treats it as unsigned, and shifting its high byte out relies on that. The
 * two settle constants are stored signed, which is not interchangeable: a
 * negative 16-bit immediate is materialised with addiu, while the same
 * bit pattern written as unsigned becomes ori. Both spellings are therefore
 * kept rather than picking one and casting. */
typedef struct {
    u8 pad_00[0x08];
    u16 flags;
    u8 pad_0A[0x16];
    union {
        s32 packed;
        u8 channel[4];
    } tint;
    s32 field_24;
    u16 angle_step;
    union {
        u16 unsigned_value;
        s16 signed_value;
    } angle;
    u8 pad_2C[0x02];
    s16 mode;
    u8 pad_30[0x30];
    u16 counter;
    u8 pad_62[0x08];
    u8 record_index;
    u8 pad_6B[0x01];
    u8 phase;
} DuelCardTurnObject;

void func_80022674(DuelCardTurnObject *p);
void func_800229F4(DuelCardTurnObject *p);

#endif
