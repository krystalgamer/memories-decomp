#define D_8009B318_IN_DATA
#include "../types.h"

#ifndef GRAPHICS_FRAME_REGIONAL_SMALL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B090-0x8009B0E0, defined in
 * address order ahead of the headers that declare it (#2602). The startup,
 * frame and service units sit beside this one in text and declare these
 * through their headers. gGraphics_bOrderingTable1Length..3Length are bytes of
 * gGraphics_abOrderingTableLengths named by link_symbols.ld. */
static u32 D_8009B090;
static u32 D_8009B094;
u16 gGraphics_wFlags;
volatile s32 gMain_dwFrameCounter;
u8 gGraphics_abOrderingTableLengths[4];
s32 D_8009B0A4;
u8 gGraphics_bSkipDisplaySwap;
static u8 D_8009B0A9;
static u16 D_8009B0AA;
u8 gGraphics_bActiveBuffer;
u8 gGraphics_bDither;
s32 D_8009B0B0;
struct GraphicsFrameBuffer *gGraphics_pActiveFrameBuffer;
void (*gMain_pfnExtraFrameService)(void);
s32 D_8009B0BC;
volatile u8 gGraphics_bSyncVBlankTarget;
u8 gGraphics_bExtraFrameStep;
static u8 D_8009B0C2;
u8 gMain_bVBlankSoundPending;
volatile s32 gSaveData_dwVBlankCounter;
volatile s32 gGraphics_dwSyncVBlankCounter;
volatile s32 gGraphics_dwSyncedFrameCount;
u8 gGraphics_bClearBackground;
u8 D_8009B0D1;
s32 D_8009B0D4;
s32 D_8009B0D8;
static u32 D_8009B0DC;
#else
extern u16 gGraphics_wFlags;
extern volatile s32 gMain_dwFrameCounter;
extern u8 gGraphics_abOrderingTableLengths[4];
extern s32 D_8009B0A4;
extern u8 gGraphics_bSkipDisplaySwap;
extern u8 gGraphics_bActiveBuffer;
extern u8 gGraphics_bDither;
extern s32 D_8009B0B0;
extern struct GraphicsFrameBuffer *gGraphics_pActiveFrameBuffer;
extern void (*gMain_pfnExtraFrameService)(void);
extern s32 D_8009B0BC;
extern volatile u8 gGraphics_bSyncVBlankTarget;
extern u8 gGraphics_bExtraFrameStep;
extern u8 gMain_bVBlankSoundPending;
extern volatile s32 gSaveData_dwVBlankCounter;
extern volatile s32 gGraphics_dwSyncVBlankCounter;
extern volatile s32 gGraphics_dwSyncedFrameCount;
extern u8 gGraphics_bClearBackground;
extern u8 D_8009B0D1;
extern s32 D_8009B0D4;
extern s32 D_8009B0D8;
#endif
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libetc.h"
#include "../psyq/sdk_internal.h"
#define D_8009B142_IN_DATA
#define D_8009B0C0_IS_VOLATILE
#include "graphics_frame.h"
#define D_8009B141_IN_DATA
#include "fade.h"
#include "fade_constants.h"
#include "graphics_frame_buffer.h"
#include "../psyq/setjmp.h"

#ifndef GRAPHICS_FRAME_REGIONAL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B4A8-0x800E9DF0, defined in
 * address order (#2602). graphics_frame.h declares the first three in this
 * order; the rest are first declared here, ahead of ordering_tables.h. The
 * ordering-table slot names and the per-slot callback names are elements,
 * named by link_symbols.ld. */
GraphicsFrameBuffer gGraphics_aFrameBuffers[2];
u8 gGraphics_abPacketBuffers[2 * GRAPHICS_PACKET_BUFFER_SIZE];
DISPENV gGraphics_DispEnv;
static u8 D_800E9D40[0x30];
RECT gGraphics_aImageTransferRects[2];
static u32 D_800E9D80[4];
GsOT *gGraphics_apOrderingTables[4];
static u32 D_800E9DA0[4];
void (*gMain_apFrameServiceCallbacks[4])(void);
jmp_buf gMain_RecoveryJmpBuf;
#else
extern GraphicsFrameBuffer gGraphics_aFrameBuffers[2];
extern u8 gGraphics_abPacketBuffers[2 * GRAPHICS_PACKET_BUFFER_SIZE];
extern DISPENV gGraphics_DispEnv;
extern RECT gGraphics_aImageTransferRects[2];
extern GsOT *gGraphics_apOrderingTables[4];
extern void (*gMain_apFrameServiceCallbacks[4])(void);
extern jmp_buf gMain_RecoveryJmpBuf;
#endif
#include "ordering_tables.h"
#define MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE
#include "model_graphics_state.h"
#include "movie_playback_control.h"
#include "../external_funcs.h"
#include "graphics_frame_controls.h"

/* Waits for the current GPU/VBlank boundary and publishes the bounded number
   of frame advances consumed by the next game update. */
void Graphics_SyncFrame(void)
{
    if ((gGraphics_wFlags & 0x8000) == 0) {
        DrawSync(0);
    }
    while (gGraphics_dwSyncVBlankCounter < gGraphics_bSyncVBlankTarget) {
    }

    gGraphics_bExtraFrameStep = gGraphics_dwSyncVBlankCounter;
    if (gGraphics_bExtraFrameStep & 0xFF) {
        gGraphics_bExtraFrameStep = 1;
    }
    gGraphics_nBaseFrameStep = gGraphics_bExtraFrameStep + 1;

    gGraphics_bFrameStep = gGraphics_FrameStepControl ? 2 : gGraphics_nBaseFrameStep;

    gGraphics_FrameStepControl = 0;
    gGraphics_dwSyncVBlankCounter = -1;

    VSync(0);

    gGraphics_dwSyncedFrameCount++;
}

void Graphics_BeginFrame(void)
{
    s32 i;
    GsOT **slot;
    s32 idx;
    GsOT *ptr;
    u8 *arg;
    GsOT **base;

    if (gGraphics_bSkipDisplaySwap == 0) {
        gLibGs_DrawEnvView[0].isbg = gGraphics_bClearBackground;
        gLibGs_DrawEnvView[0].dtd = gGraphics_bDither;
        gLibGs_DrawEnvView[0].r0 = gGraphics_bClearRed;
        gLibGs_DrawEnvView[0].g0 = gGraphics_bClearGreen;
        gLibGs_DrawEnvView[0].b0 = gGraphics_bClearBlue;
        GsSwapDispBuff();
        if ((gGraphics_wFlags & 0x2000) != 0) {
            PutDispEnv(&gGraphics_DispEnv);
        }
    }
    ResetGraph(1);
    if ((gMovie_bPlaybackState & 0x80) != 0) {
        Movie_UpdatePlayback();
    }
    if ((gMovie_bPlaybackState & 0x80) == 0) {
        if (gFade_bOrderingTableMode != 0) {
            GsSortOt(&gGraphics_pActiveFrameBuffer->ordering_tables[1],
                     &gGraphics_pActiveFrameBuffer->ordering_tables[0]);
            if ((gFade_bOrderingTableMode & FADE_ORDERING_TABLE_HIDE_SECONDARY) == 0) {
                GsSortOt(&gGraphics_pActiveFrameBuffer->ordering_tables[2],
                         &gGraphics_pActiveFrameBuffer->ordering_tables[0]);
                GsSortOt(&gGraphics_pActiveFrameBuffer->ordering_tables[3],
                         &gGraphics_pActiveFrameBuffer->ordering_tables[0]);
            }
            GsDrawOt(&gGraphics_pActiveFrameBuffer->ordering_tables[0]);
        }
    }
    idx = GsGetActiveBuff();
    i = 3;
    gGraphics_bActiveBuffer = idx;
    arg = &gGraphics_abPacketBuffers[
        gGraphics_bActiveBuffer * GRAPHICS_PACKET_BUFFER_SIZE
    ];
    gGraphics_bPacketBufferIndex = idx;
    gGraphics_pActiveFrameBuffer =
        &gGraphics_aFrameBuffers[gGraphics_bActiveBuffer];
    GsSetWorkBase((PACKET *)arg);
    base = gGraphics_apOrderingTables;
    slot = base + 3;
    do {
        ptr = &gGraphics_pActiveFrameBuffer->ordering_tables[i];
        *slot = ptr;
        slot--;
        ptr->length = gGraphics_abOrderingTableLengths[i];
        GsClearOt(0, 0, ptr);
        i--;
    } while (i >= 0);
}
