#ifndef MEMORIES_DECOMP_SCRIPT_SCENE_H
#define MEMORIES_DECOMP_SCRIPT_SCENE_H
#include "../types.h"
#include "display_object.h"
#include "../ygo_types.h"

/* Creates and registers the campaign scene's primary screen-space object.
 *
 * It allocates a type 2 object, configures it through DisplayObject_ConfigureSpriteAtPosition with a
 * fixed geometry and depth, sets DISPLAY_OBJECT_FLAG_SCREEN_SPACE in the flag
 * halfword at +0x08, hands it to DisplayObject_SelectOrderingTable1, and returns the pointer.
 *
 * Both callers store the result in the typed gCampaign_PrimaryDisplayObject DisplayObject owner. */
DisplayObject *Campaign_CreatePrimaryDisplayObject(void);

#define CAMPAIGN_DIALOG_PORTRAIT_COUNT 25
#define CAMPAIGN_DIALOG_PORTRAIT_GRID_COLUMN_COUNT 5
#define CAMPAIGN_DIALOG_PORTRAIT_FIRST_EFFECT_ID 0x41
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH 48
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_HEIGHT 48
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WORD_WIDTH \
    (CAMPAIGN_DIALOG_PORTRAIT_IMAGE_WIDTH / 2)
#define CAMPAIGN_DIALOG_PORTRAIT_IMAGE_SIZE 0x900
#define CAMPAIGN_DIALOG_PORTRAIT_CLUT_WORD_WIDTH 0x40
#define CAMPAIGN_DIALOG_PORTRAIT_CLUT_SIZE 0x80
#define CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE \
    (CAMPAIGN_DIALOG_PORTRAIT_IMAGE_SIZE + \
     CAMPAIGN_DIALOG_PORTRAIT_CLUT_SIZE)
#define CAMPAIGN_DIALOG_PORTRAIT_CLUT_ROWS_PER_COLUMN 16
#define CAMPAIGN_DIALOG_PORTRAIT_PHASE_SIZE 0xF000
#define CAMPAIGN_DIALOG_PORTRAIT_USED_SIZE \
    (CAMPAIGN_DIALOG_PORTRAIT_COUNT * \
     CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE)
#define CAMPAIGN_DIALOG_PORTRAIT_SPILL_SIZE \
    (CAMPAIGN_DIALOG_PORTRAIT_PHASE_SIZE - \
     CAMPAIGN_DIALOG_PORTRAIT_USED_SIZE)

typedef char CampaignDialogPortrait_record_size_must_be_0x980[
    CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE == 0x980 ? 1 : -1
];
typedef char CampaignDialogPortrait_spill_size_must_be_0x280[
    CAMPAIGN_DIALOG_PORTRAIT_SPILL_SIZE == 0x280 ? 1 : -1
];

/* Initializes the fixed scene presentation package: seeds the event-script
 * flag word gScript_EventState from its argument, clears the viewport origin, and
 * requests the package transfer.
 *
 * Both callers declared the parameter s32 while the definition takes s16,
 * and nothing compared them: this unit had no header. Neither call is
 * affected -- DebugMenu_UpdateBustUpEntry passes a constant 0, and Main_RunCampaign passes
 * the u8 gCampaignSceneIndex, which widens the same way to either type -- so
 * the header states the definition's own width.
 *
 * DebugMenu_UpdateCampaignEntry also passes a constant zero through this declaration. */
void Campaign_LoadScenePackage(s16 scene_index);
void Campaign_LoadScenePackageStage(
    FileTransferDescriptor *descriptor, s32 stage
);

/* The scene package's primary display object. The loader clears this slot
 * before initialization and later creates it; the debug campaign entry
 * releases/recreates it. DuelResult conditionally hides/restores its renderable
 * flag. Releases do not imply a null store or a live pointer afterward.
 * The debug entry selects the DATA view for its absolute load/store. */
extern DisplayObject *gCampaign_PrimaryDisplayObject;

/* The completion callback Script_OpDuelResult hands to File_RequestAsyncTransfer
 * for the duel result screen's asset sector. It dispatches on its second
 * parameter, so the transfer layer calls it back for more than one stage of
 * the same request.
 *
 * Worth stating why this declaration belongs here rather than at the caller.
 * The callback slot is typed FileTransferCallback, which ygo_types.h defines
 * as `void (*)()` -- unprototyped -- so passing this function there checks
 * nothing about its arguments, and Script_OpDuelResult only ever took its address.
 * Between them, no site in the tree could compare a declaration against the
 * definition. Declaring it in the defining unit's own header, which
 * script_scene.c includes, makes the compiler check it on every build. */
