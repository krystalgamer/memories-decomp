#ifndef MEMORIES_DECOMP_UTIL_COMPARE_S16_H
#define MEMORIES_DECOMP_UTIL_COMPARE_S16_H

#include "../types.h"

/* Three-way comparator over two signed halfwords, returning 0, 1 or -1. Its
 * only consumer is the retained `func_80024734` candidate, which hands it to
 * qsort to order the combined deck's card ids.
 *
 * The prototype is spelled exactly as the definition writes it, const and
 * all. The local declaration it replaces dropped the const and named the
 * parameters s16 * rather than const short * -- the same types, since s16 is
 * short, but a weaker promise than the function actually keeps. Adopting the
 * definition's spelling is free here because the qsort call casts the
 * function to (int (*)()) anyway, which erases the prototype at the point of
 * use; nothing observed depends on the declaration's parameter types. */
int Util_CompareS16(const short *left, const short *right);

#endif
