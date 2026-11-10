#ifndef MEMORIES_DECOMP_FILE_TRANSFER_H
#define MEMORIES_DECOMP_FILE_TRANSFER_H

#include "../ygo_types.h"
#include "../psyq/libcd.h"
#include "file_constants.h"

#define FILE_TRANSFER_STATE_PRIMARY_ACTIVE 0x10
#define FILE_TRANSFER_STATE_SECONDARY_PENDING 0x20
#define FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED 0x40
#define FILE_TRANSFER_STATE_COMMAND_BUSY 0x400
#define FILE_TRANSFER_STATE_POSITION_QUERY_BUSY 0x800
#define FILE_TRANSFER_STATE_POSITION_QUERY_PENDING 0x1000
#define FILE_TRANSFER_REQUEST_BLOCKED_MASK 0x02000030

/* File_ActivateTransfer promotes request slot 1 into slot 0, and
   SD_ConfigureTransferPhase consumes slot 0. The whole-record copies take its
   address as bytes at the use. */
extern FileRequestSlot gFile_ActiveRequestSlot;
/* Range-state wait count, not a frame count; keep the signed completion view. */
extern u16 gFile_RangeWaitCountdown;

typedef char FileTransfer_default_image_must_fill_sector[
    FILE_TRANSFER_DEFAULT_IMAGE_WORD_WIDTH * FILE_TRANSFER_DEFAULT_IMAGE_HEIGHT *
        sizeof(u16) == FILE_SECTOR_SIZE ? 1 : -1
];

void File_InitTransferState(s32);
void File_GetPosition(s32 *, const char *);
void File_SetTransferLocation(
    FileTransferDescriptor *transfer, s32 file_index, s32 sector_offset, s32 size
);
/* The signed initial_phase_index is stored in the descriptor's u32 phase
 * cursor. Callbacks may redirect it; it is not a return/completion status. */
FileTransferDescriptor *File_RequestAsyncTransfer(
    s32 flags, u8 *loader_argument, s32 sector_offset, s32 size,
    FileTransferCallback callback, s32 initial_phase_index, s32 target
);
FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32 flags, u8 *loader_argument, s32 sector_offset, s32 size,
    FileTransferCallback callback, s32 initial_phase_index, s32 target
);
FileTransferDescriptor *File_RequestSecondaryAsyncTransfer(
    s32 flags, u8 *loader_argument, s32 sector_offset, s32 size,
    FileTransferCallback callback, s32 initial_phase_index, s32 target
);
FileTransferDescriptor *File_RequestSecondaryRangeTransfer(
    s32 start_lba, s32 end_lba, s32 filter_channel, s32 filter_file
);
FileTransferDescriptor *File_InitTransferDescriptor(
    FileTransferDescriptor *transfer, s32 flags, u8 *loader_argument,
    s32 sector_offset, s32 size, FileTransferCallback callback,
    s32 initial_phase_index, s32 target
);
FileTransferDescriptor *File_RequestSecondarySeek(s32 file_index, s32 sector_offset);
FileTransferDescriptor *File_TryStartPrimaryTransfer(
    s32 file_index, s32 sector_offset
);
/* The four command-completion callbacks the runtime installs through
 * DsCommand and DsPacket: each re-issues its command on event 5 and clears
 * the busy bit on event 2. */
void File_HandleReadEvent(u8 event);
void File_HandleSeekEvent(u8 event);
void File_HandleCancelPauseEvent(u8 event);
void File_HandleRangePauseEvent(s32 event);
/* The DsStartReadySystem callback, in libds's DslRCB shape. Only the
 * interrupt byte is read. */
void File_TransferReadyCallback(u8 intr, u8 *result, u32 *unused);

void File_StepActiveTransfer(void);
void File_ServiceTransfers(s32 arg0);
void SD_ConfigureTransferPhase(FileTransferDescriptor *descriptor, s32 phase_index);
/* The sound producer passes FileRequestSlot directly; null polls the pending
   state. The result retains its historical status-or-descriptor integer ABI. */
s32 File_RequestSecondarySlot(FileRequestSlot *request, u8 *loader_argument);
void File_ActivateTransfer(void);
void File_WaitForTransfers(void);
void File_RequestMainMenuPackage(void);
void File_RequestNameEntryPackage(void);
void File_RequestPasswordPackage(void);
void File_RequestEgyptOverworldPackage(void);
void File_RequestOptionsPackage(void);
void File_RequestGameOverPackage(void);
void Options_LoadPackageStage(FileTransferDescriptor *descriptor, s32 mode);
void GameOver_LoadPackageStage(
    FileTransferDescriptor *descriptor,
    s32 mode
);
void MainMenu_LoadPackageStage(FileTransferDescriptor *descriptor, s32 stage);

