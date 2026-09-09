#include "../types.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../unmatched.h"

extern u8 D_8009B108;
extern s32 D_8009B0E8;
extern s32 D_8009B0FC;
extern u8 D_8009B110;
extern u16 D_8009B124;
extern s32 D_8009B12C;

extern s32 func_8007AFA4(void);
extern s32 CdPosToInt_8007E710(const CdlLOC *);

void File_InitTransferState(s32 value)
{
    D_8009B118 = value;
    D_8009B110 = 0;
    D_8009B108 = 0;
    D_8009B0F4 = 0;
    D_8009B120 = 0;
    D_8009B0F0 = 0;
    D_8009B134 = 0;
    D_8009B112 = 0;
    D_8009B10C = 0;
    D_8009B12C = 0;
    D_8009B124 = 0;
    D_8009B0E8 = 0;
    D_8009B130 = 0;
    while (func_8007AFA4() == 0) {
    }
    D_8009B0FC = 1;
}

void File_GetPosition(s32 *output, const char *path)
{
    DslFILE file;

    while (DsSearchFile(&file, (char *)path) == 0) {
    }
    *output = CdPosToInt_8007E710((const CdlLOC *)&file.pos);
}

void func_80013940(
    FileTransferDescriptor *transfer,
    s32 file_index,
    s32 sector_offset,
    s32 vertical
)
{
    transfer->total_bytes = vertical;
    file_index &= FILE_TRANSFER_FILE_INDEX_MASK;
    if (vertical < 0)
        transfer->total_bytes = -(vertical << FILE_SECTOR_SHIFT);

    if (sector_offset < 0) {
        transfer->file_bytes = 0;
        transfer->absolute_lba = -sector_offset;
    } else {
        register volatile s32 *lbas = gFile_anLba;

        transfer->file_bytes = sector_offset << FILE_SECTOR_SHIFT;
        transfer->absolute_lba = lbas[file_index] + sector_offset;
    }
}

FileTransferDescriptor *File_InitTransferDescriptor(
    FileTransferDescriptor *transfer,
    s32 flags,
    u8 *source,
    s32 sector,
    s32 vertical,
    FileTransferCallback callback,
    s32 field_40,
    s32 length
)
{
    transfer->loader_argument = source;
    func_80013940(transfer, flags, sector, -vertical);
    transfer->done = 1;
    transfer->substate = 0;
    transfer->buffer_index = 0;
    transfer->mode = 0;
    transfer->phase_callback = callback;
    transfer->result = field_40;
    if (length) {
        if (flags & 0x1000000) {
            transfer->direct_destination = length;
        } else {
            transfer->mode = transfer->total_bytes;
            if (length < 0) {
                transfer->done = 1;
                transfer->value_0C = length;
                transfer->value_08 = length;
            } else {
                flags |= 0x10000;
                transfer->done = 2;
                transfer->field_30.h.field_32 = ((u32)length) >> 16;
                transfer->field_30.h.counter = length;
                transfer->w = FILE_TRANSFER_DEFAULT_IMAGE_WORD_WIDTH;
                transfer->h = FILE_TRANSFER_DEFAULT_IMAGE_HEIGHT;
                transfer->value_08 = D_8009B118;
                transfer->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
            }
        }
    }
    transfer->status_flags = flags;
    return transfer;
}

FileTransferDescriptor *func_80013A94(s32 file_index, s32 sector_offset)
{
    FileTransferDescriptor *transfer;

    if (D_8009B0F4 & FILE_TRANSFER_STATE_SECONDARY_PENDING)
        return 0;

    transfer = &gFile_SecondaryTransferDescriptor;
    func_80013940(transfer, file_index & FILE_TRANSFER_FILE_INDEX_MASK,
                  sector_offset, 0);
    transfer->done = 0;
    transfer->status_flags = 0x00100000;
    D_8009B0F4 |= FILE_TRANSFER_STATE_SECONDARY_PENDING;
    return transfer;
}
