#ifndef MEMORIES_DECOMP_FUNC_8005A53C_H
#define MEMORIES_DECOMP_FUNC_8005A53C_H

#include "../types.h"

/* Walks a run of variable-length handler entries and rewrites the key of each
 * one the callback accepts.
 *
 * Every entry starts with its key, and the halfword at +4 gives the entry's
 * length in words, so the run is stepped through rather than indexed. For an
 * entry that qualifies, the callback is given the old key and its result
 * replaces the key unless it is -1.
 *
 * The callback takes and returns the key itself, not an index or an entry
 * pointer, which is what a bare `extern void func_8005A53C();` at the one
 * call site could not say.
 *
 * arg2 is declared because the caller passes a third value, but the body
 * never reads it. It is kept in the signature rather than dropped, since the
 * call site's argument setup is what the build depends on. */
void func_8005A53C(s32 (*fn)(s32), u8 *entry, s32 arg2, s32 count);

#endif