/* Puts the loader's control halfword at 0x8009B112 into mode 2, clearing the
   other bits of its low field. Every caller reaches it through the same
   guard: only when a primary transfer is active and it is a sector-range
   one. */
void File_StopRangeTransfer(void);

/* That guard on its own, with no other work. File_WaitForTransfers spins on
   it while a transfer is outstanding and no secondary request is pending. */
void File_StopActiveRangeTransfer(void);

/* Raises the secondary-request word to 0x80 if anything is still in flight,
   running File_StopActiveRangeTransfer's guard first. The pad handler in DebugMenu_UpdateSoundEntry
   calls it to abandon the wait. */
void File_CancelTransfers(void);

/* The resident loader's request-and-state word at 0x8009B0F4.
 *
 * Every File_* entry point and every CD/DS sector callback tests or updates
 * it, and the FILE_TRANSFER_STATE_*, FILE_TRANSFER_FLAG_SECTOR_RANGE and
 * FILE_TRANSFER_REQUEST_BLOCKED_MASK bits declared above are its bits.
 * FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED blocks secondary promotion while
 * File_RequestAsyncTransfer initializes the primary request. The command-busy
 * bit is raised after a successful DsCommand/DsPacket submission and cleared
 * by its completion callback. The position-query pair gates DsCommand 0x10:
 * File_HandleRangeReadEvent raises pending, File_StepActiveTransfer submits it and raises
 * busy, and File_HandleLogicalPositionEvent clears busy. The word is only ever read and written
 * whole, and only ever through bit masks. Nothing indexes it, so the
 * `gFile_dwTransferState[0]` spellings this header replaces were an addressing device
 * rather than evidence of an array.
 *
 * `volatile` is load-bearing, measured rather than assumed: dropping it
 * builds a 0x1D0668-byte executable against the retail 0x1D0800.
 *
 * The retail image reaches this address both ways. The ready-sector callback
 * in `file_transfer_runtime.c` uses `%gp_rel(gFile_dwTransferState)($gp)`
 * seven times, while six other generated assembly files use `lui %hi` /
 * `%lo` fifty-nine times. */
/* The word at 0x800101D8, which holds the address 0x80168000: the base of the
 * WA_MRG screen-bank overlay slot (notes/memory-map.md) that each screen
 * package's executable phase loads into.
 *
 * It reaches the same two descriptor fields that take a buffer address in the
 * case immediately above it -- Password_LoadPackageStage's case 2 writes
 * `sector_destination = (s32)gFile_StagingBuffer`, and its case 3 writes this word into the same
 * two fields -- so the word is a stored address and is declared as one.  Every read is a single load of that word; the array and
 * scalar spellings this replaces reached it as `*(s32 *)(gFile_pScreenBankBase)` and as
 * a plain read, which are the same load. It sits at 0x800101D8, out of $gp's
 * range, and every unit reaches it with %hi/%lo.
 */
extern u8 *gFile_pScreenBankBase;

/* The per-file starting sector table the transfer setup indexes by file
   number. Four sources reached it, all with this identical declaration and
   none of them defining it, so it is still generated data. Entries follow
   gFile_apszName: the movie player seeks from MOVIE.STR's LBA, file
   transfer starts secondary (XA) ranges from MASTER.XA's, and Main_Init
   passes that LBA's low byte to SD_InitState. */
#define FILE_INDEX_MOVIE_STR 3
#define FILE_INDEX_MASTER_XA 6
extern s32 gFile_anLba[];

extern volatile u32 gFile_dwTransferState;

/* The transfer-step flag word. File_StepActiveTransfer sets and clears every
 * bit of it through the streaming retry state machine and reloads it after
 * each store; File_ActivateTransfer ORs in bit 0; the two readers outside
 * this family test the set and clear phases of bit 0x4000, which is set when
 * the range read advances the stepper from 5 to 6 and cleared by the 0x3FFC
 * mask when the range ends or stops. volatile keeps File_StepActiveTransfer's back-to-back read-modify-writes
 * from folding. */
extern volatile u16 gFile_wTransferStepFlags;

/* The loader's secondary-request word at 0x8009B134, the other half of the
 * `(gFile_dwTransferState & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | gFile_dwCancelRequestState` predicate
 * that eighteen units use to ask whether a transfer is still in flight.
 * `File_CancelTransfers` and `File_FinishActiveTransfer` raise it to 0x80, the frame pump
 * `File_ServiceTransfers` latches 0x40 into it once and otherwise clears it, and
 * `File_InitTransferState` zeroes it with the rest of the loader block.
 *
 * It takes the same two addressing views as gFile_dwTransferState, for the same reason,
 * but it is deliberately *not* volatile. That is a measured difference
 * between the two neighbouring words, not an oversight: declaring it
 * volatile makes `File_ServiceTransfers` re-load it for the 0x40 test and again for
 * the `|=`, where retail keeps one load live in `$3` across all three uses.
 */
