#ifndef MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H
#define MEMORIES_DECOMP_DUEL_SELECTION_LAYOUT_H

#include "../types.h"
#include "duel_card_pick_cursor.h"
#include "duel_hand.h"

#define DUEL_SELECTION_RECORD_SIZE 0x1C
#define DUEL_SELECTION_RECORDS_PER_SIDE 4
#define DUEL_SELECTION_SIDE_SIZE \
    (DUEL_SELECTION_RECORDS_PER_SIDE * DUEL_SELECTION_RECORD_SIZE)

/* Prefix view of one side's selection records. The hand pointer at +8 is
 * written by both duel phase entries; the remaining side record stays byte
 * addressed because its wider layout is not established. */
typedef struct {
    u8 pad_00[8];
    DuelHandSlot *hand;
} DuelSelectionSideView;

typedef char DuelSelectionSideView_hand_offset_must_be_8[
    (u32)&(((DuelSelectionSideView *)0)->hand) == 8 ? 1 : -1
];
typedef char DuelSelectionSideView_size_must_be_0xC[
    sizeof(DuelSelectionSideView) == 0xC ? 1 : -1
];

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
 * two records further in. func_80023144 (src/candidates/func_80023144.c)
 * passes the same expression to func_80023090.
 *
 * The overlap is retail's, not an artefact of the port, and the two names
 * have to stay separate: DuelScene_UpdateHandActions and
 * DuelScene_UpdateFieldActions each take the
 * address of both symbols, and each carries its own
 * `addiu $v1, $v1, %lo(D_800E9F48)` relocation. Folding this into an offset
 * from D_800E9F10 would change which symbol those relocations name. */
extern u8 D_800E9F48[];

/* Assigned from both bases above. Assigned by three C functions and loaded
 * by two: DuelScene_UpdateStartup assigns `D_800E9F10 + D_8009B1D5 *
 * DUEL_SELECTION_SIDE_SIZE` and stores 0xAE at +0xC (duel_phase_entry.c:
 * 69-70); DuelScene_UpdateDrawPhase assigns the same from `side` and stores `base` at
 * +8 (:154-156), then assigns it again and stores 0xAE at +0xC (:206-207);
 * func_8001B938 assigns `D_800E9F48 + D_8009B1D5 * DUEL_SELECTION_SIDE_SIZE`
 * and stores a halfword at +0xC and bytes at +0x11, +0x12, +0x13, +0x18 and
 * +0x19 (func_8001B938.c:37-42), +0x11 and +0x12 again (:53-54), +0x10
 * (:59) and +0xF (:68), loading it back for each; func_80017034 loads it
 * directly as a DuelCardPickCursor (src/game/func_80017034.c). Six
 * functions still in assembly, none with a profile in matching_c.json, also
 * store or load it: DuelScene_UpdateExodiaResult.s:31 and :87, DuelScene_UpdateCardPlacement.s:32,
 * DuelScene_UpdateHandActions (stores :35, :106, :720, :1140, :1369, :1394;
 * loads :92, :501, :507, :1150, :1227), DuelScene_UpdateFieldActions
 * (stores :56, :613, :753, :1409,
 * :1439; load :1412), DuelScene_UpdateBattle.s (stores :93, :183; load :96) and
 * func_800235C0.s:7.
 *
 * DuelCardPickCursor * because that view does cover every store. This
 * note used to say the byte view was the only one that fits, on the
 * grounds that DuelFieldCursor is 0x11 bytes while func_8001B938 writes
 * +0x19. That is true of DuelFieldCursor and false of the record as a
 * whole: DuelCardPickCursor is 0x1A bytes and ends at 0x19 exactly, and
 * duel_card_pick_cursor.h already recorded that its col, row and status
 * agree with the narrower views. func_80017034 now uses this owning type
 * without a cast.
 *
 * The store at +8 stays a cast: it writes a DuelHandSlot *, and giving
 * the record a pointer member there would raise its alignment to 4 and
 * its size to 0x1C, which the asserted 0x1A forbids. Every access in the binary is a %gp_rel lw or sw
 * (func_80017034.s:6, DuelScene_UpdateStartup.s:94, DuelScene_UpdateDrawPhase.s:38 and :164,
 * func_8001B938.s:12 and its nine loads), so this is the plain declaration.
 * Four bytes at 0x8009B1B4, with D_8009B1B8 at +4 (c_symbols.ld:133-134).
 * Initial value not read. */
extern DuelCardPickCursor *D_8009B1B4;

/* Resets that table. For both sides it walks the four records, zeroing the
 * first three words and the byte at 0x18, then writing 1 to 0x13, the record
 * index to 0x17, and to 0x14 the index again except for record 3, which gets
 * 1. Afterwards it clears 0x13 on the first record of each side, so the value
 * written in the loop survives only for records 1 to 3.
 *
 * Declared here because this header owns everything it touches: D_800E9F10
 * above, and the three size constants the walk is written in terms of.
 * func_800179F4 (src/candidates/func_800179F4.c) is the only caller, and
 * its old func_800179F4.c held the only declaration. */
void func_80017708(void);

#endif
