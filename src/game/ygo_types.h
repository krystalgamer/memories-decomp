#ifndef YGO_TYPES_H
#define YGO_TYPES_H

#include "../types.h"

/* Shared game structures.
 *
 * Types live here so that a translation unit consumes a definition rather than
 * re-deriving one. Each field is named only where its width and offset are
 * established by the accesses in the sources that use it; the remainder is
 * padding so that later offsets stay correct.
 */

/* Display object, 0x47 bytes. Position and extent occupy the first four
 * halfwords, and the two halfwords at +0x30 and +0x32 are read as a pair
 * alongside the byte flag at +0x46. */
typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s32 field08;
    s32 field0C;
    u8 pad10[0xC];
    s32 field1C;
    u8 pad20[0x10];
    s16 field30;
    s16 field32;
    u8 pad34[0x12];
    u8 field46;
} Object;

#endif /* YGO_TYPES_H */
