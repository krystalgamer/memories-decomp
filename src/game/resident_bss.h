#ifndef MEMORIES_DECOMP_GAME_RESIDENT_BSS_H
#define MEMORIES_DECOMP_GAME_RESIDENT_BSS_H

#include "../types.h"
#include "../ygo_types.h"

/* Resident small-data and BSS globals that the generated zero-filled BSS
 * image used to provide through linker assignments. The bss_*.c units in
 * this directory now define them (#2602); these declarations moved here
 * from unmatched.h unchanged, conditional consumer views included. */


/* One consumer, Model_ApplyTextureTint. These four contiguous halfwords are filled
 * as a RECT for StoreImage2, LoadImage2, and MoveImage. The linker assignments
 * establish the scratch storage, but no subsystem header owns it. */
extern s16 gModel_sTextureTintRectX;
extern s16 gModel_sTextureTintRectY;
extern s16 gModel_sTextureTintRectW;
extern s16 gModel_sTextureTintRectH;

/* One consumer, func_8005C374.c, which writes its three parameters to these
 * contiguous bytes. Their individual roles remain unknown, and no movie
 * subsystem header owns the storage. */
extern u8 D_8009B4A0;
extern u8 D_8009B4A1;
extern u8 D_8009B4A2;

/* A single word at 0x8009B118, four bytes: c_symbols.ld names gFile_FilterParams
 * immediately after it, so no element can hide inside. Fifteen files declared
 * it identically as `extern s32 D_8009B118;`, which is why it can move here
 * unchanged - there is no per-consumer spelling to preserve.
 *
 * It holds an address despite the s32 spelling:
 * campaign_map_load_package_stage.c passes it to LoadImage2 as (u32 *) and
 * also adds 0x800 to it. The s32 is left as-is rather than retyped, which is
 * a separate question from centralizing it.
 *
 * Six consumers do NOT take the plain arm, and both reasons are load bearing.
 * Five carry section(".data"), which is the -G8 lever that keeps the address
 * being rebuilt per access instead of resolved gp-relative, and
 * duel_load_package_stage.c additionally spells it u8 * because it does
 * pointer arithmetic on it. Those two arms are selected by
 * D_8009B118_IN_DATA and D_8009B118_IS_POINTER_IN_DATA, the same shape
 * input.h and sound.h use, rather than by re-declaring the symbol locally. */

#ifdef D_8009B118_IS_POINTER_IN_DATA
extern u8 *G32 D_8009B118 __attribute__((section(".data")));
#elif defined(D_8009B118_IN_DATA)
extern s32 D_8009B118 __attribute__((section(".data")));
#else
extern s32 D_8009B118;
#endif

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

/* The text engine's control byte, shared with the dialog and duel-effect
 * screens. Text_HandleChoiceCommand sets it from a command nibble
 * (`c & 0xF0`), text_stream_commands.c reads it whole, and the other two test
 * it by mask -- 0x30 for the layout arm and 0x40 for the choice arm. No C
 * source defines it and the four that use it share no subsystem header, so it
 * is homeless by the rule at the top of this file.
 *
 * Script_OpSavePrompt also clears it, spelled with a .data section
 * attribute because it addresses the byte outside small data. */

#ifdef D_8009B34C_IN_DATA
extern u8 D_8009B34C __attribute__((section(".data")));
#else
extern u8 D_8009B34C;
#endif
extern s32 D_8009B0FC;
extern u8 D_8009B108;
extern u8 D_8009B110;
extern s32 D_8009B12C;
extern u8 gDuel_bSelectedTrapSlot;
extern u8 gMain_bModeRunnerStep;
extern s32 gGameOver_pTitleObject;

/* An interior label, not separate storage: gModel_aSlots[2].model_id, the
 * stage slot's model id (0x800F2C40 + 2 * 0xE20 + 0xDF8). Model_SetSlotProperties
 * reads it through this label to pick gModel_pBackgroundRecord. */
extern u16 gModel_awStageSlotModelId[];
extern s16 gDisplayObject_wSpriteSheetListHead;

/* gFile_anLba[6], the LBA File_SetPositionTable records for gFile_apszName[6]
 * (MASTER.XA). File transfer control adds it to a descriptor offset to start
 * secondary (XA) range transfers. Main_Init passes its first byte to
 * SD_InitState after Main_InitHardware has filled the position table, so
 * that argument is the LBA's low byte; what SD_InitState makes of it is not
 * interpreted here. The raw byte-array view is kept because both consumers
 * match on it. */
extern u8 gFile_nMasterXaLba[];
extern s32 D_8009B0A4;
extern s32 D_8009B0B0;
extern s32 D_8009B0BC;
extern s32 D_8009B0D4;

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
extern u8 gDuelEffect_bRequestFinished;

#ifdef D_8009B264_VISIBLE
extern DuelEffectRequest *G32 gDuelEffect_pActiveRequest;
#endif

#ifdef D_8009B2F8_AS_ARRAY
extern u8 gBuildDeck_bConfirmMode[];
#else
extern u8 gBuildDeck_bConfirmMode;
#endif

#ifdef D_8009B370_AS_BYTE_ARRAY
extern u8 D_8009B370[];
#else
extern u16 D_8009B370;
#endif
extern u16 D_8009B372;

/* DuelScene_UpdateBattle (src/game/duel_scene_battle.c) reads it through a
 * %hi/%lo pair into the load's own register, which is the .data form. */

#ifdef D_8009B374_IN_DATA
extern u16 D_8009B374 __attribute__((section(".data")));
#else
extern u16 D_8009B374;
#endif

#ifdef D_8009B_MODEL_VISIBLE
extern ModelBytes8 gModel_ViewSnapshot;
#endif
extern s8 gDuel_bSide1CardViewMode[];
extern u8 D_800EAE8E[];
extern u8 D_800EF6E0[];
extern s16 D_800F2B22;
extern u8 gMovie_FileInfo[];

#ifdef GCAMPAIGN_SCENE_INDEX_AS_ARRAY
extern u8 gCampaignSceneIndex[];
#elif defined(GCAMPAIGN_SCENE_INDEX_AS_SCALAR)
extern u8 gCampaignSceneIndex;
#else
extern u8 gCampaignSceneIndex __attribute__((section(".data")));
#endif

/* One byte at 0x8009B363, written by four files that share nothing else.
 *
 * frontend_scene_states.c clears it, and duel_effect_sound_commands.c,
 * script_op_open_campaign_map.c and main_run_selection_menus.c each store a value into it;
 * the last also reads it back to pass to func_8016866C. Four subsystems with
 * no header above them, which is what makes this genuinely homeless rather
 * than merely undeclared.
 *
 * The array spelling is a lever, not a size. Every one of the four is
 * compiled and assembled at -G8, where a byte-sized global would otherwise be
 * reached %gp_rel, and an array is what escapes that -- the table in
 * notes/build.md, second row. All four already write the same incomplete-array
 * form, so this declaration reproduces it exactly and no arm is needed.
 *
 * It is only ever indexed at [0], in all four files, so nothing here claims
 * the object is longer than one byte. c_symbols.ld agrees: gDuel_bTerrain
 * begins at 0x8009B364, immediately after it. */
extern u8 D_8009B363[];
#ifdef D_800E9ECE_AS_SCALAR
extern u8 D_800E9ECE;
#else
extern u8 D_800E9ECE[];
#endif
#ifdef D_800E9ECF_AS_SCALAR
extern u8 D_800E9ECF;
#else
extern u8 D_800E9ECF[];
#endif

#endif
