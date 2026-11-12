#ifndef MEMORIES_DECOMP_MEM_CARD_H
#define MEMORIES_DECOMP_MEM_CARD_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "menu_record.h"
#include "duel_effect_states.h"
#include "../ygo_types.h"
#include "card_constants.h"
#include "campaign_flags.h"

#define MEM_CARD_DIRECTORY_ENTRY_SIZE 40
#define MEM_CARD_BLOCK_SIZE 8192
#define MEM_CARD_BLOCK_COUNT 15
#define MEM_CARD_DIRECTORY_RETRIES 5

#define MEM_CARD_DIALOG_FLAG_RESULT_CREATED 0x40
#define MEM_CARD_DIALOG_FLAG_RESULT_READY 0x80
#define MEM_CARD_DIALOG_FLAG_CLOSING 0x800
#define MEM_CARD_DIALOG_FLAG_IO_PENDING 0x1000
#define MEM_CARD_DIALOG_FLAG_STARTED 0x2000
#define MEM_CARD_DIALOG_FLAG_OPENED 0x4000
#define MEM_CARD_DIALOG_FLAG_ACTIVE 0x8000

/* Low-level request producer ABI: file names and buffers are integer
 * addresses, while offset/size are full words at the call boundary.
 * These are distinct from the LIBMCRD MemCardReadFile/WriteFile API. */
s32 MemCard_ReqLoadDirectory(s32 channel);
s32 MemCard_ReqReadFile(s32 channel, s32 name, s32 buffer, s32 offset, s32 size);
s32 MemCard_ReqWriteFile(s32 channel, s32 name, s32 buffer, s32 offset, s32 size);
s32 MemCard_ReqReadSector(s32 channel, s32 buffer, s32 sector);
s32 MemCard_ReqWriteSector(s32 channel, s32 buffer, s32 sector);
s32 MemCard_ReqCreateFile(s32 channel, s32 name, s32 blocks);
s32 MemCard_ProcessRequest(s32 mode, s32 *request, s32 *result);

/* The result of the card's asynchronous IO, set from the callbacks
 * mem_card_io_result_callbacks.h declares and polled by the request state
 * machines. MemCard_BeginRequest resets it to -1 before starting a request.
 *
 * Three of the five consumers declare it volatile, and they are the ones
 * that read it several times in a row while waiting -- without the
 * qualifier gcc commons those reads into one register and the poll cannot
 * observe the callback. The other two write it once or read it once. */
/* The memory-card I/O event handles opened at init and closed on teardown.
   Five sources declared it identically, and all five spell the element type
   `long` rather than s32; that spelling is preserved rather than normalised,
   because DeliverEvent/OpenEvent hand these back as long and nothing here has
   measured the difference. */
extern long gMemCard_aIOEventHandles[];

extern volatile s32 gMemCard_nIOResult;

/* The memory-card channel a request was issued on. MemCard_BeginRequest
 * stores its first parameter here before it resets gMemCard_nIOResult;
 * every caller passes it the int it was itself handed, and
 * MemCard_ReqLoadDirectory passes that same int to _card_info(long chan).
 * The readers hand it back to _card_info, _card_clear and _card_load, and to
 * MemCard_FindFiles's s32 first parameter. Retail stores it with sb and
 * reads it with lbu at ten sites, all gp-relative into $a0, five of the reads
 * in MemCard_ProcessRequest; so it is one unsigned byte, and the one
 * char spelling was the writer's, where a store shows no sign. */
extern u8 gMemCard_bChannel;

/* Japan's card save file name and MemCardWork_UpdateOperation diagnostics,
 * linker symbols there (see japanese/mem_card.c); the other builds write them
 * as literals in mem_card.c. */
extern u8 gMemCard_szSaveFileName[];
extern char gMemCard_szFreeBlockCountFormat[];
extern char gMemCard_szMakeFileTrace[];
extern char gMemCard_szFailFileTrace[];

/* Japanese "BISLPMP86398-YUGIOH": the file name SaveData_RequestOptionsWrite
 * hands to MemCardDialog_Request, separate from the ordinary save name. */
extern u8 gMemCard_szOptionsFileName[];

/* The modal request's shared filename buffer.
 *
 * mem_card_dialog.c strcpy()s a name into it, and the load/save
 * operations then pass it as the (char *) filename to MemCardCreateFile,
 * MemCardGetDirentry, MemCardReadFile and MemCardWriteFile. Its consumers
 * include this header, so it belongs here rather than in unmatched.h: that
 * header is for symbols with no identified owner, and this one has an obvious
 * one.
 *
 * The incomplete-array spelling is kept. c_symbols.ld names gDisplayObject_awListHeads
 * thirty-two bytes later, but
 * nothing reads or writes through a bound, so no size is asserted here. */
extern u8 gMemCardDialog_FileNameBuffer[];

/* Mutable modal request fields: keep the raw encoded port byte and
 * unsigned-halfword size/offset narrowing and wrap at every existing store. */
extern u8 gMemCardDialog_Port;
extern u16 gMemCardDialog_TransferSize;
extern u16 gMemCardDialog_TransferOffset;

/* Takes full words. The body only stores the low byte of `value` and ors
 * `bits` into the flag halfword, so every constant caller builds the same
 * either way; MemCardDialog_UpdateSave is the caller that tells them apart, as
 * it passes a free-block count retail hands over without masking. */
void MemCardDialog_SetMessage(s32 value, s32 bits);

