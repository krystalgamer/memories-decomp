#ifndef MEMORIES_DECOMP_DUEL_CHECK_QUIT_INPUT_H
#define MEMORIES_DECOMP_DUEL_CHECK_QUIT_INPUT_H

#include "../types.h"

/* Checks Select in the two-player duel path and arms the quit dialog. It has
 * no caller in matched C; retail reaches it from assembly. */
s32 Duel_CheckQuitInput(void);

/* The state that function arms, declared here because arming it is the whole
 * of what that function does. Two bits are established:
 *
 *   0x01  a quit was requested -- Duel_CheckQuitInput stores 1 when Select is
 *         pressed and gDuel_bOpponentID[0] is negative.
 *   0x80  the dialog is already up. DuelScene_Update
 *         (src/candidates/DuelScene_Update.c) is the consumer: on a nonzero
 *         value with 0x80 clear it sets that bit, plays SE 0x30 and raises
 *         the box through TextBox_CreateFlagged;
 *         with 0x80 set it waits for the box, destroys it, and clears the
 *         byte back to 0.
 *
 * func_800179F4 clears it when a duel starts, in the same run of assignments
 * that resets the rest of the duel state.
 *
 * These three are the only accessors in the image, assembly included.
 * Duel_CheckQuitInput is matching C; the other two are candidates under
 * src/candidates/. */
extern u8 gDuel_bQuitDialogState;

/* The duel outcome flags the quit flow reports into, as the two small-data
 * users spell it.
 *
 * DuelScene_Update raises 0x2000 here at the point it clears
 * gDuel_bQuitDialogState above -- that is the quit result -- and Main_RunDuel
 * reads exactly that bit back. func_800179F4 clears the word when the duel
 * starts, in the same run of assignments that clears the quit state, and
 * raises 0x1000 there for its own reason.
 *
 * Only those two units take this plain u16 spelling, and the declaration lives
 * here rather than in duel_side_state.h for a checked reason:
 * Main_RunDuel (src/candidates/func_8002CEE8.c) reads the word as
 * `u16 D_8009B16C[9]`, which is
 * the oversized-array form of the absolute addressing the .data attribute also
 * produces, and that file does include duel_side_state.h. It does not include
 * this header, and neither does debug_effect_screen.c, which takes a third
 * view -- `u8 D_8009B16C[4]`, touching byte 2 rather than the halfword at 0.
 * So the spellings never meet here and no guarded arm is needed.
 *
 * The byte-2 use is worth knowing about before anyone widens this: the address
 * carries more than the flags word these two functions see. */
extern u16 D_8009B16C;

#endif
