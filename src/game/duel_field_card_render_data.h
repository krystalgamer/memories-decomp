#ifndef MEMORIES_DECOMP_DUEL_FIELD_CARD_RENDER_DATA_H
#define MEMORIES_DECOMP_DUEL_FIELD_CARD_RENDER_DATA_H

#include "../types.h"

/* Duel small data that duel_field_card_render.c defines (#2602), for the
 * units that reach it without the render prototypes. */
/* Active duel display-transition request/status flags. Every consumer uses
 * this same scalar view; it is not the separate view-tween frame counter.
 *
 * gDuel_wDisplayTransitionFlags is pinned by its neighbour: c_symbols.ld names D_8009B164
 * two bytes later, so it has no room for an element to carry its own name.
 * gDuel_wSceneStateFlags has moved to its owner, duel_scene_state.h. */
extern u16 gDuel_wDisplayTransitionFlags;   /* nine declarers  */
extern u16 D_8009B23A;   /* candidate lexical alias for gDuel_wSceneStateFlags */
/* Three more undefined globals, moved together for the same reason as the
 * batch above: this header is a serialisation point, so separate changes for
 * separate symbols conflict with each other without making any of them
 * easier to review.
 *
 * All three are one-byte state bytes that every consumer spells `extern u8`
 * and reads or writes whole. None is ever indexed, so there is no array
 * hiding behind the scalar and no per-consumer addressing form to preserve.
 *
 * gMemCardDialog_Port is the strongest of the three structurally: c_symbols.ld names
 * gMemCard_wDialogFlags one byte later, so the object is exactly one byte and
 * nothing can carry a second name inside it. It is the memory card slot the
 * MemCard* calls are issued against.
 *
 * gMemCardDialog_OperationState and gDuel_bSceneSubStep have larger gaps to the next name -- two bytes
 * and eight -- but those are upper bounds rather than sizes, like the gap
 * after gDuel_wSceneStateFlags. Nothing is named inside either gap, and no
 * consumer of either reads past the byte, so the u8 all five consumers agree
 * on is what is declared and the bytes above stay unclaimed.
 *
 * Both of the latter two are packed state bytes rather than plain counters,
 * which is why the byte width matters to every reader: gMemCardDialog_OperationState is switched
 * on through `& 0xF` with MEM_CARD_DIALOG_FLAG_* bits set above it, and
 * gDuel_bSceneSubStep carries a step in its low nibble with 0x20, 0x40 and 0x80 used
 * as independent flags. The modal port and internal-state declarations live in
 * game/mem_card.h; the width/addressing evidence above remains historical. */
extern u8 gDuel_bSceneSubStep;   /* five declarers */
/* Five more undefined scalars, batched for the reason the two batches above
 * give: this header serialises, so one change per symbol buys nothing and
 * conflicts with every other one in flight.
 *
 * None of the five is ever indexed anywhere in the tree, so each is a plain
 * scalar rather than an array wearing a scalar's spelling, and every consumer
 * already writes the same declaration. Neither the width nor the addressing
 * form is being changed here; the spellings below are copied from the
 * consumers that had them.
 *
 * Three are pinned exactly, with the next name sitting at precisely the end
 * of the declared width, so no element can hide inside them: gMemCardDialog_TransferSize and
 * gMemCardDialog_TransferOffset are two bytes each with a name two bytes on, and gDuel_wStepCounter is
 * two bytes with gDuel_wEffectCardID immediately after it.
 *
 * gMemCardDialog_SyncResult has a larger gap than its width and is treated as an upper
 * bound rather than a size. Nothing is named
 * inside that gap and no consumer reads past the declared width, so the
 * agreed type is what is declared and the bytes above stay unclaimed.
 *
 * The first four are memory card state, shared by the create, load, save and
 * dialog paths together with mem_card_dialog.c. gDuel_wStepCounter is
 * unrelated to them and belongs to the duel side; it is here because it
 * passed the same checks, not because it is part of that group. The modal
 * transfer halfwords and sync result live in game/mem_card.h with the same types. */
extern u16 gDuel_wStepCounter;   /* four declarers */
extern u8 gDuel_bSelectedTrapSlot;
/* Duel_UpdateCardUseConfirmation's step flags: bit 7 once it has created the
 * two flip markers, bit 6 while a left/right flip of gDuel_pActionCard runs.
 * DuelScene_UpdateFieldActions clears the byte before the step starts. */
extern u8 gDuel_bConfirmFlags;
#ifdef D_8009B_DISPLAY_OBJECTS_VISIBLE
/* The two sprites placed beside the card being flipped, 8 pixels left of it
 * and 0x3C right of it, 0x1E down. The confirmation step and the hand step
 * create them; both release them once the choice is made. */
extern DisplayObject *G32 gDuel_pFlipLeftMarker;
extern DisplayObject *G32 gDuel_pFlipRightMarker;
#endif
#ifdef D_8009B_DISPLAY_OBJECTS_VISIBLE
/* The field-action card: DuelScene_UpdateFieldActions moves it to (0x86, 0x5A)
 * and Duel_UpdateCardUseConfirmation flips it face up or down. */
extern DisplayObject *G32 gDuel_pActionCard;
extern DisplayObject *G32 gDuel_pActionReadyMarker;
#endif

extern s8 gDuel_bSide1CardViewMode[];

#endif
