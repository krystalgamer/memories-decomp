#ifndef MEMORIES_DECOMP_FILE_TRANSFER_H
#define MEMORIES_DECOMP_FILE_TRANSFER_H

#include "../ygo_types.h"
#include "file_constants.h"

#define FILE_TRANSFER_STATE_PRIMARY_ACTIVE 0x10
#define FILE_TRANSFER_STATE_SECONDARY_PENDING 0x20
#define FILE_TRANSFER_REQUEST_BLOCKED_MASK 0x02000030
#define FILE_TRANSFER_DESCRIPTOR_WORD_COUNT 18

/* One entry of the two-slot request table at D_801D4200.

   func_80014C40 stages the caller's request into slot 1 with a whole-record
   copy, file_cd_transfer.c's func_800141A8 promotes slot 1 into slot 0 the
   same way once the drive is ready, and func_80014B30 then programs the
   transfer descriptor out of slot 0. Two independent 0x20-byte copies at that
   stride are what fix the size; func_80014B30 names the four words.

   func_80014C40 reads the same record before it copies it, still as `u8 *`,
   and evidences four more offsets that way: words at 0x00 and 0x04, a
   halfword at 0x1C and bytes at 0x1E and 0x1F. Those stay padding here
   because nothing reaches them through the type yet. */
typedef struct {
    u8 pad_00[0xC];
    s32 field_0C;
    s32 field_10;
    s32 field_14;
    s32 field_18;
    u8 pad_1C[4];
} FileRequestSlot;

typedef char FileRequestSlot_size_must_be_0x20[
    sizeof(FileRequestSlot) == 0x20 ? 1 : -1
];

/* A FileTransferDescriptor's worth of words, for the one place that copies a
   whole descriptor: func_800141A8 overwrites the primary descriptor with the
   secondary one.

   This is a block-move spelling, not a second description of the record --
   the element type is what sets the alignment and therefore the move width,
   so it is deliberately `s32` and deliberately not interchangeable with
   FileTransferDescriptor itself, which contains halfword members. The assert
   below is what ties the two together. */
typedef struct {
    s32 value[FILE_TRANSFER_DESCRIPTOR_WORD_COUNT];
} FileTransferDescriptorWords;

typedef char FileTransferDescriptorWords_size_must_match_descriptor[
    sizeof(FileTransferDescriptorWords) == sizeof(FileTransferDescriptor) ? 1 : -1
];

typedef char FileTransfer_default_image_must_fill_sector[
    FILE_TRANSFER_DEFAULT_IMAGE_WORD_WIDTH * FILE_TRANSFER_DEFAULT_IMAGE_HEIGHT *
        sizeof(u16) == FILE_SECTOR_SIZE ? 1 : -1
];

void File_InitTransferState(s32);
void File_GetPosition(s32 *, const char *);
void func_80013940(
    FileTransferDescriptor *, s32 file_index, s32 sector_offset, s32 vertical
);
FileTransferDescriptor *File_RequestAsyncTransfer(
    s32, u8 *, s32, s32, FileTransferCallback, s32, s32
);
FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32, u8 *, s32, s32, FileTransferCallback, s32, s32
);
FileTransferDescriptor *File_RequestSecondaryAsyncTransfer(
    s32, u8 *, s32, s32, FileTransferCallback, s32, s32
);
FileTransferDescriptor *File_RequestSecondaryRangeTransfer(
    s32, s32, s32, s32
);
FileTransferDescriptor *File_InitTransferDescriptor(
    FileTransferDescriptor *, s32, u8 *, s32, s32, FileTransferCallback, s32,
    s32
);
FileTransferDescriptor *func_80013A94(s32 file_index, s32 sector_offset);
void func_8001455C(void);
void func_80014A5C(s32 arg0);
void func_80014B30(FileTransferDescriptor *descriptor, s32 mode);
s32 func_80014C40(u8 *request, u8 *source);
void File_ActivateTransfer(void);
void File_WaitForTransfers(void);
void File_RequestMainMenuPackage(void);
void File_RequestNameEntryPackage(void);
void File_RequestPasswordPackage(void);
void File_RequestEgyptOverworldPackage(void);
void func_8003C120(FileTransferDescriptor *descriptor, s32 mode);
void func_8003C328(FileTransferDescriptor *descriptor, s32 mode);
void func_80043328(FileTransferDescriptor *descriptor, s32 mode);
void func_800434F4(FileTransferDescriptor *descriptor, s32 mode);
void func_8005B64C(FileTransferDescriptor *descriptor, s32 mode);

