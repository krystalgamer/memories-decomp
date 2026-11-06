#include "../../types.h"
#include "../../psyq/libcd.h"
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
volatile u16 gFile_wStepState;
CdlFILTER gFile_FilterParams;
u8 D_8009B108;
void (*gFile_SecondaryPayloadCompleteCallback)(void);
u8 D_8009B110;
s32 gFile_ReadyDataEventCount;
volatile u16 gFile_wTransferStepFlags;
s32 gFile_CommandRetryCount;
u32 gFile_dwCancelRequestState;
void (*gFile_RangePauseCallback)(void);
s32 D_8009B12C;
volatile u16 gFile_ServiceCallGate;
volatile s32 gFile_ServiceReentryLatch;
u16 gFile_RangeWaitCountdown;
void (*gFile_RangeReadCallback)(void);
s32 D_8009B0FC;
u32 *gFile_ReadySourceCursor;
void (*gFile_BeforePrimaryRequestCallback)(void);
u8 gFile_ReadyEventCount;
u8 *gFile_TransferBufferBase;
char gFile_TargetLoc[4];

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../file_transfer_runtime.c"
