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

#endif