extern u32 gFile_dwCancelRequestState;

/* 0x801DC000, gLibrary_aCardArtRecord in config/slus_01411/symbols.txt:372.
 * File_SetPositionTable hands its address to File_InitTransferState
 * (src/game/file_set_position_table.c), which stores it into gFile_TransferBufferBase
 * (File_InitTransferState, file_transfer_runtime.c). The two memory-card dialogs also reach it, always by
 * address: MemCardDialog_UpdateSave and MemCardDialog_UpdateTradeSave
 * (mem_card_dialog.c) pass it to MemCardReadFile as the destination of a
 * read whose last argument is 0x480, and compare it as a SaveDataState.
 * Every retail access is an address-take (one in File_SetPositionTable,
 * two in each of MemCardDialog_UpdateSave and
 * MemCardDialog_UpdateTradeSave), so the code says nothing
 * about the object's width; gFile_abPackageStagingBuffer
 * (file_package_staging.h) is named 0x1000 bytes higher and
 * gText_GlyphUploadScratch 0x2000 lower. The three units used to declare it
 * privately, all as `u8 []`. */
extern u8 gLibrary_aCardArtRecord[];

/* The primary transfer descriptor. Four sources in this family reach it as a
 * FileTransferDescriptor, agreeing on the spelling, and none defines it. */
extern FileTransferDescriptor gFile_PrimaryTransferDescriptor;

/* The initialized .sdata pointer targets the primary descriptor, which may
   be switched by a phase callback. The sector callback reads it through
   the FileTransferDescriptor members; only its image-phase buffer select is
   still an address sum (see File_TransferReadyCallback). */
extern FileTransferDescriptor *gFile_ReadyTransferDescriptor;
extern u32 *gFile_ReadySourceCursor;

/* File_HandleReadEvent resets these counters before the ready-system callback,
   File_TransferReadyCallback, increments them. Neither view is volatile or forced .data. */
extern u8 gFile_ReadyEventCount;
extern s32 gFile_ReadyDataEventCount;

/* The CdlSetfilter parameter block: File_HandleSetFilterEvent passes it to
   DsCommand(DslSetfilter) and the sector-range path writes the channel byte. */
extern CdlFILTER gFile_FilterParams;

/* The CD callback's state word, switched on and advanced by
 * File_TransferReadyCallback and the rest of file_transfer_runtime.c. It remains a volatile u16 and
 * small-data eligible so the callbacks use the retail halfword accesses. */
extern volatile u16 gFile_wStepState;

/* The CD position buffer handed to DsPacket and CdIntToPos_8007E600.
 * file_transfer_runtime.c defines it. It is four bytes, a CdlLOC (minute,
 * second, sector, mode): the definition must be four bytes for the compiler to
 * word-align it at 0x8009B104, and a char array still decays to the pointer
 * each call passes. */
extern char gFile_TargetLoc[4];

/* The callback the request functions run before starting a primary transfer.
 * File_SetPositionTable installs File_WaitForTransfers and
 * File_InitTransferState clears it; File_RequestAsyncTransfer and
 * File_TryRequestAsyncTransfer call it when it is set, else check the blocked
 * mask. One TU held a u32 view beside an
 * asm("gFile_BeforePrimaryRequestCallback") alias of this type; the pointer
 * is what every use assigns and calls. */
extern void (*gFile_BeforePrimaryRequestCallback)(void);
extern u8 D_8009B0E0;
/* A GsSPRITE-shaped record that File_SetPositionTable fills field by field:
 * 24x24 at (0x120, 0xD0), tpage 0xB, uv (0x00, 0xA0), clut (0x230, 0xFC),
 * neutral grey, attribute 0x08000000 (libgs's GsROTOFF). It was a u8[] reached
 * through offset casts; no other C source names it. */
extern SpritePrim D_800E9DF0;
void File_SetPositionTable(void);

/* The two command callbacks the sound driver hangs on the loader:
 * SD_InitState installs SD_FadeInStreamAudio in gFile_RangeReadCallback and
 * SD_FadeOutStreamAudio in gFile_RangePauseCallback (both `void (void)`,
 * sound_output_transition.h), File_InitTransferState clears both beside its
 * clear of gFile_BeforePrimaryRequestCallback, and File_StepActiveTransfer's
 * transfer step runs gFile_RangePauseCallback from its state 1 and state 6
 * arms and gFile_RangeReadCallback from state 5, each only when non-zero. As
 * with gFile_BeforePrimaryRequestCallback, the loader owns the slot and
 * another unit registers the handler, and the pointer is what every use
 * assigns and calls: one declarer spelled them s32 and only stored 0, another
 * void * and only assigned the two functions. Initial value not read. */
