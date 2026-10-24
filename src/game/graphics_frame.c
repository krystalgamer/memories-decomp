#define D_8009B318_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libetc.h"
#include "../psyq/sdk_internal.h"
#define D_8009B142_IN_DATA
#define D_8009B0C0_IS_VOLATILE
/* Keep the frozen declaration position for common emission order. */
#ifdef D_8009B0A8
#undef D_8009B0A8
#endif
#define D_8009B0A8 gGraphics_bSkipDisplaySwap
#include "graphics_frame.h"
#define D_8009B141_IN_DATA
#include "fade.h"
#include "fade_constants.h"
#include "graphics_frame_buffer.h"
#include "ordering_tables.h"
#define MODEL_GRAPHICS_STATE_FRAME_ABSOLUTE
#include "model_graphics_state.h"
#include "movie_playback_control.h"
#include "../external_funcs.h"
#include "graphics_frame_controls.h"

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the load-delay nop before the store below. The linker binding overrides this
   common symbol, so no storage is allocated here. */
u8 gGraphics_bExtraFrameStep;

/* gp-relative in the target, so this unit defines them */
u8 gGraphics_bSkipDisplaySwap;
u8 gGraphics_bDither;
u8 gGraphics_bClearBackground;
u16 gGraphics_wFlags;
u8 gGraphics_abOrderingTableLengths[4];
u8 gGraphics_bActiveBuffer;
GraphicsFrameBuffer *G32 gGraphics_pActiveFrameBuffer;
s16 gGraphics_sViewportX __attribute__((section(".sbss"))) = 0;
s16 gGraphics_sViewportY __attribute__((section(".sbss"))) = 0;

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
    GsOT *G32 *slot;
    s32 idx;
    GsOT *ptr;
    u8 *arg;
    GsOT *G32 *base;

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
