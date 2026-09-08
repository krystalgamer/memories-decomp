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
extern volatile u32 D_8009B0F4;
extern volatile u32 D_8009B0F4_abs __attribute__((section(".data")));

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

#endif