/* Another wrapper of the same shape as the four above: it asks for sector
   0x2157, 0x32 sectors, with func_8003C328 as the completion callback, then
   waits. It keeps its address-based name because which package that sector
   holds is not established here -- the four named ones use FILE_WA_*
   constants, and this one still spells the sector as a literal. */
void func_8003C498(void);

/* Puts the loader's control halfword at 0x8009B112 into mode 2, clearing the
   other bits of its low field. Every caller reaches it through the same
   guard: only when a primary transfer is active and it is a sector-range
   one. */
void func_80015010(void);

/* That guard on its own, with no other work. File_WaitForTransfers spins on
   it while a transfer is outstanding and no secondary request is pending. */
void func_80015038(void);

/* Raises the secondary-request word to 0x80 if anything is still in flight,
   running func_80015038's guard first. The pad handler in func_800307B8
   calls it to abandon the wait. */
void func_80014FA4(void);

/* The resident loader's request-and-state word at 0x8009B0F4.
 *
 * Every File_* entry point and every CD/DS sector callback tests or updates
 * it, and the FILE_TRANSFER_STATE_*, FILE_TRANSFER_FLAG_SECTOR_RANGE and
 * FILE_TRANSFER_REQUEST_BLOCKED_MASK bits declared above are its bits. It is
 * only ever read and written whole, and only ever through bit masks. Nothing
 * indexes it, so the `D_8009B0F4[0]` spellings this header replaces were an
 * addressing device rather than evidence of an array.
 *
 * `volatile` is load-bearing on both names, measured rather than assumed:
 * dropping it from the plain name builds a 0x1D0668-byte executable and
 * dropping it from `D_8009B0F4_abs` builds a 0x1D071C-byte one, against the
 * retail 0x1D0800.
 *
 * Two names, one word. The retail image reaches this address both ways. The
 * loader unit still held as assembly in `text_004428.s` uses
 * `%gp_rel(D_8009B0F4)($gp)` seven times, while six other generated assembly
 * files use `lui %hi` / `%lo` fifty-nine times. One declaration cannot
 * produce both inside a -G8 translation unit, because the form follows from
 * whether the symbol is small-data eligible. The plain name is, so the
 * assembler resolves it gp-relative; `D_8009B0F4_abs` carries
 * `section(".data")` so it is not, and `c_symbols.ld` ties that name to the
 * same address. Which of the two a unit needs is a property of its compiler
 * profile, not of the word.
 */
/* The word at 0x800101D8, which holds the address 0x80168000.
 *
 * It reaches the same two descriptor fields that take a buffer address in the
 * case immediately above it -- func_8003BD14's case 2 writes
 * `value_08 = (s32)D_801A8000`, and its case 3 writes this word into the same
 * two fields -- so the word is a stored address and is declared as one.  Every read is a single load of that word; the array and
 * scalar spellings this replaces reached it as `*(s32 *)(D_800101D8)` and as
 * a plain read, which are the same load.
 *
 * D_800101D8_IN_DATA is a codegen input, measured on each unit separately:
 * put either func_8003BF00.c or func_8003B808.c on the plain declaration and
 * the link fails on that object alone with `relocation truncated to fit:
 * R_MIPS_GPREL16 against D_800101D8`, because the unit reaches the symbol
 * gp-relatively and 0x800101D8 is out of range of $gp.  The attribute takes it
 * out of small data for those two; the other three do not need it.
 */