/* The memory-card dialog's flag word.
 *
 * Declared plain here because the readers need it plain: every other user
 * only tests bits or does a read-modify-write, and marking the object
 * volatile forces reloads that grow .text by 40 bytes.
 *
 * MemCardDialog_SetMessage needs the opposite and reaches the same word
 * through an explicit volatile lvalue; see the comment there.
 */
extern u16 gMemCard_wDialogFlags;

/* The second block of IO event handles, sixteen bytes past
 * gMemCard_aIOEventHandles at 0x800F2AE0, so four `long` handles apart. Both
 * are handed to MemCard_ClearIOEvents, and all three sources that name this
 * one already include this header and already spell the element `long`. That
 * spelling is preserved here for the same reason it is above: the event API
 * hands these back as long and nothing has measured the difference. */
extern long gMemCard_aHwIOEventHandles[];

/* The request state machines' shared state. Every symbol below was declared
 * identically by each of its users, all of which already include this header.
 *
 *   gMemCardDialog_Outcome  The open path seeds 2; load/save/trade paths
 *               publish 1 or 3. Poll exposes it only after pumping and
 *               observing zero flags. Start does not reset this byte.
 *   gMemCardDialog_BlockCount  The block count a save needs; computed by
 *               mem_card_dialog.c and passed as MemCardCreateFile's
 *               third argument.
 *   gMemCardDialog_StepIndex  The raw low byte of Start's s32 argument;
 *               Update uses it without a mask or range check to select
 *               one of the existing modal table callbacks.
 *   gMemCardDialog_RetryCounter  Shared duplicate-record attempt marker
 *               and regional SDK-probe countdown, not one universal
 *               remaining-retries policy.
 *   gMemCardDialog_SyncCommand  The SDK cmds output, separately passed
 *               beside the rslt output and tested against 2 by save work.
 *               NA adjacency does not describe the European layout.
 *   gMemCardDialog_DirectoryBuffer  The directory buffer, cast to (struct DIRENTRY *) at each
 *               use and passed with the file count beside it.
 *
 * gMemCardDialog_CardNumberArg keeps its unsized spelling, and it is load-bearing: the note in
 * mem_card_dialog.c records that as a plain s32 extern the -G8 build puts
 * it in small data and the store collapses to one gp-relative word, where
 * retail materialises the %hi half in its own register. Declaring it here
 * changes where the spelling lives, not the spelling. */
extern u8 gMemCardDialog_Outcome;
/* The IO event machinery's own two bytes.
 *
 *   gMemCard_bRequest   The request code, armed from MemCard_BeginRequest's
 *                       argument and tested as `>= 0`, `== 1` and `!= 8`,
 *                       so the sign and the specific values both matter.
 *                       -1 is idle; mem_card_begin_request.h lists the
 *                       codes. Its target reads it `lb` five times and `lbu`
 *                       once, so the poll spells that one read
 *                       `*(u8 *)&gMemCard_bRequest` and takes the signed
 *                       declaration for the rest.
 *   gMemCard_bDirFlags  A flag byte. MemCard_DoLoadDirectory sets bit 0x80
 *                       once it has tried to list the card and tests it to
 *                       skip the reload; the init path clears the whole
 *                       byte. */
extern s8 gMemCard_bRequest;
extern u8 gMemCard_bDirFlags;

/* One declaration each, and the reader is what fixes the type. The producer,
 * mem_card_driver.c, only *stores* these three -- one assignment to the step
 * (:120), four to the offset (:165, :182, :199, :216) and three to the size
 * (:167, :201, :233) -- and a store is `sb` or `sh` whichever sign the
 * declaration carries, so the signed views it used to select never reached an
 * instruction. The poll, src/game/mem_card_driver.c, is what reads them,
 * and its target listing fixes the widths and signs: eight `lbu` of the step,
 * three `lhu` of the offset and four of the size
 * in its byte-matched resident body.
 * Size is bytes for file I/O but blocks for create; offset is bytes for file
 * I/O but a sector number for the raw-card requests. */
extern u8 gMemCard_bRequestStep;
extern u16 gMemCard_wRequestOffset;
extern u16 gMemCard_wRequestSize;
extern char gMemCard_szRequestPath[];
extern s32 gMemCard_pRequestBuf;

/* The retry budget of the current stage. Stored by MemCard_BeginRequest
 * (mem_card_driver.c, `gMemCard_bRetries = 10;`;
 * MemCard_BeginRequest.s:8 `sb`) and by MemCard_DoLoadDirectory, which loads it
 * under u8 and matched: `v0 = gMemCard_bRetries - 1;
 * gMemCard_bRetries = (u8)v0;` in mem_card_driver.c,
 * and `gMemCard_bRetries = 0xA;`
 * (MemCard_DoLoadDirectory.s lbu :40, :94, :121; sb :43, :73, :97, :104, :124). The
 * writer spelled it char, where a store shows no sign; the gMemCard_bChannel
 * comment above records the same split. MemCard_ProcessRequest reads the byte,
 * decrements it with byte wrapping, and tests the signed result.
 * gMemCard_bLoadStep is the
 * next symbol, at +1 (c_symbols.ld:272). Every access is `%gp_rel`; plain
 * declaration. */
extern u8 gMemCard_bRetries;