void DuelResult_LoadPackageStage(FileTransferDescriptor *d, s32 mode);

/* The scene script's slot table at gSceneScript_Slots.
 *
 * Campaign_LoadScenePackage initializes the fixed scene presentation package
 * and clears
 * this table by subscripting it; Script_OpLoadImageScene is a script command
 * that runs ScriptImage_ReleaseObjects over it before reading its next operand
 * from the script
 * stream. Main_RunCampaign is one of the consumers.
 *
 * Five records of 0x14 bytes, and the shape is measured rather than assumed.
 * The pre-migration pointer-array declarations wrote
 * indices 0, 5 and 10, which are byte offsets 0, 0x14 and 0x28 -- the first
 * word of records 0, 1 and 2. A third declared it u8[100], and 100 is
 * 5 * 0x14. Our matching tree reads exactly those two words at +0x14 and
 * +0x28 in byte-exact functions.
 *
 * object_address holds a display object: Script_OpDuelResult stores it into
 * records 0, 1 and 2, and SceneScript_UpdateColorPulse reads records 0 and 1 back. It is
 * left s32 here on purpose. ScriptImageEntry has a separate stored-pointer
 * view of the same prefix; naming the table does not merge these access forms.
 *
 * script_scene.c takes this declaration too. Its three stores are
 * the first word of slots 0, 1 and 2, as gSceneScript_Slots[i].object_address.
 * That spelling builds the unit's .text, relocations and data sections identical
 * to the pointer-array view it used to declare privately. (A cast of the
 * table to `Object **` indexed [0], [5] and [10] was the spelling that
 * differed from byte 0x8002F780.) SceneScriptSlot's measured layout is
 * defined once in ygo_types.h.
 */
#define SCENE_SCRIPT_SLOT_SIZE 0x14
#define SCENE_SCRIPT_SLOT_COUNT 5

/* Unsized: a declared size is a -G input for this toolchain, and no consumer
 * needs the bound. The count is above, with what it was derived from.
 *
 * Main_RunCampaign (main_mode_runners.c) defines only three records: that is all the room
 * before gDebug_szEditorCaretLine, and SceneScript_UpdateSlots sweeps three.
 * Package initialization's -1 store at slot three's head (+0x3C) lands in
 * the four bytes left before that next object.
 * The five-record figure above comes from old declarations, and our
 * matching tree's byte read at +0x64 lands inside the caret line; neither
 * is resolved here. */
extern SceneScriptSlot gSceneScript_Slots[];

/* The per-tick sweep over the first three slots. It dispatches each live
 * slot through gSceneScript_RecordCallbacks using the low byte at +4 as the callback index. */
void SceneScript_UpdateSlots(SceneScriptSlot *records);

struct DisplayObject;

/* Entry 1 of the scene-script record callback table gSceneScript_RecordCallbacks
 * (script_scene.c). It advances a sine phase at +6 of
 * `state`, latched by bit 0x8000 of +4 on the first call. From it, it writes
 * a pulsing grey into the colour bytes (rgb) of `color` -- the display
 * object SceneScript_UpdateSlots passes from the slot's own first word -- and a
 * separately computed component into the independently loaded display
 * objects in scene-script slots 0 and 1. These objects may alias `color`. */
void SceneScript_UpdateColorPulse(SceneScriptSlot *state, struct DisplayObject *color);

/* Entry 0 of the scene-script record callback table gSceneScript_RecordCallbacks
 * (script_scene.c). It is empty: a record whose selector
 * byte is 0 has no per-frame work. */
void SceneScript_NoOpRecordCallback(void);

/* One-shot latch on bit 15 of gScript_wCommandWord. Returns 0 the first time it is
 * called after the word is cleared, and 1 on every call after that, so a
 * script command handler runs its body once and then reports busy. Callers
 * uniformly spell the test as ScriptCommand_MarkStarted() == 0. */
s32 ScriptCommand_MarkStarted(void);

void Script_OpHalt(void);

/* gScript_apfnCommandHandlers handler: the conditional script jump on deck state. Always
 * consumes a 16-bit offset, and repoints gScript_StreamCursor into gFile_StagingBuffer only when
 * Duel_IsPlayerDeckComplete reports the deck is NOT full -- so the jump is the
 * deck-incomplete path and falling through is the complete one. */
