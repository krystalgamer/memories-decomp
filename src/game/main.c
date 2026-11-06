/* The resident startup and frame layer: Main_Init, the VBlank callback and
   frame advance, the graphics frame sync and ordering-table setup, and the
   per-frame service pump with the boot-time hardware initialization. Retail
   built these as one unit: its own small data is gp-relative from every
   function here and absolute from every other unit (#2602). */
#define D_8009B0C0_IS_VOLATILE
#define MODEL_GRAPHICS_STATE_CLAMP_NONVOLATILE
#define D_8009B0D1_IS_VOLATILE
#define D_8009B142_IN_DATA_VOLATILE
#define D_8009B14A_IN_DATA_VOLATILE
#include "../types.h"

#ifndef GRAPHICS_FRAME_REGIONAL_SMALL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B090-0x8009B0E0, defined in
 * address order ahead of the headers that declare it (#2602). The startup,
 * frame and service units sit beside this one in text and declare these
 * through their headers. */
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
s32 gGraphics_nBaseFrameStep;
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
extern s32 gGraphics_nBaseFrameStep;
#endif

/* Small data at 0x8009AF08-0x8009AF0D. gMain_bSoundVSyncBusy keeps a nested VBlank callback from calling SD_VSync
   while the previous call is still running. */
s32 gMain_nServiceResyncCountdown = 0x3C;
u8 gMain_bSoundVSyncBusy = 0;

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libetc.h"
#include "../psyq/sdk_internal.h"
#include "graphics_frame.h"
#include "fade.h"
#include "fade_constants.h"
#include "graphics_frame_buffer.h"
#include "../psyq/setjmp.h"

#ifndef GRAPHICS_FRAME_STORAGE_ORDER
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
/* The regional wrapper declared these in its own address order. */
GraphicsFrameBuffer gGraphics_aFrameBuffers[2];
u8 gGraphics_abPacketBuffers[2 * GRAPHICS_PACKET_BUFFER_SIZE];
DISPENV gGraphics_DispEnv;
RECT gGraphics_aImageTransferRects[2];
GsOT *gGraphics_apOrderingTables[4];
void (*gMain_apFrameServiceCallbacks[4])(void);
jmp_buf gMain_RecoveryJmpBuf;
#endif
#include "../unmatched.h"
#include "duel_side_state.h"
#include "duel_effect_resource_setup.h"
#include "display_object_core.h"
#include "sound.h"
#include "text_init_decimal_digit_glyph_map.h"
#include "main_frame.h"
#include "../psyq/libapi.h"
#include "../psyq/crt.h"
#include "../psyq/rand.h"
#include "file_transfer.h"
#include "main_menu_selection.h"
#include "duel_effect_reset_state.h"
#include "main_run_boot_sequence.h"
#include "main_run_frontend_loop.h"
#include "main_loop.h"
#include "main_reset_frontend_runtime.h"
#include "main_services.h"
#include "rand_constants.h"
#include "movie_playback_control.h"
#include "main_mode_state.h"
#include "graphics_frame_controls.h"
#include "input.h"
#include "sound_output.h"
#include "model_graphics_state.h"
#include "../external_funcs.h"
#include "../psyq/libmcrd.h"
#include "func_800136D4.h"
#include "display_object_render_frame.h"
#include "graphics_constants.h"
#include "model.h"
#include "ordering_tables.h"

s32 Main_Init(void)
{
    s32 r;
    s32 t;
    register GraphicsFrameBuffer *p;

    __main();
#ifdef VERSION_EUROPE
    SetVideoMode(1);
#endif
    EnterCriticalSection();
    ResetCallback();
    GsInitVcount();
    ExitCriticalSection();
    StopCallback();
    SetMem(2);
    SetDispMask(0);
    Fade_DisableOrderingTables();
    p = gGraphics_aFrameBuffers;
    gGraphics_dwSyncedFrameCount = 0;
    gGraphics_dwSyncVBlankCounter = 0;
    gGraphics_bSyncVBlankTarget = 0;
    gMain_dwFrameCounter = 0;
    t = gMain_dwFrameCounter;
#ifdef VERSION_EUROPE
    gText_bLanguageIndex = BUILD_LANGUAGE_INDEX;
#endif
    gMain_bVBlankSoundPending = 0;
    gGraphics_bExtraFrameStep = 0;
    gGraphics_nBaseFrameStep = 1;
    gGraphics_wFlags = 0x5000;
    D_8009B0D1 = 0;
    gDuel_bTwoPlayerCardViewMode = 1;
    gGraphics_pActiveFrameBuffer = p;
    gSaveData_dwVBlankCounter = t;
    Main_InitHardware(p);
    DisplayObject_Reset();
    Fade_Init();
    Main_ClearFrameServiceCallbacks();
    Movie_ResetPlaybackState();
    DuelEffect_ResetState();
    Text_InitDecimalDigitGlyphMap();
    SD_InitState(*(u8 *)&gFile_anLba[FILE_INDEX_MASTER_XA]);
    VSyncCallback(Main_VBlankCB);
    Sound_InitFrontend();
    srand(RAND_BOOT_SEED);
    SetDispMask(1);
    Main_ResetFrontendRuntime();
    Main_ResetFrontendRuntime();
    Main_RunBootSequence(0);
    r = setjmp(gMain_RecoveryJmpBuf);
    Main_ResetFrontendRuntime();
    if (r != 0) {
        File_RequestMainMenuPackage();
        File_WaitForTransfers();
    }
    Main_ApplyMenuSelection(Main_RunFrontendLoop());
    gMain_bReturnMode = 8;
    Main_Loop();
    return 0;
}

void Main_VBlankCB(void)
{
    gMain_dwFrameCounter++;
    gSaveData_dwVBlankCounter++;
    gMain_bVBlankSoundPending = 1;
    gGraphics_dwSyncVBlankCounter++;
    Input_ReadRawPads();

    if (gMain_bSoundVSyncBusy == 0) {
        gMain_bSoundVSyncBusy = 1;
        SD_VSync();
        gMain_bSoundVSyncBusy = 0;
        gMain_bVBlankSoundPending = 0;
    }
}

void Main_AdvanceFrame(void)
{
    Main_RunFrameServices();
    Graphics_SyncFrame();
    Graphics_BeginFrame();
    Input_UpdatePads();
}

void Main_AdvanceFrames(s32 count)
{
    do {
        Main_AdvanceFrame();
    } while (--count != 0);
}

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
        GsDRAWENV.isbg = gGraphics_bClearBackground;
        GsDRAWENV.dtd = gGraphics_bDither;
        GsDRAWENV.r0 = gGraphics_bClearRed;
        GsDRAWENV.g0 = gGraphics_bClearGreen;
        GsDRAWENV.b0 = gGraphics_bClearBlue;
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

/* Per-frame dispatcher: runs the two fixed housekeeping calls, then each of
   the 4 slots in gMain_apFrameServiceCallbacks and the single gMain_pfnExtraFrameService callback if set. If
   neither of the two progress pairs (f1A8/f19C, f1B4/f1CC) has advanced and
   the watchdog counter D_8009AF08 underflows, resets the counter to 0x3C
   and re-syncs both progress pairs. Finishes with
   File_ServiceTransfers/func_800136D4. */
void Main_RunFrameServices(void) {
    void (*fn)(void);
    s32 i;
    s32 cnt;

    Fade_DrawOverlay();
    DisplayObject_RenderFrame();

    for (i = 0; i < 4; i++) {
        fn = gMain_apFrameServiceCallbacks[i];
        if (fn != 0) {
            fn();
        }
    }

    fn = gMain_pfnExtraFrameService;
    if (fn != 0) {
        fn();
    }

    if (D_8009B0B0 < D_8009B0A4 || D_8009B0BC < D_8009B0D4) {
        goto reset;
    }
    cnt = gMain_nServiceResyncCountdown - 1;
    gMain_nServiceResyncCountdown = cnt;
    if (cnt < 0) {
    reset:
        gMain_nServiceResyncCountdown = 0x3C;
        D_8009B0B0 = D_8009B0A4;
        D_8009B0BC = D_8009B0D4;
    }

    File_ServiceTransfers(0);
    func_800136D4();
}

/* Boot-time graphics and input startup. The work area contains two 0x5160
 * byte frame buffers; each receives four ordering tables before the display
 * environment and frontend services are initialized. */
void Main_InitHardware(GraphicsFrameBuffer *base)
{
    GraphicsFrameBuffer *buf;
    s32 k;
    s32 six;
    u16 count;

    ResetGraph(0);
    GsInitGraph(GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 4, 1, 0);
    GsDefDispBuff(0, 0, 0x140, 0);
    six = 6;
    buf = base;
#ifdef VERSION_EUROPE
    /* The European build moves the display 20 lines down and shows 256. */
    GsDISPENV.screen.y = 20;
    GsDISPENV.screen.h = 256;
#endif
    gGraphics_bDither = 1;
    gGraphics_bClearBackground = 1;
    gGraphics_bSkipDisplaySwap = 0;
    gFade_bTargetRed = 1;
    gGraphics_bClearRed = 1;
    gFade_bTargetGreen = 1;
    gGraphics_bClearGreen = 1;
    gFade_bTargetBlue = 1;
    gGraphics_bClearBlue = 1;
    ((volatile DRAWENV *)&GsDRAWENV)->isbg = 1;
    ((volatile DRAWENV *)&GsDRAWENV)->dtd = 1;
    ((volatile DRAWENV *)&GsDRAWENV)->r0 = 1;
    ((volatile DRAWENV *)&GsDRAWENV)->g0 = 1;
    ((volatile DRAWENV *)&GsDRAWENV)->b0 = 1;
    count = six;
    ((volatile u8 *)gGraphics_abOrderingTableLengths)[0] = 2;
    ((volatile u8 *)gGraphics_abOrderingTableLengths)[1] = count;
    ((volatile u8 *)gGraphics_abOrderingTableLengths)[2] = 0xC;
    ((volatile u8 *)gGraphics_abOrderingTableLengths)[3] = count;
next:
    k = 3;
    buf->ordering_tables[0].length = 2;
    buf->ordering_tables[1].org =
        (GsOT_TAG *)(buf->ordering_table_tags + 0x10);
    buf->ordering_tables[2].length = 0xC;
    buf->ordering_tables[2].org =
        (GsOT_TAG *)(buf->ordering_table_tags + 0x110);
    buf->ordering_tables[0].org = (GsOT_TAG *)buf->ordering_table_tags;
    buf->ordering_tables[1].length = six;
    buf->ordering_tables[3].length = six;
    buf->ordering_tables[3].org =
        (GsOT_TAG *)(buf->ordering_table_tags + 0x4110);
    do {
        GsClearOt(0, k, &buf->ordering_tables[k]);
        k--;
    } while (k >= 0);
    buf++;
    if ((s32)buf < (s32)(base + 2)) {
        goto next;
    }
    gGraphics_DispEnv = GsDISPENV;
    InitGeom();
    GsInit3D();
    GsSetOrign(0, 0);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);
#ifdef VERSION_EUROPE
    /* The European build starts the memory cards before the pads. */
    MemCardInit(1);
    Input_InitPads();
#else
    Input_InitPads();
    MemCardInit(1);
#endif
    File_SetPositionTable();
    srand(RAND_GRAPHICS_INIT_SEED);
}