/* MemCard_DoLoadDirectory's sub-state: 0 _card_info, 1 _card_clear,
 * 2 _card_load. Stored by MemCard_BeginRequest (mem_card_driver.c,
 * `gMemCard_bLoadStep = 0;`; MemCard_BeginRequest.s:13 `sb $zero`) and by
 * MemCard_DoLoadDirectory, which loads it under u8 and matched:
 * `v1 = gMemCard_bLoadStep;`,
 * `gMemCard_bLoadStep = (u8)(gMemCard_bLoadStep + 1);` and
 * `gMemCard_bLoadStep = 2;` in mem_card_driver.c (MemCard_DoLoadDirectory.s lbu :5, :72; sb :75,
 * :106). No function still in assembly names it. gMemCard_bRequest,
 * declared s8 above, is at +1 (c_symbols.ld:273). Every access is
 * `%gp_rel`; plain declaration. */
extern u8 gMemCard_bLoadStep;

/* The dialog's own three runtime values, alongside gMemCard_wDialogFlags
 * above. mem_card_dialog.c raises the box with
 *
 *     TextBox_Create(gMemCardDialog_ChannelIndex, gMemCardDialog_MessageCode, 0x20, 0x50, 0x100, 0x30)
 *
 * which is where two of them get their meaning, TextBox_Create being
 * (index, string_id, x, y, width, height).
 *
 *   gMemCardDialog_ChannelIndex  The effect channel the box uses. It defaults
 *               to 0, then selects the first slot lacking flags_34 0x8000;
 *               all-active input retains 0 rather than reporting failure.
 *               Selection is not a reservation, and
 *               every other use indexes gText_aChannels with it or hands it to
 *               TextBox_Destroy and MemCardDialog_StepSlide.
 *   gMemCardDialog_MessageCode  The low byte of SetMessage's full-word value,
 *               stored before its two volatile flag stores raising
 *               MEM_CARD_DIALOG_FLAG_RESULT_READY. Distinct from the paired
 *               presentation code owned by two_player_save_setup.h.
 *   gMemCard_pDialogObject
 *               The box's display object. Assigned when the dialog opens,
 *               null-tested before teardown and reset to 0 after
 *               DisplayObject_ReleaseIfPresent releases it; callers drive its field_60
 *               between -0x400 and 0x400.
 */
extern u8 gMemCardDialog_ChannelIndex;
extern u8 gMemCardDialog_MessageCode;
extern DisplayObject *gMemCard_pDialogObject;
extern u8 *gMemCard_pPrimaryTransferCursor;
extern u8 *gMemCard_pSecondaryTransferCursor;
extern u8 gMemCardDialog_BlockCount;
extern u8 gMemCardDialog_StepIndex;
extern u8 gMemCardDialog_RetryCounter;
extern s32 gMemCardDialog_SyncCommand;
/* Internal operation state and SDK sync words remain plain independent
 * globals; the raw command, result and poll return are different values. */
extern u8 gMemCardDialog_OperationState;
extern s32 gMemCardDialog_SyncStatus;
extern s32 gMemCardDialog_SyncResult;
extern u8 gMemCardDialog_DirectoryBuffer[];
/* Modal card-number view of shared text staging +0x40, also used through
 * deck-validation/rank views. Keep this separate unsized relocation name. */
extern s32 gMemCardDialog_CardNumberArg[];

/* The scrolling checkerboard backdrop: one empty entry point and the tiler
 * that fills the screen with alternating cells.
 *
 * Neither function has a caller. Nothing in src/ names them, and the only
 * references in the generated assembly are inside their own object, so the
 * two records below are described entirely by this translation unit. */

/* The state Checkerboard_DrawBackground reads its scroll offsets and phase from. Only the
 * three fields it touches are named; the gaps are padding to hold the offsets,
 * not evidence that the record ends at 0x16. */
typedef struct {
    /* None of the three is a scroll offset or phase. The tiler copies
       attribute into the sprite's GsSPRITE attribute word and rgb into its
       r/g/b word, and passes ot_depth to GsSortFastSprite as the ordering
       table priority. They sit at the offsets DisplayObject gives its own
       attribute, rgb and ot_depth, which fits a display object being
       handed in, but with no caller that is not proven. */
    u8 pad_00[4];
    s32 attribute;
    u8 pad_08[4];
    s32 rgb;
    u8 pad_10[4];
    s16 ot_depth;
} CheckerboardState;

/* PS1 scratchpad SPRT-primitive builder, reused for every sprite drawn. This
 * is the layout the tiler writes, not a Psy-Q type: it is filled by hand and
 * handed to the ordering table, so it is kept here rather than replaced with a
 * libgpu primitive. */
typedef struct {
    /* The first 0x18 bytes of a GsSPRITE, which is what every sprite is
       handed to GsSortFastSprite as, so the members take libgs.h's names
       (and SpritePrim's for the packed pairs): wh packs w in its low half
       and h in its high half, and uv's two bytes are u and v. */
    u32 attribute;
    u16 x;
    u16 y;
    u32 wh;
    u16 tpage;
    union {
        u16 word;
        struct {
            u8 u;
            u8 v;
        } bytes;
    } uv;
    u16 cx;
    u16 cy;
    u32 rgb;
} CheckerboardSprite;

void func_8003D32C(void);
void Checkerboard_DrawBackground(
    CheckerboardState *state, GsOT *ordering_table);

/* Public request API implemented by mem_card_dialog.c. */

/* Polls the memory-card dialog and reports its result, or 0 while a dialog
 * is still up. */
