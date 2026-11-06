#include "../../types.h"

/* SLES-03947 build of src/game/main.c. */
#define VERSION_EUROPE
/* The European display height. */
#define GRAPHICS_DEFAULT_HEIGHT 256
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#define GRAPHICS_FRAME_STORAGE_ORDER
/* This build lays out the shared source's zero-initialized storage at
 * 0x8009C418-0x800EAD50 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern RECT gGraphics_aImageTransferRects[];
static PADDING_VARIABLE(16);
extern GsOT *gGraphics_apOrderingTables[];
extern int gMain_RecoveryJmpBuf[];
extern void (*gMain_apFrameServiceCallbacks[])(void);
extern DISPENV gGraphics_DispEnv;
static PADDING_VARIABLE(0x30);
extern u8 gGraphics_abPacketBuffers[];
extern struct GraphicsFrameBuffer gGraphics_aFrameBuffers[];
#define GRAPHICS_FRAME_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009BFE8-0x8009C038 (#2602); its
 * layout differs from the North American block the shared source defines. */
static PADDING_VARIABLE(4);
static PADDING_VARIABLE(4);
s32 D_8009B0B0;
struct GraphicsFrameBuffer *gGraphics_pActiveFrameBuffer;
static PADDING_VARIABLE(4);
volatile s32 gSaveData_dwVBlankCounter;
s32 D_8009B0BC;
volatile u8 gGraphics_bSyncVBlankTarget;
u8 gMain_bVBlankSoundPending;
static PADDING_VARIABLE(2);
volatile s32 gGraphics_dwSyncVBlankCounter;
volatile s32 gGraphics_dwSyncedFrameCount;
u8 gGraphics_abOrderingTableLengths[4];
u8 gGraphics_bClearBackground;
u8 D_8009B0D1;
static PADDING_VARIABLE(2);
s32 D_8009B0D4;
s32 gGraphics_nBaseFrameStep;
u16 gGraphics_wFlags;
static PADDING_VARIABLE(2);
volatile s32 gMain_dwFrameCounter;
static PADDING_VARIABLE(2);
u8 gGraphics_bSkipDisplaySwap;
u8 gText_bLanguageIndex;
s32 D_8009B0A4;
void (*gMain_pfnExtraFrameService)(void);
u8 gGraphics_bExtraFrameStep;
u8 gGraphics_bActiveBuffer;
u8 gGraphics_bDither;
static PADDING_VARIABLE(1);

#include "../main.c"
