#ifndef MEMORIES_DECOMP_FUNC_800300C8_H
#define MEMORIES_DECOMP_FUNC_800300C8_H

#include "../types.h"

/* The two records func_800300C8 reads and writes. Both are narrow views: they
 * name only the fields this function touches and carry the rest as padding, so
 * neither is a claim about the whole record. */

/* Read side, D_800EB15C. Only the three values at 0x3C are named; the other
 * three files that use this symbol pass it around as u8 * and never look
 * inside, so nothing else constrains the shape. */
struct LayoutSource {
    u8 pad0[0x3C];
    s16 base;
    s16 spacing;
    u16 row;
};

/* Write side, D_8009B2E4: four x/y pairs at a 8-byte stride from 0x28.
 *
 * This is NOT the whole record, and deliberately not merged with the other
 * views of the same pointer. func_80031354.c describes D_8009B2E4 with its own
 * LocalRecord naming a u16 at offset 0x08, and duel_interface_setup.c holds it
 * as void *. The three views do not disagree -- the fields named here start at
 * 0x28, so they do not overlap LocalRecord's 0x08 at all -- but no file reads
 * both regions, so there is no evidence for a single combined layout and
 * combining them would assert one. */
struct LayoutTarget {
    u8 pad0[0x28];
    s16 field28;
    s16 field2A;
    u8 pad2C[4];
    s16 field30;
    s16 field32;
    u8 pad34[4];
    s16 field38;
    s16 field3A;
    u8 pad3C[4];
    s16 field40;
    s16 field42;
};

void func_800300C8(void);

#endif