extern void (*gFile_RangeReadCallback)(void);
extern void (*gFile_RangePauseCallback)(void);

/* SD_ConfigureTransferPhase invokes this after its normal secondary payload
 * legs. */
extern void (*gFile_SecondaryPayloadCompleteCallback)(void);

/* Disk-error command reissues; reset only by loader initialization. */
extern s32 gFile_CommandRetryCount;

/* The service-call gate and reentry latch. Before centralization,
 * file_transfer_runtime.c spelled both `extern volatile` while
 * the former file_stream.c (now the start of file_transfer_runtime.c)
 * spelled both plain, and neither declaration was shared.
 *
 * The qualifier is not decoration on the runtime's side, and the reason is
 * instruction scheduling rather than anything being discarded. File_ServiceTransfers
 * stores one word and then immediately tests the other:
 *
 *     gFile_ServiceCallGate = 1;
 *     if (gFile_ServiceReentryLatch != 0) {
 *         return;
 *     }
 *
 * With `volatile` the load of gFile_ServiceReentryLatch cannot move above the store to
 * gFile_ServiceCallGate, so the load-delay slot in front of the branch has nothing to
 * fill it and the assembler leaves a nop:
 *
 *     sh    v0,0(gp)        # gFile_ServiceCallGate = 1
 *     lw    v0,0(gp)        # gFile_ServiceReentryLatch
 *     nop
 *     bnez  v0,...
 *
 * Without it the load hoists above the store and fills that slot itself, the
 * nop goes, and the function ends four bytes earlier -- which is the whole of
 * the size difference, 0x1d07fc against 0x1d0800. The same pinning is already
 * recorded for gFile_dwTransferState in notes/decompilation-workflow.md; it is the
 * ordinary consequence of a volatile access sitting between a store and a
 * dependent load.
 *
 * It is decoration on the other side, which is what lets one declaration
 * serve both. The file-state setup only clears the pair once each inside
 * File_InitTransferState, with no dependent load to hoist, so taking the
 * volatile view costs it nothing and the build is byte for byte. The stronger
 * spelling absorbs the weaker one here.
 */
extern volatile s32 gFile_ServiceReentryLatch;
extern volatile u16 gFile_ServiceCallGate;

/* The descriptor File_ActivateTransfer copies into the primary one.
 *
 * This was deliberately absent until now, on the grounds that four of five
 * declarers spelling it FileTransferDescriptor while file_transfer_runtime.c
 * spelled it `u8 []` was a majority rather than evidence: that file also
 * reaches the loader words through inline assembly, so its spelling might
 * have been load-bearing. The note asked for a measurement rather than a
 * vote, so here is one.
 *
 * Converting the callback use in file_transfer_runtime.c alone, changing
 * nothing else, builds the
 * executable byte for byte. The `u8 []` spelling was not load-bearing, and
 * the one access it guarded -- a whole-record copy written
 * `*(FileTransferDescriptorWords *)gFile_SecondaryTransferDescriptor` --
 * becomes `*(FileTransferDescriptorWords *)&gFile_SecondaryTransferDescriptor`,
 * which is the form that
 * file already used on the line above for the primary descriptor. That copy
 * is File_ActivateTransfer, now in file_transfer_runtime.c. */
extern FileTransferDescriptor gFile_SecondaryTransferDescriptor;

/* Set to 1 by File_InitTransferState once its DsInit retry loop succeeds;
 * nothing in retail reads it. */
extern s32 gFile_DsInitialized;

/* Loader state defined by file_transfer_runtime.c whose roles are not yet
 * named. */
extern u8 D_8009B108;
extern u8 D_8009B110;
extern s32 D_8009B12C;

/* A single word at 0x8009B118, four bytes: c_symbols.ld names gFile_FilterParams
 * immediately after it, so no element can hide inside. File_InitTransferState
 * stores the transfer buffer's address into it; campaign_map_load_package_stage.c
 * passes it to LoadImage2 as (u32 *) and adds 0x800 to it, and the PAL options
 * overlay reads and writes its first byte. It is declared as the address it
 * holds; every consumer, including file_transfer_runtime.c's integer uses
 * (cast at the site), builds byte-identically with this one declaration. */
extern u8 *gFile_TransferBufferBase;

/* The CD path of SU.MRG, the archive the main-menu package is read from. */
extern u8 gFile_szSuMrgPath[];

#endif