s32 MemCardDialog_Poll(void);

/* Raises the memory-card dialog on step `step` of the gMemCardDialog_apfnSteps table. */
void MemCardDialog_Start(s32 step);

/* Queues a memory-card transfer of size bytes from buf, to or from the file
 * named by name, with step selecting the operation. The callers use steps 0
 * (load), 1 (load without the confirmation prompt), 2 (save), 3 (Japanese
 * options-screen save) and 4. */
void MemCardDialog_Request(u8 *buf, s32 size, u8 *name, s32 step);

#ifdef VERSION_JAPAN
/* Options invokes step 3 for the fixed 0x8000-byte buffer at 0x80140000;
 * this does not establish that the buffer contains only option settings. */
extern void SaveData_RequestOptionsWrite(void);
#endif

/* Dialog transition callbacks, stored in gDialog_apfnTransitionSteps.
 * Their first-entry latch and progress bits share gDialog_bCallbackState with the
 * memory-card controller; this is not per-record state.
 *
 * Contraction and open create a spotlight mask and decrease its radii.
 * Close moves/releases the primary sprite and expands/releases that mask.
 * Objects use the existing integer row slots; the channel index stays a
 * byte view at +0x1A. Japan retains the 0x60-byte channel stride. */
void Dialog_UpdateSpotlightContraction(MenuRecord *state);
void Dialog_UpdateCloseTransition(MenuRecord *state);
void Dialog_UpdateOpenTransition(MenuRecord *state);

/* Message-only variant: create the sprite and channel, advance the slide
 * by 0x20 in the same call, then pump text after reaching the endpoint.
 * Completion clears the shared gDialog_bCallbackState byte, not the record or objects. */
void Dialog_UpdateMessageTransition(MenuRecord *record);

/* Opens a text-box channel for a dialog: clears the choice selection, builds
 * the box through TextBox_Create at the regional dialog rectangle, and hands
 * the channel back. The default is 0x20,0x50 / 0x100x0x40; the Spanish wrapper
 * uses 0x08,0x50 / 0x130x0x30.
 *
 * `value`'s low fifteen bits are the string id. Bit 15 is a request to run
 * TextBox_BuildBlocking on the new channel -- but only when `set_flags` is zero;
 * a non-zero `set_flags` instead sets 0x1008 in the channel's flags_34 and
 * the bit is ignored. The two arms are exclusive, which the two callers rely
 * on: one passes 0x80D4 with set_flags zero, the other 0xD0.
 *
 * The return is the channel, and it is worth having typed: dialog_transition.c
 * already declared it DuelEffectChannel * while Dialog_UpdateMessageTransition held `u8 *`
 * and reads the same record at raw +0x30 and +0x34. It casts.
 *
 * The text-box index is gMemCardWork_Area.root.text_index. */

DuelEffectChannel *DuelEffect_CreateChannel(s32 value, s32 set_flags);

/* The first four entries of the memory-card dialog step table gMemCardDialog_apfnSteps.
 * The load and save callbacks latch DUEL_EFFECT_STATE_FLAG_INITIALIZED, seed
 * gMemCardDialog_OperationState, and then pump the corresponding state machine. Step 1 skips the
 * load confirmation prompt. NA step 3 is empty; the Japanese and European
 * builds use MemCardDialog_StepAlternateSave instead. */
extern void MemCardDialog_UpdateLoad(void);
void MemCardDialog_StepLoad(void);
void MemCardDialog_StepLoadUnprompted(void);
void MemCardDialog_UpdateSave(void);
void MemCardDialog_StepSave(void);
void MemCardDialog_StepNone(void);
void MemCardDialog_StepAlternateSave(void);

/* The memory card dialog's modal runtime.
 *
 * MemCardDialog_UpdateTradeSave is the fifth entry of the step table
 * gMemCardDialog_apfnSteps (mem_card_dialog.c), selected only by
 * SaveData_RequestTradeWrite: it reads the save from each card slot in turn,
 * checks both against the staged records with SaveData_HasSameDuelistCode
 * and writes them back in alternating 0x80-byte chunks. MemCardDialog_Update
 * is the frame pump that dispatches that table; the request API at the end of
 * the same translation unit calls it before reading the outcome. */
void MemCardDialog_UpdateTradeSave(void);
extern void MemCardDialog_Update(void);

/* Selects the first inactive channel, or retains zero if all are active,
 * then creates the display object. Selection is not channel reservation. */
void MemCardDialog_CreateObject(void);

/* One frame of the dialog box's slide. Callers load the phase at +0x60 with
 * -0x400 to open or 0x400 to close. Each call moves the phase 0x40 toward
 * zero, eases the object against (target_x, target_y) through Widget_SlideSine and, for
 * index >= 0, moves text box `index` to the result. When the phase reaches
 * zero it clears field_6C, and that byte is the return value, so every
 * caller tests it against zero to mean "slide finished".
 * Japanese text-box channels have a 0x60-byte stride instead of 0x64.
 *
 * The byte is returned as s32. Returning it as u8 costs nothing in the
 * definition, where the lbu already zero-extends, but makes each caller mask
 * the result before its test. */
s32 MemCardDialog_StepSlide(DisplayObject *object, s32 target_x, s32 target_y,
                            s32 index);

#define FUNC_8003D300_STATE_SIZE 10

/* Six-store setup for the legacy work operation. No I/O is submitted and
 * neither the whole workspace nor the separate modal runtime is reset. */
