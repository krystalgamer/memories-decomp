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
 *         pressed and gDuel_bOpponentID is negative.
 *   0x80  the dialog is already up. DuelScene_Update
 *         (src/game/duel_scene.c) is the consumer: on a nonzero
 *         value with 0x80 clear it sets that bit, plays SE 0x30 and raises
 *         the box through TextBox_CreateFlagged;
 *         with 0x80 set it waits for the box, destroys it, and clears the
 *         byte back to 0.
 *
 * Duel_InitScene clears it when a duel starts, in the same run of assignments
 * that resets the rest of the duel state.
 *
 * These three are the only accessors in the image, assembly included.
 * Duel_CheckQuitInput is matching C; the other two were candidates. */
extern u8 gDuel_bQuitDialogState;

/* The duel outcome flags the quit flow reports into, as the two small-data
 * users spell it.
 *
 * DuelScene_Update raises 0x2000 here at the point it clears
 * gDuel_bQuitDialogState above -- that is the quit result -- and Main_RunDuel
 * reads exactly that bit back. Duel_InitScene clears the word when the duel
 * starts, in the same run of assignments that clears the quit state, and
 * raises 0x1000 there for its own reason.
 *
 * The other two users take the plain scalar. */
extern u16 gDuel_wOutcomeFlags;

/* duel_scene.c's placement counter: DebugEffect_UpdatePreview zeroes
   it on first entry and steps it modulo 8 on each placement; nothing else reads
   it. It follows gDuel_wOutcomeFlags in the US and Japanese images, but the
   European ones place it elsewhere, so it is its own variable rather than a
   byte of the flags word. */
extern u8 gDebugEffect_bPlacementCounter;

#endif