#ifdef D_800101D8_IN_DATA
extern u8 *D_800101D8 __attribute__((section(".data")));
#else
extern u8 *D_800101D8;
#endif

/* The per-file starting sector table the transfer setup indexes by file
   number. Four sources reached it, all with this identical declaration and
   none of them defining it, so it is still generated data. */
extern s32 gFile_anLba[];

extern volatile u32 D_8009B0F4;
extern volatile u32 D_8009B0F4_abs __attribute__((section(".data")));

/* The transfer-step flag word. func_8001455C sets and clears every bit of
 * it through the streaming retry state machine and reloads it after each
 * store; File_ActivateTransfer ORs in bit 0; the two readers outside this
 * family test bit 0x4000, the transfer-complete flag. Same two forms as
 * D_8009B0F4 above: five units reach it gp-relative, and two --
 * func_80037B40 and func_800257A0 -- read it through a %hi/%lo pair into
 * the load's own register (retail's `lui $v0` / `lhu $v0,%lo(...)($v0)`),
 * which is the bare form; they take the _abs name. Measured per unit: with
 * either of the two on the plain name the executable links 8 bytes short
 * (0x1d07f8 against 0x1d0800), the two pairs collapsing to two gp-relative
 * loads. volatile stays on the shared form, where it keeps func_8001455C's
 * back-to-back read-modify-writes from folding; the _abs twin is not
 * volatile, and that is measured -- the two readers build byte-identical
 * without it, as D_8009B134_abs does. */
extern volatile u16 D_8009B112;
extern u16 D_8009B112_abs __attribute__((section(".data")));

/* The loader's secondary-request word at 0x8009B134, the other half of the
 * `(D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134` predicate
 * that eighteen units use to ask whether a transfer is still in flight.
 * `func_80014FA4` and `func_800144B8` raise it to 0x80, the frame pump
 * `func_80014A5C` latches 0x40 into it once and otherwise clears it, and
 * `File_InitTransferState` zeroes it with the rest of the loader block.
 *
 * It takes the same two addressing views as D_8009B0F4, for the same reason,
 * but it is deliberately *not* volatile. That is a measured difference
 * between the two neighbouring words, not an oversight: declaring it
 * volatile makes `func_80014A5C` re-load it for the 0x40 test and again for
 * the `|=`, where retail keeps one load live in `$3` across all three uses.
 */
extern u32 D_8009B134;
extern u32 D_8009B134_abs __attribute__((section(".data")));

/* 0x801DC000, gLibrary_aCardArtRecord in config/slus_01411/symbols.txt:372.
 * File_SetPositionTable hands its address to File_InitTransferState
 * (file_set_position_table.c:18), which stores it into D_8009B118
 * (file_stream.c:20). The two memory-card dialogs also reach it, always by
 * address: func_8003E854 (mem_card_create_state.c:210 and :237) and
 * func_8003EED0 (mem_card_save_state.c:93 and :118) pass it to
 * MemCardReadFile as the destination of a read whose last argument is 0x480,
 * and compare it as a SaveDataState at mem_card_create_state.c:228 and
 * mem_card_save_state.c:110. Every retail access is an address-take
 * (func_800136E4.s:5-6, func_8003E854.s:289-290 and :319-320,
 * func_8003EED0.s:127-128, :155 and :158), so the listings say nothing about
 * the object's width; D_801DD000, declared next, is named 0x1000 bytes
 * higher and D_801DA000 0x2000 lower. The three units used to declare it
 * privately, all as `u8 []`. */
extern u8 gLibrary_aCardArtRecord[];

/* Shared staging/upload buffer used by the resident transfer-phase callbacks.
 * Every C consumer treats it as an unsized byte buffer, either publishing its
 * address through a FileTransferDescriptor or passing it to LoadImage2. */
extern u8 D_801DD000[];

/* The primary transfer descriptor. Four sources in this family reach it as a
 * FileTransferDescriptor, agreeing on the spelling, and none defines it. */
