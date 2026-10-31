#ifndef MEMORIES_DECOMP_MEM_CARD_H
#define MEMORIES_DECOMP_MEM_CARD_H

#include "../types.h"
#include "display_object.h"

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
extern PSXLONG gMemCard_aIOEventHandles[];

#ifdef GMEMCARD_NIORESULT_IS_VOLATILE

extern volatile s32 gMemCard_nIOResult;
#else
extern s32 gMemCard_nIOResult;
#endif

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

extern u8 gMemCard_szSaveFileName[];

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
extern PSXLONG gMemCard_aHwIOEventHandles[];

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
 * func_800440B4.s:8 `sb`) and by MemCard_DoLoadDirectory, which loads it
 * under u8 and matched: `v0 = gMemCard_bRetries - 1;
 * gMemCard_bRetries = (u8)v0;` in mem_card_driver.c,
 * and `gMemCard_bRetries = 0xA;`
 * (func_80044608.s lbu :40, :94, :121; sb :43, :73, :97, :104, :124). The
 * writer spelled it char, where a store shows no sign; the gMemCard_bChannel
 * comment above records the same split. MemCard_ProcessRequest reads the byte,
 * decrements it with byte wrapping, and tests the signed result.
 * gMemCard_bLoadStep is the
 * next symbol, at +1 (c_symbols.ld:272). Every access is `%gp_rel`; plain
 * declaration. */
extern u8 gMemCard_bRetries;

/* MemCard_DoLoadDirectory's sub-state: 0 _card_info, 1 _card_clear,
 * 2 _card_load. Stored by MemCard_BeginRequest (mem_card_driver.c,
 * `gMemCard_bLoadStep = 0;`; func_800440B4.s:13 `sb $zero`) and by
 * MemCard_DoLoadDirectory, which loads it under u8 and matched:
 * `v1 = gMemCard_bLoadStep;`,
 * `gMemCard_bLoadStep = (u8)(gMemCard_bLoadStep + 1);` and
 * `gMemCard_bLoadStep = 2;` in mem_card_driver.c (func_80044608.s lbu :5, :72; sb :75,
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
 *               every other use indexes D_800EB0F8 with it or hands it to
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
extern DisplayObject *G32 gMemCard_pDialogObject;
extern u8 *G32 gMemCard_pPrimaryTransferCursor;
extern u8 *G32 gMemCard_pSecondaryTransferCursor;
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

#ifdef VERSION_JAPAN
/* Library calls only the Japanese executable has; the save dialog's state 2
 * polls them. Their prototypes are read off those call sites. */
void func_8008D070(PSXLONG chan);
PSXLONG func_8008DCB4(PSXLONG mode, PSXLONG *cmds, PSXLONG *result);
void func_8008CF10(PSXLONG chan, PSXLONG *apl);
#endif

#endif
