#ifndef MEMORIES_DECOMP_SCRIPT_SCENE_H
#define MEMORIES_DECOMP_SCRIPT_SCENE_H
#include "../types.h"
#include "display_object.h"
#include "../ygo_types.h"
#include "file_staging_buffer.h"
#include "menu_record.h"

typedef struct {
    ScriptImageEntry entries[3];
    s16 image_id;
} ScriptImageObjectSet;

typedef char ScriptImageObjectSet_image_id_offset_must_be_0x3C[
    OFFSETOF(ScriptImageObjectSet, image_id) == 0x3C ? 1 : -1
];

/* Three of the scene script's image slots at gSceneScript_Slots, as
 * ScriptImage_ReleaseObjects
 * releases them: a display-object pointer and callback-control halfword.
 * This is not the whole slot table -- the same function writes the mode
 * halfword in ScriptImageObjectSet after these three entries -- and the
 * callers hold their own wider views under their own names. The shared
 * ScriptImageEntry layout is defined in ygo_types.h. */
/* Transfer callback for one script image. The loader supplies its
 * FileTransferDescriptor and advances the callback stage through the setup
 * phases. */
void ScriptImage_TransferCallback(FileTransferDescriptor *transfer, s32 mode);

/* Starts the async read of one script image. `value` is packed BCD: the low
 * two nibbles are the index, and the third selects one of three tables with
 * its own sector stride (0x21, 0x51 or 0x71) -- any other mode returns without
 * requesting anything. `owner` may be null; when it is not, the id is recorded
 * in image_id first. Localized builds remap ids 0x10 and 0x11 before recording
 * the id and use an ordinary store; other builds preserve the volatile store. */
void ScriptImage_RequestTransfer(
    volatile ScriptImageObjectSet *owner, s32 value);

/* Releases the three image slots, setting image_id to -1 first.
 * Each pointer goes to DisplayObject_ReleaseIfPresent before the pointer
 * and callback-control halfword are zeroed. */
void ScriptImage_ReleaseObjects(ScriptImageObjectSet *set);

/* Builds one display object for a script image and stores it at `owner`, a
 * ScriptImageEntry, with `size` the object's request size and `mode`
 * selecting the blend arms: mode 2 takes GsALON | GsAONE and sets the entry's
 * `callback_control` halfword to 1; anything else takes 0x01000000 and clears
 * it. It also sets object_built. Its only caller is
 * ScriptImage_RebuildObjects. */
void ScriptImage_CreateObject(ScriptImageEntry *entry, s32 size, s32 mode);

#define SCRIPT_IMAGE_OVERLAY_FIRST_PRESENT 1
#define SCRIPT_IMAGE_OVERLAY_SECOND_PRESENT 2
#define SCRIPT_IMAGE_OVERLAY_SECOND_SCREEN_SPACE 0x80

/* The two overlay tables ScriptImage_RebuildObjects reads when it rebuilds a
 * scene-script image record. */
/* Ids 0x100-0x1FF: which overlays to create, and the callback-index byte
 * copied into the second overlay slot's control halfword at set +0x2C. */
typedef struct {
    u8 mask;
    u8 callback_index;
} ScriptImageOverlayPair;

/* Ids from 0x200: the same mask (bit 7 also puts the second overlay in
 * screen space), an x and y for each overlay, then the same callback index. */
typedef struct {
    u8 mask;
    u8 x1;
    u8 y1;
    u8 x2;
    u8 y2;
    u8 callback_index;
} ScriptImageOverlayPlacement;

/* Thirteen placements fill 78 of the table's 80 bytes; the last two are
 * retail padding before the script command table that follows. */
typedef struct {
    ScriptImageOverlayPlacement rows[13];
    u8 pad_4E[2];
} ScriptImageOverlayPlacementTable;

extern ScriptImageOverlayPair gScriptImage_OverlayPairs[];
extern ScriptImageOverlayPlacementTable gScriptImage_OverlayPlacements;

/* Rebuilds the display objects for one scene-script image record. `value` is
 * the packed image id, or negative to reuse the one already stored at the
 * record's +0x3C; either way that halfword ends up holding what was built.
 *
 * Ids from 0x200 up are the gScriptImage_OverlayPlacements table and get a full-screen object plus
 * up to two overlays positioned from the table row; below that the object
 * comes from ScriptImage_CreateObject and, from 0x100 up, gScriptImage_OverlayPairs's flag
 * byte decides
 * which of the two overlays exist. Under 0x100 the record's two overlay words
 * are just cleared.
 *
 * Both callers hold a wider scene-script allocation and pass its
 * ScriptImageObjectSet prefix. */
void ScriptImage_RebuildObjects(ScriptImageObjectSet *record, s32 image_id);

struct DuelEffectChannel;