void MemCardWork_InitializeOperation(u8 value);

#define SAVE_DATA_HEADER_SIZE 0x200
#define SAVE_DATA_STATE_SIZE 0x680
#define SAVE_DATA_REPLICATED_STATE_SIZE (SAVE_DATA_STATE_SIZE * 2)
#define SAVE_DATA_DUPLICATE_STATE_OFFSET \
    (SAVE_DATA_HEADER_SIZE + SAVE_DATA_STATE_SIZE)
#define SAVE_DATA_CARD_QUANTITIES_OFFSET 0x50
#define SAVE_DATA_DUELIST_CODE_OFFSET 0x334
#define SAVE_DATA_CREDITS_FLAGS_OFFSET 0x3DE
#define SAVE_DATA_SEQUENCE_OFFSET 0x404
#define SAVE_DATA_VBLANK_COUNTER_OFFSET 0x408
#define SAVE_DATA_PLAYER_NAME_OFFSET 0x40C
#define SAVE_DATA_PLAYER_NAME_CHAR_COUNT 6
#define SAVE_DATA_PLAYER_NAME_SIZE \
    (SAVE_DATA_PLAYER_NAME_CHAR_COUNT * sizeof(u16))
#define TWO_PLAYER_SAVE_SLOT_STRIDE 0x1000
#define TWO_PLAYER_SAVE_TRANSFER_SIZE 0x400
#define SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET 0x5DC
#define SAVE_DATA_OUTPUT_TYPE_OFFSET 0x5DE
#define SAVE_DATA_DUEL_WINS_OFFSET 0x518
#define SAVE_DATA_DUEL_LOSSES_OFFSET 0x51A
#define SAVE_DATA_STARCHIPS_OFFSET 0x5E0
#define SAVE_DATA_STARCHIP_MAX 999999

/* Existing primary-save identity allowance. Keep its raw byte and the
 * absolute writer/plain modal-reader declaration views separate. */
extern u8 gSaveData_AllowIdentityBypass;

#define SAVE_DATA_CRC16_BITS 16
#define SAVE_DATA_CRC16_MASK ((1 << SAVE_DATA_CRC16_BITS) - 1)
#define SAVE_DATA_CRC16_HIGH_BIT (1 << (SAVE_DATA_CRC16_BITS - 1))
#define SAVE_DATA_CRC16_POLYNOMIAL 0x1021

#define SAVE_DATA_PRIMARY_LENGTH 0x340
#define SAVE_DATA_PRIMARY_MASK_LAST_OFFSET 0x378
#define SAVE_DATA_PRIMARY_MASK_WORD_COUNT 15
#define SAVE_DATA_PRIMARY_CHECKSUM_OFFSET \
    (SAVE_DATA_PRIMARY_MASK_LAST_OFFSET + sizeof(s32))

#define SAVE_DATA_SECONDARY_OFFSET 0x380
#define SAVE_DATA_SECONDARY_LENGTH 0x6C
#define SAVE_DATA_SECONDARY_MASK_LAST_OFFSET 0x3F8
#define SAVE_DATA_SECONDARY_MASK_WORD_COUNT 4
#define SAVE_DATA_SECONDARY_CHECKSUM_OFFSET \
    (SAVE_DATA_SECONDARY_MASK_LAST_OFFSET + sizeof(s32))

#define SAVE_DATA_TERTIARY_OFFSET 0x400
#define SAVE_DATA_TERTIARY_LENGTH 0x204
#define SAVE_DATA_TERTIARY_CHECKSUM_OFFSET \
    (SAVE_DATA_TERTIARY_OFFSET + SAVE_DATA_TERTIARY_LENGTH)
#define SAVE_DATA_TERTIARY_MASK_LAST_OFFSET 0x624
#define SAVE_DATA_TERTIARY_MASK_WORD_COUNT 8
#define SAVE_DATA_RESERVED_TAIL_OFFSET \
    (SAVE_DATA_TERTIARY_MASK_LAST_OFFSET + sizeof(s32))
#define SAVE_DATA_RESERVED_TAIL_SIZE \
    (SAVE_DATA_STATE_SIZE - SAVE_DATA_RESERVED_TAIL_OFFSET)
#define SAVE_DATA_RESERVED_TAIL_PAYLOAD_OFFSET \
    (SAVE_DATA_HEADER_SIZE + SAVE_DATA_RESERVED_TAIL_OFFSET)

/* Seal views only: payload bytes keep the independent SaveDataState layout. */
typedef struct {
    u8 payload[SAVE_DATA_PRIMARY_LENGTH];
    s32 mask_words[SAVE_DATA_PRIMARY_MASK_WORD_COUNT];
    u16 checksum[2];
} SaveDataPrimaryIntegrity;

typedef struct {
    u8 payload[SAVE_DATA_SECONDARY_LENGTH];
    s32 mask_words[SAVE_DATA_SECONDARY_MASK_WORD_COUNT];
    u16 checksum[2];
} SaveDataSecondaryIntegrity;

typedef struct {
    u8 payload[SAVE_DATA_TERTIARY_LENGTH];
    u16 checksum[2];
    u32 mask_words[SAVE_DATA_TERTIARY_MASK_WORD_COUNT];
} SaveDataTertiaryIntegrity;

#define SAVE_DATA_INTEGRITY_OFFSET(type, member) ((u32)&((type *)0)->member)

