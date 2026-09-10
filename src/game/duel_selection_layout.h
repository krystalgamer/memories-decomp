#ifndef MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H
#define MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H

#include "../types.h"

#define DUEL_SELECTION_RECORD_SIZE 0x1C
#define DUEL_SELECTION_RECORDS_PER_SIDE 4
#define DUEL_SELECTION_SIDE_SIZE \
    (DUEL_SELECTION_RECORDS_PER_SIDE * DUEL_SELECTION_RECORD_SIZE)

/* The selection table itself: DUEL_SELECTION_SIDE_SIZE bytes per side. */
extern u8 D_800E9F10[];

/* A second name for the inside of that same table, 0x38 bytes in --
 * exactly 2 * DUEL_SELECTION_RECORD_SIZE, so it is record 2 of side 0, and
 * because it is indexed with the same per-side stride it is record 2 of
 * whichever side is selected:
 *
 *     D_8009B1B4 = D_800E9F48 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE;
 *
 * which is the same cursor assignment the D_800E9F10 callers make, just from
 * two records further in. duel_field_display_objects.c passes the same
 * expression to func_80023090.
 *
 * The overlap is retail's, not an artefact of the port, and the two names
 * have to stay separate: func_8001BD88 and func_8001D670 each take the
 * address of both symbols, and each carries its own
 * `addiu $v1, $v1, %lo(D_800E9F48)` relocation. Folding this into an offset
 * from D_800E9F10 would change which symbol those relocations name. */
extern u8 D_800E9F48[];

/* Resets that table. For both sides it walks the four records, zeroing the
 * first three words and the byte at 0x18, then writing 1 to 0x13, the record
 * index to 0x17, and to 0x14 the index again except for record 3, which gets
 * 1. Afterwards it clears 0x13 on the first record of each side, so the value
 * written in the loop survives only for records 1 to 3.
 *
 * Declared here because this header owns everything it touches: D_800E9F10
 * above, and the three size constants the walk is written in terms of.
 * func_800179F4.c is the only caller and held the only declaration. */
void func_80017708(void);

#endif