/* Sets DUEL_EFFECT_MARK_OBJECT_FLAG in `channel`'s flags if any
 * gDisplayEffect_aMenuRecords record is still active -- it walks that table
 * backward and stops at the first non-negative effect_id marker -- and does
 * nothing when every marker is negative.
 *
 * The parameter is the text-box channel its callers have just made: every
 * caller passes the result of TextBox_Create or TextBox_CreateFlagged, and
 * Script_OpShowDialog ORs its own flag into the same flags right after.
 * It used to be typed as a MenuRecord, whose 0x34 is the record's x
 * position, not a flag word. */
void DuelEffect_MarkObjectIfActive(struct DuelEffectChannel *channel);

/* The script engine's current command word.
 *
 * script_scene.c fetches it straight from the script stream
 * (gScript_wCommandWord = *gScript_StreamCursor++) and dispatches on its low five bits
 * (gScript_apfnCommandHandlers[gScript_wCommandWord & 0x1F]()), so the low bits carry the opcode and
 * the upper bits carry engine flags: bit 15 is the one-shot busy latch
 * documented in script_command_busy.h, and handlers set and clear 0x4000,
 * 0x2000, 0x1000, 0x800, 0x400, 0x200 and 0x80 around it.
 * Text_StartViewportTween's signed store is a cast at the use. */
extern u16 gScript_wCommandWord;

/* The script instruction cursor: a byte pointer walking the script stream.
 *
 * script_scene.c is the clearest statement of the encoding:
 * Script_ReadByte returns *gScript_StreamCursor++ for a single-byte operand;
 * Script_ReadU16LE returns current[0] | (current[1] << 8) after advancing two.
 * Operands are little-endian and the cursor is byte-granular. Command handlers
 * advance it by two, four or six according to how many operands they take, and
 * script_scene.c fetches the next opcode through it into gScript_wCommandWord.
 */
extern u8 *gScript_StreamCursor;

/* The loaded script package, as the script engine reads it: a byte base the
 * cursor above is set from.
 *
 * script_scene.c states the layout in one line --
 * `gScript_StreamCursor = gFile_StagingBuffer.bytes + gFile_StagingBuffer.script_offsets[g]` -- so the head of
 * the package is a table of little-endian u16 byte offsets indexed by script
 * number, and the entry is where that script's code starts. The other three
 * namers jump within the same package: Script_OpStoryFlag,
 * Script_OpJump and Script_OpJumpIfDeckIncomplete all set the cursor to
 * `gFile_StagingBuffer.bytes + offset`. file_staging_buffer.h describes
 * the other subsystems' views of the same buffer. */

/* The viewport tween target, X then Y, as two consecutive halfwords.
 *
 * Script_OpStageImage and Script_OpViewportTween read them out of the script
 * stream as little-endian operands (cursor[0] | (cursor[1] << 8));
 * Script_OpShowImage copies them straight into gGraphics_sViewportX/Y,
 * and Script_UpdateViewportTween interpolates the viewport towards them,
 * reading them signed for the delta (`*(s16 *)&gScript_ViewportTargetX`, retail lh)
 * and unsigned for the final copy (retail lhu). Retail reaches them
 * gp-relative in all four of those units. */
/* Shared retained halfword: viewport commands use it as their divisor/countdown;
 * Script_OpDuelResult also stages its two-byte operand here. It is not
 * universally a duration. Text_StartViewportTween uses TextStream_ReadU16LE.
 *
 * Consumers read it unsigned except where a site takes it as
 * `*(s16 *)&gScript_SharedOperand` to read the signed divisor; that cast
 * stays at the use, so the declaration is the plain u16. */
extern u16 gScript_SharedOperand;
extern u16 gScript_ViewportTargetX;
extern s16 gScript_ViewportTargetY;

/* Script_UpdateViewportTween's 16.16 X/Y accumulators and per-invocation
 * deltas. The intervening command latch and stream pointer are independent
 * symbols, not padding in a fabricated contiguous tween object. */
extern s32 gScript_ViewportAccumulatorX;
extern s32 gScript_ViewportAccumulatorY;
extern s32 gScript_ViewportDeltaX;
extern s32 gScript_ViewportDeltaY;

/* Signed halfword countdown, decremented even on the initialization call.
 * Keep narrowing/wrap and the existing signed completion test. */
extern s16 gScript_WaitCounter;

/* Script_OpShowImage and Script_OpDuelResult retain an allocated display object
 * here until their later command phase releases it with DisplayObject_ReleaseIfPresent.
 * The stored pointer is not nulled by those releases. */
struct DisplayObject;
extern struct DisplayObject *gScript_TransitionObject;

#define SCRIPT_IMAGE_OPERAND_ID_MASK 0xFFF
#define SCRIPT_IMAGE_OPERAND_SKIP_FADE_IN 0x4000
#define SCRIPT_IMAGE_OPERAND_HAS_VIEWPORT 0x8000

