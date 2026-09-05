#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"

typedef struct {
    u8 pad00[4];
    u16 width;
    u16 height;
    s32 field_08;
    s32 field_0C;
    volatile s32 field_10;
    s32 field_14;
    s32 field_18;
    s32 field_1C;
    s32 field_20;
    s32 field_24;
    u8 pad28[4];
    u32 flags;
    u16 x;
    u16 y;
    s32 field_34;
    u8 pad38[8];
    s32 field_40;
    u16 field_44;
    u8 state;
    u8 substate;
} FileTransfer;

extern u8 D_8009B108;
extern s32 D_8009B0E8;
extern s32 D_8009B0F0;
extern u32 D_8009B0F4;
extern s32 D_8009B0FC;
extern s32 D_8009B10C;
extern u8 D_8009B110;
extern u16 D_8009B112;
extern s32 D_8009B118;
extern s32 D_8009B120;
extern u16 D_8009B124;
extern s32 D_8009B12C;
extern s32 D_8009B130;
extern s32 D_8009B134;
extern FileTransfer D_800E9E18;
extern s32 gFile_anLba[];

extern s32 func_8007AFA4(void);
extern s32 func_8007D3F0(DslFILE *, const char *);
extern s32 func_8007E710(const CdlLOC *);

void func_80013898(s32 value)
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

    while (func_8007D3F0(&file, path) == 0) {
    }
    *output = func_8007E710((const CdlLOC *)&file.pos);
}

void func_80013940(
    FileTransfer *transfer,
    s32 file_index,
    s32 sector_offset,
    s32 vertical
)
{
    transfer->field_10 = vertical;
    file_index &= 0xF;
    if (vertical < 0)
        transfer->field_10 = -(vertical << 11);

    if (sector_offset < 0) {
        transfer->field_14 = 0;
        transfer->field_24 = -sector_offset;
    } else {
        register volatile s32 *lbas = gFile_anLba;

        transfer->field_14 = sector_offset << 11;
        transfer->field_24 = lbas[file_index] + sector_offset;
    }
}

FileTransfer *File_InitTransferDescriptor(
    FileTransfer *transfer,
    s32 flags,
    s32 field_18,
    s32 sector,
    s32 vertical,
    s32 field_20,
    s32 field_40,
    s32 length
)
{
    transfer->field_18 = field_18;
    func_80013940(transfer, flags, sector, -vertical);
    transfer->state = 1;
    transfer->substate = 0;
    transfer->field_44 = 0;
    transfer->field_1C = 0;
    transfer->field_20 = field_20;
    transfer->field_40 = field_40;
    if (length) {
        if (flags & 0x1000000) {
            transfer->field_34 = length;
        } else {
            transfer->field_1C = transfer->field_10;
            if (length < 0) {
                transfer->state = 1;
                transfer->field_0C = length;
                transfer->field_08 = length;
            } else {
                flags |= 0x10000;
                transfer->state = 2;
                transfer->y = ((u32)length) >> 16;
                transfer->x = length;
                transfer->width = 0x40;
                transfer->height = 0x10;
                transfer->field_08 = D_8009B118;
                transfer->field_0C = D_8009B118 + 0x800;
            }
        }
    }
    transfer->flags = flags;
    return transfer;
}

FileTransfer *func_80013A94(s32 file_index, s32 sector_offset)
{
    FileTransfer *transfer;

    if (D_8009B0F4 & 0x20)
        return 0;

    transfer = &D_800E9E18;
    func_80013940(transfer, file_index & 0xF, sector_offset, 0);
    transfer->state = 0;
    transfer->flags = 0x00100000;
    D_8009B0F4 |= 0x20;
    return transfer;
}
