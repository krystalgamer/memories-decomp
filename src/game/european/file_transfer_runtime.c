#include "../../types.h"
#define FILE_TRANSFER_RUNTIME_REGIONAL_STORAGE
/* This build lays out the shared source's zero-initialized storage at
 * 0x800EAD78-0x800EAE28 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern s32 gFile_anLba[];
extern struct FileTransferDescriptor gFile_PrimaryTransferDescriptor;
extern struct FileTransferDescriptor gFile_SecondaryTransferDescriptor;
#define FILE_TRANSFER_RUNTIME_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C040-0x8009C098 (#2602); its
 * layout differs from the North American block the shared source defines. */
volatile u32 gFile_dwTransferState;
u32 D_8009C044_sles_03947;
u32 D_8009C048_sles_03947;
u32 D_8009C04C_sles_03947;
u32 D_8009C050_sles_03947;
u32 D_8009C054_sles_03947;
u32 D_8009C058_sles_03947;
u32 D_8009C05C_sles_03947;
u32 D_8009C060_sles_03947;
u32 gFile_dwCancelRequestState;
u32 D_8009C068_sles_03947;
u32 D_8009C06C_sles_03947;
u32 D_8009C070_sles_03947;
u32 D_8009C074_sles_03947;
u32 D_8009C078_sles_03947;
u32 D_8009C07C_sles_03947;
u32 D_8009C080_sles_03947;
u32 D_8009C084_sles_03947;
u32 D_8009C088_sles_03947;
u32 D_8009C08C_sles_03947;
s32 gFile_TransferBufferBase;
u32 D_8009C094_sles_03947;

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../file_transfer_runtime.c"