extern FileTransferDescriptor gFile_PrimaryTransferDescriptor;

/* The CD callback's state word, switched on by file_transfer_control.c and
 * advanced by the callbacks in file_cd_transfer.c.
 *
 * Both declarers already spell it `volatile u16` and it stays that way. It
 * also has to stay small-data eligible: file_cd_transfer.c stores to it from
 * inline assembly written as `sh $4, %gp_rel(D_8009B100)($28)`, which names
 * the symbol and assumes $gp addressing. A two-byte scalar is eligible under
 * -G8, so this declaration keeps that true; a `.data` arm here would break
 * that store rather than merely change a load. */
extern volatile u16 D_8009B100;

/* The CD position buffer handed to func_8007B468 and CdIntToPos_8007E600.
 * Both declarers write `char D_8009B104[1]`, and the one-element spelling is
 * kept exactly: it is an addressing device that makes the name decay to a
 * pointer at each call, not a claim that one byte is all that is there. */
extern char D_8009B104[1];

/* The callback the request functions run before starting a primary transfer.
 * File_SetPositionTable installs File_WaitForTransfers and
 * File_InitTransferState clears it; File_RequestAsyncTransfer and
 * File_TryRequestAsyncTransfer call it when it is set, else check the
 * blocked mask. Retail: sw %lo through $at in File_SetPositionTable (the
 * as -G2 profile, where a four-byte object is non-small whatever its type),
 * gp-relative sw and two lw elsewhere. One TU held a u32 view beside an
 * asm("D_8009B10C") alias of this type; the pointer is what every use
 * assigns and calls. */
extern void (*D_8009B10C)(void);

/* The two command callbacks the sound driver hangs on the loader: SD_InitState
 * installs func_8004666C in D_8009B0F0 and func_800466C8 in D_8009B120 (both
 * `void (void)`, sound_output_transition.h), File_InitTransferState clears
 * both beside its clear of D_8009B10C, and func_8001455C's transfer step runs
 * D_8009B120 from its state 1 and state 6 arms and D_8009B0F0 from state 5,
 * each only when non-zero. As with D_8009B10C, the loader owns the slot and
 * another unit registers the handler, and the pointer is what every use
 * assigns and calls: one declarer spelled them s32 and only stored 0, another
 * void * and only assigned the two functions. Retail: gp-relative sw of zero
 * in File_InitTransferState and gp-relative lw in func_8001455C, but sw %lo
 * through $at in SD_InitState, whose unit defines the .data arms below for
 * that. Initial value not read. */
#ifdef D_8009B0F0_IN_DATA
extern void (*D_8009B0F0)(void) __attribute__((section(".data")));
#else
extern void (*D_8009B0F0)(void);
#endif
#ifdef D_8009B120_IN_DATA
extern void (*D_8009B120)(void) __attribute__((section(".data")));
#else
extern void (*D_8009B120)(void);
#endif

/* A counter the CD and stream paths bump at each step they complete. */
extern s32 D_8009B130;

/* The descriptor File_ActivateTransfer copies into the primary one.
 *
 * This was deliberately absent until now, on the grounds that four of five
 * declarers spelling it FileTransferDescriptor while file_cd_transfer.c
 * spelled it `u8 []` was a majority rather than evidence: that file also
 * reaches the loader words through inline assembly, so its spelling might
 * have been load-bearing. The note asked for a measurement rather than a
 * vote, so here is one.
 *
 * Converting file_cd_transfer.c alone, changing nothing else, builds the
 * executable byte for byte. The `u8 []` spelling was not load-bearing, and
 * the one access it guarded -- a whole-record copy written
 * `*(FileTransferDescriptorWords *)gFile_SecondaryTransferDescriptor` --
 * becomes `*(FileTransferDescriptorWords *)&gFile_SecondaryTransferDescriptor`,
 * which is the form that
 * file already used on the line above for the primary descriptor. */
extern FileTransferDescriptor gFile_SecondaryTransferDescriptor;

#endif