void Script_OpJumpIfDeckIncomplete(void);

/* gScript_apfnCommandHandlers handler: hands the scene to mode 8, with D_8009B268 set to 1 and
 * gMain_bRequestedMenuID to 5. Consumes nothing from the stream and never clears the busy
 * word. */
void Script_OpReturnToMenu(void);

void Script_RunTick(void);

/* Three gScript_apfnCommandHandlers handlers that drive the scene's own state rather than any
 * object. Script_OpGameOver and Script_OpCredits set the mode pair gMain_bReturnMode and
 * gMain_bActiveModeState to 12 and 15 -- and both write the same value twice, once to each,
 * which is why they are two functions and not one with an argument.
 *
 * Script_OpWait is the wait: it takes a 16-bit count from the stream on the
 * first tick, then counts down once per tick and clears gScript_wCommandWord when the
 * count reaches zero. */
void Script_OpGameOver(void);
void Script_OpCredits(void);
void Script_OpWait(void);

/* Script_OpStoryFlag is the campaign-flag command, and it is both a write and a
 * branch: with CAMPAIGN_FLAG_COMMAND_WRITE set it records a card-used flag and
 * consumes two stream bytes, and without it it consumes four -- the flag id and
 * a 16-bit offset -- and jumps gScript_StreamCursor into gFile_StagingBuffer when
 * Campaign_TestStoryFlag agrees. The stream pointer is written twice on that
 * path, first past two bytes and then past four. The decoded command is an
 * unsigned halfword; the offset is assembled from the next two bytes.
 *
 * Script_OpViewportTween takes three 16-bit operands -- target x, target y and
 * duration -- and leaves gScript_wCommandWord at 7, which is what hands the scene over to
 * Script_UpdateViewportTween for the following ticks. */
void Script_OpStoryFlag(void);
void Script_OpViewportTween(void);
void Script_UpdateViewportTween(void);

#define SCRIPT_SHOW_IMAGE_CONTROL_TRANSFER_PHASE 0x4000
#define SCRIPT_SHOW_IMAGE_CONTROL_REBUILD_PHASE 0x2000

/* The two adjacent image opcodes. Script_OpStageImage leaves the viewport
 * pair gScript_ViewportTargetX/gScript_ViewportTargetY and state 5 in gScript_wCommandWord; Script_OpShowImage
 * publishes that viewport when it draws. */

/* gScript_apfnCommandHandlers handler: the viewport jump. Clears the viewport pair, takes a
 * 16-bit id from the stream, and with bit 15 set takes two more halfwords as an
 * immediate viewport position -- then leaves gScript_wCommandWord at 5, which is the
 * state the following ticks read. */
void Script_OpStageImage(void);

/* gScript_apfnCommandHandlers handler: draws the script image. Builds the 0x140 by 0xA0 VRAM
 * copy object kept in gScript_TransitionObject, releases the previous image slots, and
 * publishes the viewport from gScript_ViewportTargetX/gScript_ViewportTargetY; a later pass tears the
 * object down again and rebuilds the slots through ScriptImage_RebuildObjects. */
void Script_OpShowImage(void);

/* Two gScript_apfnCommandHandlers entries with empty bodies. They are distinct functions at
 * distinct addresses in the target, not one entry repeated, so the table needs
 * both names -- and neither clears gScript_wCommandWord, which means a script command
 * that lands on either never reports itself finished. */
void Script_OpUnusedHalt14(void);
void Script_OpUnusedHalt15(void);

/* gScript_apfnCommandHandlers handler: the duel result screen. Reads the two-byte result code
 * from the stream, queues the result sector with DuelResult_LoadPackageStage as the
 * completion callback, and on the first pass -- once the transfer and fade flags
 * are clear -- builds the banner, the two 0x140-wide panels and the fill object
 * kept in gScript_TransitionObject, then starts the music. */
void Script_OpDuelResult(void);

/* gScript_apfnCommandHandlers handler: starts a fade out. The low six bits of its stream byte,
 * when set, become the fade step in gFade_State.step; bit 7 keeps the command busy
 * until gFade_State.flags bit 7 clears, which is how the script waits for the
 * fade rather than only starting it. */
void Script_OpFadeOut(void);

#define SCRIPT_LOAD_IMAGE_SCENE_CONTROL_FADE_PHASE 0x800

/* gScript_apfnCommandHandlers handler: enters a script image scene. Resets the viewport pair,
 * releases the three image slots through ScriptImage_ReleaseObjects, then
 * takes a 16-bit
 * image id from the stream -- and when bit 15 is set, two further halfwords
 * that seed the viewport, advancing the cursor past all six bytes. */
