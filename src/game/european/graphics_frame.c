#include "../../types.h"
#define GRAPHICS_FRAME_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x8009C418-0x800EAD50 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 D_8009C418_sles_03947[0x4E938];
#define D_8009B318_IN_DATA
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
s32 D_8009B0D8;
u16 gGraphics_wFlags;
u16 D_8009C022_sles_03947;
volatile s32 gMain_dwFrameCounter;
u16 D_8009C028_sles_03947;
u8 D_8009C02A_sles_03947;
u8 D_8009C02B_sles_03947;
s32 D_8009B0A4;
void (*gMain_pfnExtraFrameService)(void);
u8 gGraphics_bExtraFrameStep;
u8 gGraphics_bActiveBuffer;
u8 gGraphics_bDither;
u8 D_8009C037_sles_03947;

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../graphics_frame.c"
