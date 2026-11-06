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
static u32 D_8009C428_sles_03947[4];
extern GsOT *gGraphics_apOrderingTables[];
extern int gMain_RecoveryJmpBuf[];
extern void (*gMain_apFrameServiceCallbacks[])(void);
extern DISPENV gGraphics_DispEnv;
static u8 D_8009C4A0_sles_03947[0x30];
extern u8 gGraphics_abPacketBuffers[];
extern struct GraphicsFrameBuffer gGraphics_aFrameBuffers[];
#define GRAPHICS_FRAME_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009BFE8-0x8009C038 (#2602); its
 * layout differs from the North American block the shared source defines. */
u32 D_8009BFE8_sles_03947;
u32 D_8009BFEC_sles_03947;
s32 D_8009B0B0;
struct GraphicsFrameBuffer *gGraphics_pActiveFrameBuffer;
u32 D_8009BFF8_sles_03947;
volatile s32 gSaveData_dwVBlankCounter;
s32 D_8009B0BC;
volatile u8 gGraphics_bSyncVBlankTarget;
u8 gMain_bVBlankSoundPending;
u16 D_8009C006_sles_03947;
volatile s32 gGraphics_dwSyncVBlankCounter;
volatile s32 gGraphics_dwSyncedFrameCount;
u8 gGraphics_abOrderingTableLengths[4];
u8 gGraphics_bClearBackground;
u8 D_8009B0D1;
u16 D_8009C016_sles_03947;
s32 D_8009B0D4;
s32 gGraphics_nBaseFrameStep;
u16 gGraphics_wFlags;
u16 D_8009C022_sles_03947;
volatile s32 gMain_dwFrameCounter;
u16 D_8009C028_sles_03947;
u8 gGraphics_bSkipDisplaySwap;
u8 gText_bLanguageIndex;
s32 D_8009B0A4;
void (*gMain_pfnExtraFrameService)(void);
u8 gGraphics_bExtraFrameStep;
u8 gGraphics_bActiveBuffer;
u8 gGraphics_bDither;
u8 D_8009C037_sles_03947;

#include "../main.c"