void Script_OpLoadImageScene(void);

/* gScript_apfnCommandHandlers handler: takes the Campaign Map location from the stream and
 * requests frontend mode 5. */
void Script_OpOpenCampaignMap(void);

#define SCRIPT_SAVE_PROMPT_CONTROL_CARD_DIALOG 0x80
#define SCRIPT_SAVE_PROMPT_CONTROL_DECK_ERROR 0x200
#define SCRIPT_SAVE_PROMPT_CONTROL_EXIT_CONFIRM 0x400
#define SCRIPT_SAVE_PROMPT_CONTROL_SUBPHASE_STARTED 0x800
#define SCRIPT_SAVE_PROMPT_CONTROL_SLIDE_OUT 0x1000
#define SCRIPT_SAVE_PROMPT_CONTROL_SLIDE_IN 0x2000
#define SCRIPT_SAVE_PROMPT_CONTROL_FOLLOWUP_PHASE 0x4000

/* Retained LE16 scene operand across prompt phases. The halfword workspace
 * stores write the scene byte and opaque companion; only the build-deck
 * handoff explicitly narrows it to u8. This does not assert allocation
 * extent or a shared record with the neighboring event flags. */
extern u16 gScript_SavePromptSceneOperand;

/* gScript_apfnCommandHandlers handler: the dialog choice step, and the deck test it shares with
 * Script_OpJumpIfDeckIncomplete.
 *
 * Duel_IsPlayerDeckComplete reports whether the player's deck is full -- 1 when no
 * DECK_SIZE entry of gDuel_awPlayerDeck is zero, 0 as soon as one is -- and
 * Script_OpJumpIfDeckIncomplete branches the script on it. It lives here because this is the
 * unit that defines it; nothing about it is specific to the choice step. */
extern void Script_OpSavePrompt(void);
s32 Duel_IsPlayerDeckComplete(void);

/* gScript_apfnCommandHandlers handler: opens a dialog text box at a fixed 0x10,0xB0 rectangle
 * from the 16-bit string id in the stream, marks it through
 * DuelEffect_MarkObjectIfActive and sets its 0x8 flag. Bit 15 of the id
 * instead raises 0x4000 in gScript_wCommandWord and CLEARS that 0x8 bit, reading the
 * box's flags word through a volatile view so the two writes are not merged.
 * The busy word is then latched into gScript_DialogCommandLatch, which this unit defines. */
void Script_OpShowDialog(void);

/* The latched busy word. script_scene.c owns the definition; Script_RunTick there
 * clears it, tests it, and copies it back into gScript_wCommandWord, and spelled its
 * own `extern u16` for it.
 *
 * The note here used to say the symbol was defined rather than declared "so
 * the store stays gp-relative", and that a header declaration would disturb
 * it. notes/build.md records that this was wrong: a plain declaration in
 * front of the definition does not change it, and the test is agreement
 * rather than definition. The consumer's spelling and the definition are the
 * same `u16`, so the declaration is inert and belongs here. */
extern u16 gScript_DialogCommandLatch;

/* gScript_apfnCommandHandlers handler: opens one of the two gDisplayEffect_aMenuRecords menu records -- bit 7 of
 * the second stream byte picks which -- tears down whatever that record was
 * showing, and stores the first byte as its content id with the low nibble of
 * the second as its mode. The chosen record is published through gScript_pMenuRecord for
 * the handlers that follow. */
void Script_OpShowMenu(void);

/* Three gScript_apfnCommandHandlers command handlers that read their operands out of the script
 * stream at gScript_StreamCursor and leave it advanced past them.
 *
 * Script_OpSound is the widest: bits 0-5 of its first byte pick between
 * replaying, latching or copying the current BGM command, a clear low six bits
 * instead take a 16-bit id from the stream and bit 6 sends that id to
 * SD_SEPlayFull rather than SD_BGMPlay, and bit 7 keeps the command busy for
 * another tick instead of clearing gScript_wCommandWord.
 *
 * Script_OpFadeBgm fades the BGM out, with the low seven bits as the step and
 * zero meaning the default, and carries the same bit 7. Script_OpJump is the script
 * jump: it takes a 16-bit offset and repoints gScript_StreamCursor into gFile_StagingBuffer. */
void Script_OpSound(void);
void Script_OpFadeBgm(void);
void Script_OpJump(void);

#endif
