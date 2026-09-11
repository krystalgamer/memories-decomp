#ifndef MEMORIES_DECOMP_SORTED_ENTRY_H
#define MEMORIES_DECOMP_SORTED_ENTRY_H

#include "../types.h"

/* The list of pending eight-byte entries that func_800355C8 sorts and whose
 * inverse index links it rebuilds.
 *
 * func_80035680 opens the list: it points both the base D_8009B304 and the
 * append pointer D_8009B310 at the same address and zeroes the count
 * D_8009B314.  func_80033CF8 appends one entry per call, advancing D_8009B310
 * by one entry and bumping D_8009B314, so the live entries are always
 * [D_8009B304, D_8009B310).  func_800355C8 sorts them, writes the inverse
 * links, and resets D_8009B310 back to the base.
 *
 * The comparator func_80035598 compares the u32 at offset 0 of an entry.  On
 * this target that word holds `distance` in the high halfword and `packed` in
 * the low one, so the order is ascending by distance, ties broken by `packed`.
 */
typedef struct {
    /* ratan2(dx, dz) / 16, clamped to 255 and shifted left by 8, or'd with
     * dy >> 4.  The two halves are never read apart, so they are not named
     * separately here. */
    s16 packed;
    /* SquareRoot0(dx * dx + dz * dz). */
    s16 distance;
    /* The value of D_8009B314 when the entry was appended, i.e. the entry's
     * position in append order. */
    s16 append_index;
    /* Written by func_800355C8, which stores each entry's sorted position
     * into the slot named by that entry's append_index. */
    s16 sorted_position;
} SortedEntry;

#define SORTED_ENTRY_OFFSET(member) ((u32)&(((SortedEntry *)0)->member))

typedef char SortedEntry_size_must_be_8[
    sizeof(SortedEntry) == 8 ? 1 : -1
];
typedef char SortedEntry_distance_must_be_at_2[
    SORTED_ENTRY_OFFSET(distance) == 2 ? 1 : -1
];
typedef char SortedEntry_append_index_must_be_at_4[
    SORTED_ENTRY_OFFSET(append_index) == 4 ? 1 : -1
];
typedef char SortedEntry_sorted_position_must_be_at_6[
    SORTED_ENTRY_OFFSET(sorted_position) == 6 ? 1 : -1
];

/* func_80035668 stores 0x808080 here alongside its write to D_8009B30C.
 * Nothing else in the decompiled tree reads it, so its role is unverified. */
extern u32 D_8009B300;

extern SortedEntry *D_8009B304;
/* The entry count, saved by func_800355C8 across the sort and compared
 * against D_8009B314 by the walk in func_800164FC. */
extern u32 D_8009B308;
/* Flag word.  func_80035680 sets bit 2 when it opens the list and
 * func_800355C8 clears it again.  func_800164FC tests bit 1 and clears bits
 * 0 and 1; nothing in the decompiled tree sets either of those two, so what
 * raises the flag it tests is not known here.  func_80035668 writes the word
 * wholesale, and both of its call sites pass 0. */
extern u32 D_8009B30C;
extern SortedEntry *D_8009B310;
extern u32 D_8009B314;

/* func_800220B8 also declares D_8009B30C, as s32 and under
 * __attribute__((section(".data"))).  It is left alone here because folding
 * it in needs that arm and the u32/s32 disagreement settled. */

#endif
