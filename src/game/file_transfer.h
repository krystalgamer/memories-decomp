#ifndef MEMORIES_DECOMP_FILE_TRANSFER_H
#define MEMORIES_DECOMP_FILE_TRANSFER_H

#include "../ygo_types.h"
#include "file_constants.h"

#define FILE_TRANSFER_STATE_PRIMARY_ACTIVE 0x10
#define FILE_TRANSFER_STATE_SECONDARY_PENDING 0x20
#define FILE_TRANSFER_REQUEST_BLOCKED_MASK 0x02000030
#define FILE_TRANSFER_DESCRIPTOR_WORD_COUNT 18

typedef char FileTransfer_default_image_must_fill_sector[
    FILE_TRANSFER_DEFAULT_IMAGE_WORD_WIDTH * FILE_TRANSFER_DEFAULT_IMAGE_HEIGHT *
        sizeof(u16) == FILE_SECTOR_SIZE ? 1 : -1
];

void File_InitTransferState(s32);
void File_GetPosition(s32 *, const char *);
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
void File_ActivateTransfer(void);
void File_WaitForTransfers(void);

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

extern volatile u32 D_8009B0F4;
extern volatile u32 D_8009B0F4_abs __attribute__((section(".data")));

#endif