typedef char SaveDataPrimaryIntegrity_size_must_be_0x380[
    sizeof(SaveDataPrimaryIntegrity) == SAVE_DATA_SECONDARY_OFFSET ? 1 : -1
];
typedef char SaveDataPrimaryIntegrity_mask_offset_must_be_0x340[
    SAVE_DATA_INTEGRITY_OFFSET(SaveDataPrimaryIntegrity, mask_words) ==
        SAVE_DATA_PRIMARY_LENGTH ? 1 : -1
];
typedef char SaveDataPrimaryIntegrity_checksum_offset_must_be_0x37c[
    SAVE_DATA_INTEGRITY_OFFSET(SaveDataPrimaryIntegrity, checksum) ==
        SAVE_DATA_PRIMARY_CHECKSUM_OFFSET ? 1 : -1
];
typedef char SaveDataSecondaryIntegrity_size_must_be_0x80[
    sizeof(SaveDataSecondaryIntegrity) ==
        SAVE_DATA_TERTIARY_OFFSET - SAVE_DATA_SECONDARY_OFFSET ? 1 : -1
];
typedef char SaveDataSecondaryIntegrity_mask_offset_must_be_0x6c[
    SAVE_DATA_INTEGRITY_OFFSET(SaveDataSecondaryIntegrity, mask_words) ==
        SAVE_DATA_SECONDARY_LENGTH ? 1 : -1
];
typedef char SaveDataSecondaryIntegrity_checksum_offset_must_be_0x7c[
    SAVE_DATA_INTEGRITY_OFFSET(SaveDataSecondaryIntegrity, checksum) ==
        SAVE_DATA_SECONDARY_CHECKSUM_OFFSET - SAVE_DATA_SECONDARY_OFFSET
            ? 1 : -1
];
typedef char SaveDataTertiaryIntegrity_size_must_be_0x228[
    sizeof(SaveDataTertiaryIntegrity) ==
        SAVE_DATA_RESERVED_TAIL_OFFSET - SAVE_DATA_TERTIARY_OFFSET ? 1 : -1
];
typedef char SaveDataTertiaryIntegrity_checksum_offset_must_be_0x204[
    SAVE_DATA_INTEGRITY_OFFSET(SaveDataTertiaryIntegrity, checksum) ==
        SAVE_DATA_TERTIARY_LENGTH ? 1 : -1
];
typedef char SaveDataTertiaryIntegrity_mask_offset_must_be_0x208[
    SAVE_DATA_INTEGRITY_OFFSET(SaveDataTertiaryIntegrity, mask_words) +
        (SAVE_DATA_TERTIARY_MASK_WORD_COUNT - 1) * sizeof(u32) ==
        SAVE_DATA_TERTIARY_MASK_LAST_OFFSET - SAVE_DATA_TERTIARY_OFFSET
            ? 1 : -1
];

#undef SAVE_DATA_INTEGRITY_OFFSET

/* Observed prefix, not the full 0x680-byte persistent state allocation. */
typedef struct SaveDataState {
    u16 player_deck[DECK_SIZE];
    u8 card_quantities[CARD_COUNT];
    u8 pad_322[
        SAVE_DATA_DUELIST_CODE_OFFSET -
        (DECK_SIZE * sizeof(u16) + CARD_COUNT)
    ];
    s32 duelist_code;
    u8 pad_338[SAVE_DATA_CREDITS_FLAGS_OFFSET -
        (SAVE_DATA_DUELIST_CODE_OFFSET + sizeof(s32))];
    /* Main_RunCredits sets the low two bits when the ending starts, beside
     * parking campaign_scene_index at 0x30 and marking every card used. */
    u8 credits_flags;
    u8 pad_3DF[
        SAVE_DATA_TERTIARY_OFFSET -
        (SAVE_DATA_CREDITS_FLAGS_OFFSET + sizeof(u8))
    ];
    /* The first word the tertiary CRC covers. SaveData_BuildPayload zeroes
     * it before sealing; no reader of it is known. */
    s32 field_400;
    u32 save_sequence;
    u32 vblank_counter;
    u8 player_name_sjis[SAVE_DATA_PLAYER_NAME_SIZE];
    u8 campaign_flags[
        (CAMPAIGN_FLAG_ID_MASK + 1) >> CAMPAIGN_FLAG_BYTE_SHIFT
    ];
    u16 duel_wins;
    u16 duel_losses;
    SaveDataDuelistRecord duelist_records[FREE_DUEL_GRID_ENTRY_COUNT];
    u8 pad_5BC[
        SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET -
        (0x51C + sizeof(SaveDataDuelistRecord) * FREE_DUEL_GRID_ENTRY_COUNT)
    ];
    /* The campaign scene byte. The save prompt and the credits store a
     * halfword here, which also covers the opaque byte after it. */
    union {
        s16 word;
        struct {
            u8 index;
            u8 field_5DD;
        } bytes;
    } campaign_scene;
    u8 output_type;
    u8 pad_5DF;
    u32 starchips;
} SaveDataState;

/* The live state starts at +0x200 in the reusable 0x801D0000 workspace.
 * This view describes only that prefix, not the cleared 0x3000-byte arena. */
typedef struct {
    u8 prefix[SAVE_DATA_HEADER_SIZE];
    SaveDataState state;
} SaveDataWorkspace;

