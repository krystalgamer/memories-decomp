#include "../types.h"
#include "card_list_setup.h"
#include "fade.h"
#include "graphics_frame.h"
#include "graphics_frame_controls.h"
#include "main_mode_state.h"
#include "resident_bss.h"

/* Resident .sbss storage from 0x8009B090 to 0x8009B146, previously part of the
   generated zero-filled BSS image (#2602). Unnamed bytes keep address names. */
u32 bss_D_8009B090[2] __asm__("D_8009B090") __attribute__((section(".sbss"))) = {0};
u16 gGraphics_wFlags __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B09A __asm__("D_8009B09A") __attribute__((section(".sbss"))) = 0;
volatile s32 gMain_dwFrameCounter __attribute__((section(".sbss"))) = 0;
u8 bss_gGraphics_abOrderingTableLengths __asm__("gGraphics_abOrderingTableLengths") __attribute__((section(".sbss"))) = 0;
u8 bss_gGraphics_bOrderingTable1Length __asm__("gGraphics_bOrderingTable1Length") __attribute__((section(".sbss"))) = 0;
u8 bss_gGraphics_bOrderingTable2Length __asm__("gGraphics_bOrderingTable2Length") __attribute__((section(".sbss"))) = 0;
u8 bss_gGraphics_bOrderingTable3Length __asm__("gGraphics_bOrderingTable3Length") __attribute__((section(".sbss"))) = 0;
s32 D_8009B0A4 __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bSkipDisplaySwap __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B0A9 __asm__("D_8009B0A9") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B0AA __asm__("D_8009B0AA") __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bActiveBuffer __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bDither __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B0AE __asm__("D_8009B0AE") __attribute__((section(".sbss"))) = 0;
s32 D_8009B0B0 __attribute__((section(".sbss"))) = 0;
GraphicsFrameBuffer * gGraphics_pActiveFrameBuffer __attribute__((section(".sbss"))) = 0;
u32 bss_gMain_pfnExtraFrameService __asm__("gMain_pfnExtraFrameService") __attribute__((section(".sbss"))) = 0;
s32 D_8009B0BC __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bSyncVBlankTarget __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bExtraFrameStep __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B0C2 __asm__("D_8009B0C2") __attribute__((section(".sbss"))) = 0;
u8 gMain_bVBlankSoundPending __attribute__((section(".sbss"))) = 0;
volatile s32 gSaveData_dwVBlankCounter __attribute__((section(".sbss"))) = 0;
volatile s32 gGraphics_dwSyncVBlankCounter __attribute__((section(".sbss"))) = 0;
volatile s32 gGraphics_dwSyncedFrameCount __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bClearBackground __attribute__((section(".sbss"))) = 0;
u8 D_8009B0D1 __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B0D2 __asm__("D_8009B0D2") __attribute__((section(".sbss"))) = 0;
s32 D_8009B0D4 __attribute__((section(".sbss"))) = 0;
s32 D_8009B0D8 __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B0DC __asm__("D_8009B0DC") __attribute__((section(".sbss"))) = 0;
u8 D_8009B0E0 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B0E1 __asm__("D_8009B0E1") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B0E2 __asm__("D_8009B0E2") __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B0E4 __asm__("D_8009B0E4") __attribute__((section(".sbss"))) = 0;
volatile s32 gFile_ServiceReentryLatch __attribute__((section(".sbss"))) = 0;
u16 gFile_RangeWaitCountdown __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B0EE __asm__("D_8009B0EE") __attribute__((section(".sbss"))) = 0;
u32 bss_gFile_RangeReadCallback __asm__("gFile_RangeReadCallback") __attribute__((section(".sbss"))) = 0;
volatile u32 D_8009B0F4 __attribute__((section(".sbss"))) = 0;
u32 * gFile_ReadySourceCursor __attribute__((section(".sbss"))) = 0;
s32 D_8009B0FC __attribute__((section(".sbss"))) = 0;
volatile u16 gFile_wStepState __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B102 __asm__("D_8009B102") __attribute__((section(".sbss"))) = 0;
char gFile_TargetLoc[1] __attribute__((section(".sbss"))) = {0};
u8 bss_pad_8009B105 __asm__("D_8009B105") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B106 __asm__("D_8009B106") __attribute__((section(".sbss"))) = 0;
u8 D_8009B108 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B109 __asm__("D_8009B109") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B10A __asm__("D_8009B10A") __attribute__((section(".sbss"))) = 0;
u32 bss_gFile_BeforePrimaryRequestCallback __asm__("gFile_BeforePrimaryRequestCallback") __attribute__((section(".sbss"))) = 0;
u8 D_8009B110 __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B111 __asm__("D_8009B111") __attribute__((section(".sbss"))) = 0;
volatile u16 gFile_wTransferStepFlags __attribute__((section(".sbss"))) = 0;
u8 gFile_ReadyEventCount __attribute__((section(".sbss"))) = 0;
u8 bss_pad_8009B115 __asm__("D_8009B115") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B116 __asm__("D_8009B116") __attribute__((section(".sbss"))) = 0;
s32 D_8009B118 __attribute__((section(".sbss"))) = 0;
char gFile_FilterParams[1] __attribute__((section(".sbss"))) = {0};
u8 bss_D_8009B11D __asm__("D_8009B11D") __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B11E __asm__("D_8009B11E") __attribute__((section(".sbss"))) = 0;
u32 bss_gFile_RangePauseCallback __asm__("gFile_RangePauseCallback") __attribute__((section(".sbss"))) = 0;
volatile u16 gFile_ServiceCallGate __attribute__((section(".sbss"))) = 0;
u16 bss_pad_8009B126 __asm__("D_8009B126") __attribute__((section(".sbss"))) = 0;
u32 bss_gFile_SecondaryPayloadCompleteCallback __asm__("gFile_SecondaryPayloadCompleteCallback") __attribute__((section(".sbss"))) = 0;
s32 D_8009B12C __attribute__((section(".sbss"))) = 0;
s32 gFile_CommandRetryCount __attribute__((section(".sbss"))) = 0;
u32 D_8009B134 __attribute__((section(".sbss"))) = 0;
s32 gFile_ReadyDataEventCount __attribute__((section(".sbss"))) = 0;
u32 bss_pad_8009B13C __asm__("D_8009B13C") __attribute__((section(".sbss"))) = 0;
u8 D_8009B140 __attribute__((section(".sbss"))) = 0;
u8 gFade_bOrderingTableMode __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bClearBlue __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bClearGreen __attribute__((section(".sbss"))) = 0;
u8 gGraphics_bClearRed __attribute__((section(".sbss"))) = 0;
u8 D_8009B145 __attribute__((section(".sbss"))) = 0;