/* Held with a direction, this button doubles the adjustment step. The Japanese
 * release swaps Cross and Circle, so a regional build names its own. */
#ifndef MAIN_SCREEN_ADJUST_BUTTON
#define MAIN_SCREEN_ADJUST_BUTTON PAD_BUTTON_CROSS
#endif

/* Debug screen-offset adjustment: zeroes the display origin, raises bit 0x2000
 * of gGraphics_wFlags, then advances frames until Start is pressed. While a
 * direction is held the origin moves by 2 per frame, or 4 with Cross held.
 *
 * The origin is cleared through one pointer and adjusted through a second
 * that is copied from it after the first Main_AdvanceFrame call. The first
 * pointer therefore lives only in the entry block and crosses that call, so
 * local-alloc ties it to the %hi temporary that feeds it and gives the pair
 * $s0; the loop's pointer inherits $s0 through the copy, which disappears. */
void Graphics_AdjustScreenOffset(void)
{
    RECT *origin;
    RECT *r;

    origin = &gGraphics_DispEnv.disp;
    origin->x = 0;
    origin->y = 0;
    gGraphics_wFlags |= 0x2000;

    Main_AdvanceFrame();
    r = origin;
    while ((gInput_awPadPressed[0] & PAD_BUTTON_START) == 0) {
        s32 step;

        if (gInput_awPadHeld[0] & PAD_DIRECTION_MASK) {
            step = 2;
            if (gInput_awPadHeld[0] & MAIN_SCREEN_ADJUST_BUTTON) {
                step = 4;
            }
            if (gInput_awPadHeld[0] & PAD_DIRECTION_RIGHT) {
                r->x += step;
            }
            if (gInput_awPadHeld[0] & PAD_DIRECTION_LEFT) {
                r->x -= step;
            }
            if (gInput_awPadHeld[0] & PAD_DIRECTION_UP) {
                r->y -= step;
            }
            if (gInput_awPadHeld[0] & PAD_DIRECTION_DOWN) {
                r->y += step;
            }
        }
        FntFlush(-1);
        Main_AdvanceFrame();
    }

    gGraphics_wFlags &= 0xDFFF;
    Input_ResetPads();
}

/* Zeroes gMain_apFrameServiceCallbacks[0..3] and gMain_pfnExtraFrameService. */
void Main_ClearFrameServiceCallbacks(void)
{
    void (**v0)(void);
    int v1;

    v1 = 3;
    v0 = &gMain_apFrameServiceCallbacks[v1];
    do {
        *v0 = 0;
        v1 -= 1;
        v0 -= 1;
    } while (v1 >= 0);
    gMain_pfnExtraFrameService = 0;
}