/* The show-image command's halfword operand. Script_OpLoadImageScene and
 * Script_OpStageImage
 * read it from the stream as `cursor[0] | (cursor[1] << 8)`: the low twelve
 * bits are the image index, but the complete word and high flags remain.
 * 0x8000 says a viewport X/Y pair follows (Script_OpLoadImageScene masks
 * only on that optional-coordinate branch and
 * reads the pair into gGraphics_sViewportX/Y; Script_OpStageImage into
 * gScript_ViewportTargetX/Y), and Script_OpShowImage tests 0x4000.
 * Retail is sh/lhu gp-relative at all three, so the plain u16 they already wrote.
 *
 * The text callbacks store TextStream_ReadU16LE's result through $at. Their optional viewport
 * form consumes one byte then one halfword, not two halfwords. */
extern u16 gScript_ImageOperand;

/* Two halfwords the event driver keeps at 0x8009B2A4 and 0x8009B2A6.
 *
 * gScript_EventState is the event selector/state word: Campaign_LoadScenePackage seeds it from
 * its argument, Script_RunTick clears DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE
 * and ORs in DUEL_EVENT_SCRIPT_FLAG_STARTED (duel_effect.h), and
 * Script_OpShowDialog sets the dialog bit. Before the started bit is set,
 * the original whole word indexes the package table; it is not newly masked.
 * The save-prompt context at
 * gScript_SavePromptSceneOperand is declared by script_op_save_prompt.h, its actual consumer.
 * Retail reaches both gp-relative; historical listings spell the latter
 * %gp_rel(gScript_EventState + 0x2) in the listings (Script_OpSavePrompt.s:27, 324,
 * 350, 361) because nothing references its address directly, so its
 * original address-qualified name recorded a role, not a new allocation.
 * Whether it is its own object or the flag word's second element is not
 * established; the two names record the two roles. */
extern u16 gScript_EventState;

/* Keep unsized: the normal runner masks to five bits, while the text bridge
 * uses the raw current-command low byte. The NA initializer has 23 entries. */
extern ScriptCommandHandler gScript_apfnCommandHandlers[];

extern SceneScriptRecordCallback gSceneScript_RecordCallbacks[];

/* Script_OpShowMenu selects element 0 or 1 from gDisplayEffect_aMenuRecords using operand bit
 * 7, arms it, then polls its display_effect_step on subsequent ticks. */
extern MenuRecord *gScript_pMenuRecord;

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

/* One portrait record in the shared work arena: the 48x48 image
 * followed by its 64x1 CLUT row, each uploaded with LoadImage2. */
typedef struct {
    u32 image[CAMPAIGN_DIALOG_PORTRAIT_IMAGE_SIZE / sizeof(u32)];
    u32 clut[CAMPAIGN_DIALOG_PORTRAIT_CLUT_SIZE / sizeof(u32)];
} CampaignDialogPortrait;

typedef char CampaignDialogPortrait_record_size_must_be_0x980[
    sizeof(CampaignDialogPortrait) == CAMPAIGN_DIALOG_PORTRAIT_RECORD_SIZE &&
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
 * and clears this table; Script_OpLoadImageScene is a script command that
 * runs ScriptImage_ReleaseObjects over it before reading its next operand
 * from the script stream. Main_RunCampaign is one of the consumers.
 *
 * Three 0x14-byte records followed by the image-id halfword at +0x3C.
 * Main_RunCampaign (main_mode_runners.c) defines the table in the 0x40 bytes
 * before gDebug_szEditorCaretLine, SceneScript_UpdateSlots sweeps the three
 * records, and Campaign_LoadScenePackage and ScriptImage_ReleaseObjects
 * store -1 to the halfword at +0x3C. (Older declarations sized the table
 * as five records, and a matching byte read at +0x64 lands inside the caret
 * line; neither is resolved here.)
 *
 * The scene-script code reaches the records as SceneScriptSlot (object is
 * the display object Script_OpDuelResult stores into records 0, 1 and 2 and
 * SceneScript_UpdateColorPulse reads records 0 and 1 back). The
 * script-image functions take the same bytes as a ScriptImageObjectSet: three
 * ScriptImageEntry records and image_id. The table is a union of the two
 * layouts, and each caller passes the member its callee declares. */
typedef union {
    SceneScriptSlot slots[3];
    ScriptImageObjectSet images;
} SceneScriptSlotTable;

typedef char SceneScriptSlotTable_size_must_be_0x40[
    sizeof(SceneScriptSlotTable) == 0x40 ? 1 : -1
];

extern SceneScriptSlotTable gSceneScript_Slots;

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
void SceneScript_NoOpRecordCallback(SceneScriptSlot *state, struct DisplayObject *object);

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
