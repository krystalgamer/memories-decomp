#ifndef MEMORIES_DECOMP_FUNC_80035598_H
#define MEMORIES_DECOMP_FUNC_80035598_H

#include "../types.h"

/* Three-way comparator over the first word of two sorted-entry records,
 * returning 0, 1 or -1. sorted_entry_relink.c is the only consumer and hands
 * it to qsort over eight-byte entries.
 *
 * The prototype is the definition's own, const included. The local
 * declaration it replaces dropped the const, and nothing could catch that:
 * the function is only ever address-taken, and qsort is reached with no
 * declaration in scope at all, so neither end checked the signature.
 *
 * This header deliberately includes nothing but types.h. sorted_entry_relink.c
 * notes that the qsort call is unprototyped and that giving it a prototype
 * changes argument setup, so nothing here may pull a qsort declaration into
 * that file. */
int func_80035598(const u32 *left, const u32 *right);

#endif
