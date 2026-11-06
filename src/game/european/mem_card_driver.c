#include "../../types.h"
#define MEM_CARD_DRIVER_STORAGE_ORDER
/* This build lays out the shared source's zero-initialized storage at
 * 0x800F37F0-0x800F3A88 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern long gMemCard_aIOEventHandles[];
extern long gMemCard_aHwIOEventHandles[];
extern char gMemCard_szRequestPath[];
extern struct DIRENTRY gMemCard_aDirEntries[];
#define MEM_CARD_DRIVER_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C390-0x8009C3B8 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 gMemCard_bChannel;
u8 gMemCard_bRetries;
static PADDING_VARIABLE(2);
s32 gMemCard_nDirEntries;
u8 gMemCard_bLoadStep;
static PADDING_VARIABLE(1);
u16 gMemCard_wRequestOffset;
volatile s32 gMemCard_nIOResult;
struct DIRENTRY *gMemCard_pDirEntries;
s32 gMemCard_pRequestBuf;
u16 gMemCard_wRequestSize;
s8 gMemCard_bRequest;
u8 gMemCard_bDirFlags;
u8 gMemCard_bTransferRetriesRemaining;
u8 gMemCard_bRequestStep;
static PADDING_VARIABLE(2);
static PADDING_VARIABLE(4);
s32 gMemCard_nFreeBlocks;

/* SLES-03947 build of src/game/mem_card_driver.c. The US source is included
 * unchanged; config/sles_03947/symbols.txt binds its globals. */

#include "../mem_card_driver.c"
