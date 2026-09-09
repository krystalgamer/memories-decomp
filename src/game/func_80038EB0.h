#ifndef MEMORIES_DECOMP_FUNC_80038EB0_H
#define MEMORIES_DECOMP_FUNC_80038EB0_H

#include "../types.h"

/* D_80090EAC entry: the effect-script command. It reads a command id and a flag
 * byte from the object's current stream, finds the display effect record for
 * the id in D_800EB010 -- ids below 0x41 live in the first two records, others
 * in the third -- and depending on the flags stops it (bit 7), adjusts a
 * running one (bits 5 and 6), or starts it in the slot bit 0 names, putting the
 * object into the matching wait state.
 *
 * The parameter is this unit's own view of the object, moved here from the
 * source because a prototype needs its type. The table's entry type is
 * `void (*)(u8 *)`, so the entry casts. */
typedef struct {
    u8 *streams[20];
    u8 unk50;
    u8 state;
    u8 pad52[6];
    s8 depth;
} EffectObject;

void func_80038EB0(EffectObject *object);

#endif