/* A whole save payload as SaveData_BuildPayload assembles it: the header
 * template, the 0x680-byte state block, and the 0x680-byte duplicate the
 * memory card layer compares against it. SaveDataState only names the
 * state's first 0x5E4 bytes; state_tail is the rest, where the tertiary seal
 * ends and the reserved tail that BuildPayload clears begins. BuildPayload
 * copies all SAVE_DATA_STATE_SIZE bytes into duplicate, so duplicate_tail
 * carries the copy of state_tail and the payload ends at 0xF00. */
typedef struct {
    u8 header[SAVE_DATA_HEADER_SIZE];
    SaveDataState state;
    u8 state_tail[SAVE_DATA_STATE_SIZE - sizeof(SaveDataState)];
    SaveDataState duplicate;
    u8 duplicate_tail[SAVE_DATA_STATE_SIZE - sizeof(SaveDataState)];
} SaveDataPayload;

typedef char SaveDataState_card_quantities_offset_must_be_0x50[
    (u32)&(((SaveDataState *)0)->card_quantities) ==
        SAVE_DATA_CARD_QUANTITIES_OFFSET ? 1 : -1
];
typedef char SaveDataState_duelist_code_offset_must_be_0x334[
    (u32)&(((SaveDataState *)0)->duelist_code) ==
        SAVE_DATA_DUELIST_CODE_OFFSET ? 1 : -1
];
typedef char SaveDataState_credits_flags_offset_must_be_0x3DE[
    (u32)&(((SaveDataState *)0)->credits_flags) ==
        SAVE_DATA_CREDITS_FLAGS_OFFSET ? 1 : -1
];
typedef char SaveDataState_campaign_flags_offset_must_be_0x418[
    (u32)&(((SaveDataState *)0)->campaign_flags) ==
        CAMPAIGN_FLAG_BANK_OFFSET - SAVE_DATA_HEADER_SIZE ? 1 : -1
];
typedef char SaveDataState_duelist_records_offset_must_be_0x51C[
    (u32)&(((SaveDataState *)0)->duelist_records) == 0x51C ? 1 : -1
];
typedef char SaveDataState_duel_totals_offsets_must_match[
    (u32)&(((SaveDataState *)0)->duel_wins) == SAVE_DATA_DUEL_WINS_OFFSET &&
    (u32)&(((SaveDataState *)0)->duel_losses) == SAVE_DATA_DUEL_LOSSES_OFFSET
        ? 1 : -1
];
typedef char SaveDataState_campaign_scene_index_offset_must_be_0x5DC[
    (u32)&(((SaveDataState *)0)->campaign_scene) ==
        SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET ? 1 : -1
];
typedef char SaveDataState_starchips_offset_must_be_0x5E0[
    (u32)&(((SaveDataState *)0)->starchips) ==
        SAVE_DATA_STARCHIPS_OFFSET ? 1 : -1
];
typedef char SaveDataState_size_must_be_0x5E4[
    sizeof(SaveDataState) == 0x5E4 ? 1 : -1
];
typedef char SaveDataPayload_duplicate_offset_must_follow_the_state[
    (u32)&(((SaveDataPayload *)0)->duplicate) ==
        SAVE_DATA_DUPLICATE_STATE_OFFSET ? 1 : -1
];
typedef char SaveDataPayload_size_must_be_0xF00[
    sizeof(SaveDataPayload) ==
        SAVE_DATA_HEADER_SIZE + SAVE_DATA_REPLICATED_STATE_SIZE ? 1 : -1
];
typedef char SaveDataWorkspace_state_offset_must_be_0x200[
    (u32)&(((SaveDataWorkspace *)0)->state) == SAVE_DATA_HEADER_SIZE ? 1 : -1
];

extern u8 gSaveData_Workspace[];
extern u8 gLibrary_abCardChest[];
extern u8 gSaveData_aPlayerNameSjis[];
extern SaveDataDuelistRecord gFreeDuel_aDuelistRecords[];

/* The head of the 0x680-byte persistent state block: SaveData_RequestWrite
 * copies SAVE_DATA_STATE_SIZE bytes starting here. Halfwords, as the name
 * says and as duel_deck_lookup.c, script_scene.c, library_runtime.c
 * and free_duel/screen_runtime.c read it -- the files that walk it as bytes
 * are copying or scanning the block, not indexing the deck. */
extern u16 gDuel_awPlayerDeck[];

/* The campaign scene index as it is kept inside that block, which is what
 * makes it survive a save: Main_ApplyMenuSelection (main_mode_runners.c)
 * writes 0x30 into it when a new game starts and reads it back into the
 * runtime gCampaignSceneIndex when the campaign is resumed. Those two are
 * different objects -- the runtime one is at 0x8009B27A, outside this block --
 * and this is the only source in the tree that names either of them together.
 *
 * It was declared inside that source because this header did not declare it,
 * though the header already owns both of its neighbours. The address is
 * 0x801D07DC: gDuel_awRecentCardDrops ends exactly here (0x801D07BC plus
 * DUEL_RECENT_CARD_DROP_COUNT halfwords is 0x801D07DC, so it abuts without
 * covering), and gLibrary_dwStarchips begins four bytes on.
 *
 * Those four bytes are recorded as evidence about the extent, not asserted as
 * a bound: the declaration stays the incomplete s16 array its one declarer
 * wrote, and only [0] is ever read or written. */
extern s16 gCampaignSavedSceneIndex[];

extern u32 gLibrary_dwStarchips;

extern u8 gSaveData_aTransferBuffer[];
extern u8 gSaveData_aHeaderTemplate[];

extern s32 gSaveDataSequence;
/* The two words of the save-data mask state. save_data_payload.c used to
 * declare these itself, behind a SAVE_DATA_DECLARE_MASK_STATE_LOCALLY guard
 * that suppressed the pair here, with the reason recorded in that file:
 * "keeping these declarations at their original source position preserves
 * the exact GCC 2.8.1 allocation in SaveData_NextMaskWord". That was true
 * when it was written on 2026-09-10; it is not true now, and the check is
 * one build: with the guard and the local pair deleted the image still
 * hashes 84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88.
 * The guard had exactly one definer, so nothing else is affected. */
extern u32 gSaveData_dwMaskStateLow;
extern u32 gSaveData_dwMaskStateHigh;

u32 SaveData_NextMaskWord(void);
void SaveData_SetMaskSeed(u32);
u32 SaveData_CalcCrc16(u8 *, s32);
void SaveData_WritePrimarySecondaryIntegrity(u8 *);
void SaveData_WriteTertiaryIntegrity(u8 *);
void SaveData_BuildPayload(SaveDataPayload *);
s32 SaveData_ValidateIntegrity(u8 *);
void SaveData_ApplyRuntimeState(SaveDataState *state);

/* Stages the runtime state into gSaveData_aTransferBuffer, builds the payload
 * ahead of it through SaveData_BuildPayload, and asks the memory card layer to
 * write it under the card save file name.
 *
 * Its earlier C caller, Script_OpSavePrompt (script_scene.c),
 * reached it with no declaration in scope at
 * all before this, so neither the argument list nor the void return was being
 * checked against the definition. Main_RunCredits uses this same declaration
 * when requesting the completion save. */
void SaveData_RequestWrite(void);

/* The load side of the same pair, both reached from the main-menu overlay's
 * boot path. SaveData_RequestLoad starts the read into
 * gSaveData_aTransferBuffer; SaveData_PollLoad reports how it finished, and
 * MainMenu_UpdateFrontendMenu (src/overlays/main_menu/frontend.c) branches on that result.
 *
 * Both were declared in that overlay and nowhere else. */
void SaveData_RequestLoad(void);
s32 SaveData_PollLoad(void);
s32 SaveData_HasSameDuelistCode(
    SaveDataState *left,
    SaveDataState *right
);
s32 SaveData_MatchesDuelistAndCurrentSequence(
    SaveDataState *left,
    SaveDataState *right
);

s32 SaveData_UpdateLoadPair(void);

s32 SaveData_UpdateTradeLoad(void);

/* The message the two-save load shows when it rejects the pair:
 * mem_card_dialog.c sets it to 0x29, 40 and 36 at different failures
 * and reads it
 * back. All three spell it plain u8. */
extern u8 gSaveData_PairMessageCode;

/* gSaveData_LoadPairState and gSaveData_PairSetupState are one byte each
 * (c_symbols.ld:278 and :280); no unit defines either.
 *
 * gSaveData_PairSetupState: SaveData_UpdateTradeLoad and SaveData_UpdateDuelLoad in
 * mem_card_dialog.c test bit 0x80
 * clear, set it and store
 * gSaveData_PairMessageCode; DebugMenu_UpdateTradeEntry (debug_menu.c),
 * DebugMenu_UpdateTwoPlayerDuelEntry (debug_menu.c:109) and
 * MainMenu_UpdateFrontendMenu (cases 3 and 2 of
 * its gMain_bMenuID switch, src/overlays/main_menu/frontend.c) store 0.
 *
 * gSaveData_LoadPairState: SaveData_UpdateLoadPair in mem_card_dialog.c masks it
 * with 0xF, tests bits 0x80 and 0x40, stores 1, 0x82, 2, 3, 0xA and 0xB,
 * ORs 0x80, 0x40 and 0xC0 and ANDs 0xBF into it; SaveData_UpdateDuelLoad
 * stores 10 in the same unit; the three functions above store 0.
 *
 * u8 because the grouped unit's former sources already declared it u8 and
 * matched, and the retail loads are lbu. Initial value not read.
 * notes/fm-online.md:177-187 records the gSaveData_LoadPairState store at
 * 0x8003FAE8 and leaves its meaning unresolved. */
extern u8 gSaveData_LoadPairState;
extern u8 gSaveData_PairSetupState;

/* The setup/write API at the end of mem_card_dialog.c. Both functions
 * were reached through local externs
 * before this header existed, and every one of those declarations already
 * agreed with its definition.
 *
 * SaveData_UpdateDuelLoad reports a status its callers poll:
 * MainMenu_UpdateFrontendMenu (src/overlays/main_menu/frontend.c) stores the result for
 * main-menu item 2 and debug_menu.c tests it
 * before entering the two-player duel setup. On top of the trade load it
 * requires a full deck in both saves and converts both player names.
 *
 * SaveData_RequestTradeWrite stamps integrity over both slots of the pair at
 * gSaveData_aTradeWriteStaging, records the second slot in
 * gMemCard_pSecondaryTransferCursor, and starts dialog step 4
 * to write them back. MainMenu_UpdateTradeScreen
 * (src/overlays/main_menu/trade_update.c) calls it once, after moving the
 * traded card counts. */
s32 SaveData_UpdateDuelLoad(void);
void SaveData_RequestTradeWrite(void);

#endif
