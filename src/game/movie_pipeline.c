#define D_8009B142_IN_DATA
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800F5750-0x800F5768, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 gMovie_FileInfo[24];

#include "movie_open_stream.h"

#ifndef MOVIE_FRAME_PIPELINE_REGIONAL_SMALL_STORAGE
/* Zero-initialized storage this unit owns at 0x8009B498-0x8009B4A8, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 *gMovie_pWorkArea;
CdlLOC gMovie_StreamLoc;
u8 D_8009B4A0;
u8 D_8009B4A1;
u8 D_8009B4A2;
static u8 D_8009B4A3;
static u32 D_8009B4A4;
#else
extern u8 *gMovie_pWorkArea;
extern CdlLOC gMovie_StreamLoc;
extern u8 D_8009B4A0;
extern u8 D_8009B4A1;
extern u8 D_8009B4A2;
#endif

#include "movie_seek_and_start_read.h"
#include "movie_on_stream_end.h"
#include "graphics_frame.h"
#include "movie_frame_pipeline.h"
#include "movie_handoff_state.h"
#include "../psyq/libcd.h"
#include "../psyq/libpress.h"
#include "graphics_constants.h"
#include "high_memory_addresses.h"
#include "shared_work_base.h"
#include "../psyq/libgs.h"
#include "../psyq/libetc.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_cd_helpers.h"
#include "mdec_sync.h"
#include "sound_mix.h"
#include "../unmatched.h"

s32 Movie_OpenStream(CdlLOC *src, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    RECT rect;
    s32 m;
    s32 r;

    gMovie_bStreamEnded = 0;
    gMovie_bFadingOut = 0;
    gMovie_bCdVolume = 0xFF;
    gMovie_dwEndSector = a3;
    gMovie_bRgb24 = a4;
    gMovie_pWorkArea = gRuntime_pSharedWorkBase;
    gMovie_bCopyDisplayOnStop = a5;
    if (gMovie_bRgb24 != 0) {
        /* Retail sets $a0 and $a1 here even though GsGetActiveBuff takes
           no arguments. The call goes through a two-argument view of the
           libgs.h declaration so that setup survives with the prototype in
           scope, instead of through an implicit declaration. */
        m = ((int (*)(void *, s32))GsGetActiveBuff)(gRuntime_pSharedWorkBase, a5);
        rect.x = m * 0x140;
        rect.y = 0;
        rect.w = gGraphics_CurrentWidth.pixels;
        rect.h = gGraphics_CurrentHeight.word;
        ClearImage(&rect, gGraphics_bClearRed, gGraphics_bClearGreen, gGraphics_bClearBlue);
        DrawSync(0);
        VSync(0);
        GsSwapDispBuff();
        rect.x = (m ^ 1) * 0x140;
        rect.w = gGraphics_CurrentWidth.pixels;
        rect.y = 0;
        rect.h = gGraphics_CurrentHeight.word;
        ClearImage(&rect, gGraphics_bClearRed, gGraphics_bClearGreen, gGraphics_bClearBlue);
        rect.x = 0;
        rect.y = 0x100;
        rect.w = gGraphics_CurrentWidth.word * 0x1800 / 4096;
        rect.h = gGraphics_CurrentHeight.word;
        ClearImage(&rect, gGraphics_bClearRed, gGraphics_bClearGreen, gGraphics_bClearBlue);
        DrawSync(0);
        VSync(0);
        gLibGs_sActiveBufferIndex = 1;
        GsSwapDispBuff();
#ifdef VERSION_EUROPE
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, 0x100, 4, 1, 1);
        GsDefDispBuff(0, 0, 0, 0x100);
        GsDISPENV.screen.y = 20;
        GsDISPENV.screen.h = 0x100;
#else
        GsDefDispBuff(0, 0, 0, 0x100);
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 4, 1, 1);
#endif
    }
    if (src != 0) {
        gMovie_StreamLoc = *src;
    }
    if (a1 != 0) {
        gMovie_dwStartFrame = a1;
    } else {
        gMovie_dwStartFrame = 1;
    }
    if (a2 != 0) {
        gMovie_dwEndFrame = a2;
    } else {
        gMovie_dwEndFrame = 0xFFFF;
    }
    gMovie_bInputSlot = 0;
    gMovie_bOutputSlot = 0;
    DecDCTReset(0);
    DecDCToutCallback(Movie_OnDecodeOutput);
    DecDCTvlcBuild((u16 *)gMovie_pWorkArea);
    StSetRing((u32 *)(gMovie_pWorkArea + sizeof(DECDCTTAB)), 0x14);
    StClearRing();
    StSetStream(gMovie_bRgb24, gMovie_dwStartFrame, gMovie_dwEndFrame, 0, Movie_OnStreamEnd);
    if (gMovie_dwEndFrame >= 5) {
        gMovie_dwEndFrame = gMovie_dwEndFrame - 4;
    }
    Movie_SeekAndStartRead(&gMovie_StreamLoc);
    r = Movie_WaitAndDecodeFrame(0);
    if (r != 0) {
        return r;
    }
    return 0;
}

#ifndef MOVIE_FRAME_HEIGHT
#define MOVIE_FRAME_HEIGHT 0xF0
#endif

#ifndef MOVIE_DECODED_SLOT_SIZE
#define MOVIE_DECODED_SLOT_SIZE 0x2D00
#endif

/* The start of the movie player, in image order: the stop path that tears the
   stream down and repaints the screen, then the three stages that decode and
   present one frame. The four are contiguous and follow File_RequestMainMenuPackage. */

s32 Movie_StopStream(s32 arg0) {
    RECT rect;
    u8 buf[0x3C0];
    s32 i;
    s32 m;
    u8 *q;

    DrawSync(0);
    SD_SetCdVolumeImmediate(0);
    /* volatile: retail sets a0 = 0 above this store and leaves the jal's
       delay slot empty, which only a store that cannot enter a slot gives. */
    *(volatile u8 *)&gMovie_bStreamEnded = 1;
    DecDCToutCallback(0);
    StUnSetRing();
    while (CdControlB(9, 0, 0) == 0) {
    }
    if (gMovie_bCopyDisplayOnStop != 0 || arg0 != 0) {
        Movie_MoveDisplayImage(0, 0x100);
    }
    if (gMovie_bRgb24 != 0) {
        i = 0;
        m = GsGetActiveBuff();
        q = buf;
        for (; i < 0x3C0; i += 3) {
            *q++ = gGraphics_bClearRed;
            *q++ = gGraphics_bClearGreen;
            *q++ = gGraphics_bClearBlue;
        }
        rect.x = 0;
        i = 0;
        rect.y = m << 8;
        rect.w = 0x1E0;
        rect.h = 1;
        for (; i < gGraphics_CurrentHeight.word; i++) {
            LoadImage(&rect, (u32 *)buf);
            DrawSync(0);
            rect.y++;
        }
        rect.x = 0x140;
        rect.y = 0;
        rect.h = 1;
        rect.w = gGraphics_CurrentWidth.pixels;
        for (i = 0; i < gGraphics_CurrentHeight.word; i++) {
            LoadImage(&rect, (u32 *)buf);
            DrawSync(0);
            rect.y++;
        }
        DrawSync(0);
        VSync(0);
        i = 0;
        GsSwapDispBuff();
        rect.y = (m ^ 1) << 8;
        rect.w = 0x1E0;
        rect.x = 0;
        rect.h = 1;
        for (; i < gGraphics_CurrentHeight.word; i++) {
            LoadImage(&rect, (u32 *)buf);
            DrawSync(0);
            rect.y++;
        }
        DrawSync(0);
        VSync(0);
        gLibGs_sActiveBufferIndex = 1;
        GsSwapDispBuff();
#ifdef VERSION_EUROPE
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, 0x100, 4, 1, 0);
        GsDefDispBuff(0, 0, GRAPHICS_DEFAULT_WIDTH, 0);
        GsDISPENV.screen.y = 20;
        GsDISPENV.screen.h = 0x100;
#else
        GsDefDispBuff(0, 0, 0x140, 0);
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 4, 1, 0);
#endif
        rect.x = 0;
        rect.y = 0;
        rect.w = gGraphics_CurrentWidth.word * 2;
        rect.h = gGraphics_CurrentHeight.pixels;
        ClearImage(&rect, gGraphics_bClearRed, gGraphics_bClearGreen, gGraphics_bClearBlue);
        gMovie_bRgb24 = 0;
    }
    return 0;
}

/* Decodes and presents one movie frame. gMovie_bStreamEnded marks the stream as
 * finished and gMovie_bFadingOut as fading out, in which case the fade level in
 * gMovie_bCdVolume drops by 0x40 a frame and the stream ends once it reaches zero.
 * The work area at gMovie_pWorkArea + 0x40000 carries two RECTs, the frame rect at
 * +0x2420 and the display rect at +0x2428. Both are centred against the
 * current drawing clip: horizontally in the 320- or 640-wide screen chosen by
 * gMovie_bRgb24, vertically in 240. The ring slots are 0xE000 apart from
 * +0x1B000 for the coded data and 0x2D00 apart from +0x37000 for the decoded
 * output. Returns non-zero once the stream is done. */
s32 Movie_DecodeAndPresentFrame(void) {
    DRAWENV env;
    s32 fade;
    s32 result;
    s32 x;
    s32 y;
    MovieWorkArea *frame;
    MovieWorkArea *display;
    MovieWorkArea *rects;
    MovieWorkArea *out;
    s32 slot;
    s32 side;

    if (gMovie_bStreamEnded != 0) {
        return 1;
    }
    if (gMovie_bFadingOut != 0) {
        if (gMovie_bCdVolume == 0) {
            return 1;
        }
        fade = gMovie_bCdVolume - 0x40;
        if (fade < 0) {
            fade = 0;
        }
        gMovie_bCdVolume = fade;
        SD_SetCdVolumeImmediate(gMovie_bCdVolume);
    }
    /* volatile: retail sets up the following call's argument above this store
       and leaves its delay slot empty, which only a store that cannot be
       moved into a slot gives. */
    *(volatile u8 *)&gMovie_bFrameDecoded = 0;
    GetDrawEnv(&env);

    frame = (MovieWorkArea *)(gMovie_pWorkArea + 0x40000);
    display = (MovieWorkArea *)(gMovie_pWorkArea + 0x40000);
    x = env.clip.x +
        ((gMovie_bRgb24 != 0 ? 0x1E0 : 0x140) - frame->frame.w) / 2;
    side = gMovie_bRgb24;
    slot = gMovie_bInputSlot;
    /* frame and display hold one address, so typed these two stores
       hit provably distinct members and GCC reschedules them against
       the surrounding gp loads. Through u8 * it cannot tell them
       apart, which is the order retail has; kept opaque, and re-based
       so retyping the cursor does not rescale the member-derived offsets. */
    *(s16 *)((u8 *)display +
        (u32)&((MovieWorkArea *)0)->strip.x) = x;
    *(s16 *)((u8 *)frame +
        (u32)&((MovieWorkArea *)0)->frame.x) = x;

    rects = (MovieWorkArea *)(gMovie_pWorkArea + 0x40000);
    y = *(u16 *)&env.clip.y + (MOVIE_FRAME_HEIGHT - rects->frame.h) / 2;
    rects->strip.y = y;
    rects->frame.y = y;
    DecDCTin((u32 *)(gMovie_pWorkArea + 0x1B000 + slot * 0xE000), side);

    out = (MovieWorkArea *)(gMovie_pWorkArea + 0x40000);
    DecDCTout((u32 *)(gMovie_pWorkArea + 0x37000 +
                      gMovie_bOutputSlot * MOVIE_DECODED_SLOT_SIZE),
              out->strip.w * out->strip.h / 2);

    result = Movie_WaitAndDecodeFrame(1);
    if (result != 0) {
        return result;
    }
    Movie_WaitFrameDecoded();
    return 0;
}

/* Fetches the next movie frame from the CD stream ring and runs the VLC
 * decode on it. gMovie_bStreamEnded marks the stream as finished and gMovie_bFadingOut as
 * fading out; both are set from the sector position and frame count in the
 * STR header against the limits in gMovie_dwEndSector and gMovie_dwEndFrame. While no
 * sector is ready the loop waits on VSync, and after 60 frames without one
 * it re-issues the read at gMovie_StreamLoc, at once when resync is requested and
 * after ten timeouts otherwise. Frames behind gMovie_dwStartFrame are dropped. The
 * decoded output alternates between the two coded slots at gMovie_pWorkArea +
 * 0x1B000, the VLC table being the start of the work area, and the frame
 * rect at gMovie_pWorkArea + 0x42424 takes the header's width (three halves of it
 * in the 640-wide mode chosen by gMovie_bRgb24), height and column count.
 * Returns non-zero once the stream is done. */
s32 Movie_WaitAndDecodeFrame(s32 resync) {
    u32 *ring;
    StHEADER *hdr;
    s32 timeouts;
    s32 now;
    s32 last;
    /* Retail keeps the 1 stored into the two flags in a callee-saved
       register across the calls. This compiler has no global constant
       propagation, so a local set once serves every store below. */
    s32 set;
    MovieWorkArea *rects;
    StHEADER *header;
    u32 width;
    u32 frame_width;
    u16 height;
    s32 columns;
    s32 wide;
    u32 *base;

    if (gMovie_bStreamEnded != 0) {
        return 1;
    }
    set = 1;
    if (resync != 0) {
        gMovie_dwStartFrame = StGetBackloc(&gMovie_StreamLoc);
    }
    for (;;) {
        last = VSync(-1);
        timeouts = 0;
        for (;;) {
            if (StGetNext(&ring, (u32 **)&hdr) == 0) {
                break;
            }
            now = VSync(-1);
            if (now - last < 60) {
                continue;
            }
            if (gMovie_bStreamEnded != 0) {
                goto fail;
            }
            timeouts++;
            if (resync != 0 || timeouts >= 10) {
                Movie_SeekAndStartRead(&gMovie_StreamLoc);
                timeouts = 0;
            }
            /* One shared store: reorg copies it into both back-jump delay
               slots, and counting it once is what ranks the three loop
               variables into retail's callee-saved registers. */
            last = now;
            continue;
        /* The label sits after the back jump on purpose. A `return 1`
           block that is only reached by a jump is what stops GCC
           cross-jumping the first early return into it. */
        fail:
            return 1;
        }

        if (CdPosToInt(&hdr->loc) + hdr->nSectors * 2 >= gMovie_dwEndSector) {
            gMovie_bStreamEnded = set;
        }
        if (gMovie_dwStartFrame >= gMovie_dwEndFrame) {
            gMovie_bFadingOut = set;
        }
        if (hdr->frameCount >= gMovie_dwEndFrame) {
            gMovie_bFadingOut = set;
        }
        if (gMovie_bStreamEnded == 0) {
            if (hdr->frameCount < gMovie_dwStartFrame) {
                StFreeRing(ring);
                continue;
            }
        }
        break;
    }
    gMovie_bInputSlot = (gMovie_bInputSlot + 1) & 1;
    DecDCTvlc2(ring, (u32 *)(gMovie_pWorkArea + 0x1B000 + gMovie_bInputSlot * 0xE000),
               (u16 *)gMovie_pWorkArea);

    rects = (MovieWorkArea *)(gMovie_pWorkArea + 0x40000);
    width = hdr->width;
    frame_width = gMovie_bRgb24 != 0 ? width * 3 / 2 : width;
    columns = 0x10;
    *(u16 *)&rects->frame.w = frame_width;

    /* Reading the height into a local before the mode byte is what puts
       the work-area pointer ahead of the height in local-alloc. */
    header = hdr;
    rects = (MovieWorkArea *)(gMovie_pWorkArea + 0x40000);
    height = header->height;
    wide = gMovie_bRgb24;
    *(u16 *)&rects->frame.h = height;
    if (wide != 0) {
        columns = 0x18;
    }
    base = ring;
    *(u16 *)&rects->strip.w = columns;
    *(u16 *)&rects->strip.h = header->height;
    StFreeRing(base);
    return 0;
}

/* Feeds the MDEC one strip of the current movie frame. The strip header at
 * gMovie_pWorkArea + 0x42428 is copied into the ring slot for gMovie_bOutputSlot, that slot
 * and the matching decoded-output buffer go to LoadImage, the slot index
 * advances modulo four, and the accumulated strip height at +0x42428 grows by
 * the strip's own height. While the accumulation is still short of the frame
 * rect at +0x42420 the next strip is queued with DecDCTout; otherwise
 * gMovie_bFrameDecoded marks the frame complete.
 *
 * Writing dst off gMovie_pWorkArea with its own +0x40000, rather than off src, is
 * what keeps retail's two separate additions of that constant. The strip
 * height is accumulated in place (strip.x += strip.w) and compared through the
 * record: that single read-modify-write is what lets GCC's own expansion of
 * the % 4 above it take retail's registers. */
void Movie_OnDecodeOutput(void) {
    u8 *src;
    u8 *dst;
    s32 idx;
    MovieWorkArea *out;

    if (gMovie_bRgb24 != 0) {
        if (gLibCd_nDeferredStreamInterrupt != 0) {
            StCdInterrupt();
            gLibCd_nDeferredStreamInterrupt = 0;
        }
    }
    dst = gMovie_pWorkArea + (u32)&((RECT *)0)[gMovie_bOutputSlot] + 0x40000;
    src = gMovie_pWorkArea + 0x40000;
    ((MovieWorkArea *)dst)->slots[0] = ((MovieWorkArea *)src)->strip;
    idx = gMovie_bOutputSlot;
    LoadImage((RECT *)(gMovie_pWorkArea + 0x40000 + MOVIE_WORK_AREA_HEAD_SIZE +
                       (u32)&((RECT *)0)[idx]),
              (u32 *)(gMovie_pWorkArea + 0x37000 +
                      (u32)&((u8 (*)[MOVIE_DECODED_SLOT_SIZE])0)[idx]));
    gMovie_bOutputSlot = (gMovie_bOutputSlot + 1) % 4;
    out = (MovieWorkArea *)(gMovie_pWorkArea + 0x40000);
    out->strip.x += out->strip.w;
    if (out->strip.x < out->frame.x + out->frame.w) {
        DecDCTout((u32 *)(
                      gMovie_pWorkArea + 0x37000 +
                      (u32)&((u8 (*)[MOVIE_DECODED_SLOT_SIZE])0)[gMovie_bOutputSlot]),
                  out->strip.w * out->strip.h / 2);
    } else {
        gMovie_bFrameDecoded = 1;
    }
}

/* The movie frame pipeline's .sdata state block, 0x8009B060 to 0x8009B073.
 *
 * movie_frame_pipeline.h already describes what these are and settled that
 * they form one family: every symbol was declared identically by each of its
 * users and none is named outside the pipeline. This file only gives that
 * block a definition, so the header stays the place the meaning is recorded.
 *
 * The block sits inside the 40-byte range at 0x8009B058 that the notes call
 * the scattered tail. Scattered is right about ownership and wrong about
 * structure: the range holds three separate owners, and this is the middle
 * one. main_menu_load_package_stage.c owns the eight bytes before it and
 * model_transfer_flags.h the twelve after, so both are left extracted and
 * this carve takes only the run that one family names.
 *
 * Every object is zero, which makes the section attributes load bearing
 * rather than decorative: a zero-initialised object is placed in .sbss, and
 * without the attribute all twenty bytes would leave .sdata at once and the
 * window would collapse. model_background.c relies on the same
 * thing for its three trailing zero words.
 *
 * The eight bytes are written as eight separate objects rather than an array
 * because that is how the header declares them and how every consumer reads
 * them -- none indexes across the run. Two of them, gMovie_bFrameDecoded and the
 * unassigned neighbour above this block, were never given linker aliases,
 * which is the same conclusion reached from the other direction. */

u8 gMovie_bRgb24 = 0;
u8 gMovie_bCopyDisplayOnStop = 0;
u8 gMovie_bFrameDecoded = 0;
u8 gMovie_bStreamEnded = 0;
u8 gMovie_bFadingOut = 0;
u8 gMovie_bCdVolume = 0;
u8 gMovie_bInputSlot = 0;
u8 gMovie_bOutputSlot = 0;
u32 gMovie_dwStartFrame = 0;
u32 gMovie_dwEndFrame = 0;
u32 gMovie_dwEndSector = 0;

void func_8005C374(s32 first, s32 second, s32 third)
{
    D_8009B4A0 = first;
    D_8009B4A1 = second;
    D_8009B4A2 = third;
}

#include "file_cd_helpers.h"
#include "file_constants.h"
#include "movie_open_stream.h"
#include "movie_stream_requests.h"
#include "movie_stream_ranges.h"
#include "file_transfer.h"
#include "../unmatched.h"
#include "movie_stream_requests_data.h"

/* Initialized data this unit owns at 0x8009151C: only the first movie carries a range; the
   rest are reached through the running sum, which is zero for all of them. */
MovieStreamRange gMovie_aStreamRanges[MOVIE_STREAM_COUNT] = {
#ifdef VERSION_EUROPE
    /* The European build gives its first six movies the same range. */
    { 0x0D84, 0x012F },
    { 0x0D84, 0x012F },
    { 0x0D84, 0x012F },
    { 0x0D84, 0x012F },
    { 0x0D84, 0x012F },
    { 0x0D84, 0x012F },
#else
    { 0x0C08, 0x012F },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
#endif
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
    { 0x0000, 0x0000 },
};

void Movie_OpenIndexedStream(s32 index, s32 arg1, s32 end_frame, s32 arg3, s32 arg4)
{
    CdlLOC position;
    MovieStreamRange *base;
    MovieStreamRange *entry;
    s32 i;
    s32 total;

    base = gMovie_aStreamRanges;
    entry = &base[index];
    if (entry->end_frame != 0)
        end_frame = entry->end_frame;
    i = 0;
    total = 0;
    if (index > 0) {
        do {
            total += base[i].sector_count;
            i++;
        } while (i < index);
    }
    CdIntToPos_8007E600(gFile_anLba[FILE_INDEX_MOVIE_STR] + total, &position);
    {
        MovieStreamRange *table;
        s32 value;

        value = CdPosToInt_8007E710(&position);
        table = gMovie_aStreamRanges;
        value += table[index].sector_count;
        Movie_OpenStream(&position, arg1, end_frame, value, arg3, arg4);
    }
}

s32 Movie_StartFileStream(
    const char *path,
    s32 start_frame,
    s32 end_frame,
    s32 arg3,
    s32 arg4
)
{
    CdlFILE *file = (CdlFILE *)gMovie_FileInfo;

    if (File_Exists(path, file) != 0)
        return -1;
    return Movie_OpenStream(
        &file->pos, start_frame, end_frame,
        CdPosToInt_8007E710(&file->pos) +
            ((u32)(file->size + (FILE_SECTOR_SIZE - 1)) >>
             FILE_SECTOR_SHIFT),
        arg3, arg4);
}

#include "file_cd_helpers.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libds.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

s32 File_Exists(const char *path, CdlFILE *file)
{
    DslFILE *result = DsSearchFile((DslFILE *)file, (char *)path);

    if (result == 0 || result == (DslFILE *)-1) {
        return -1;
    }
    return 0;
}

int Movie_StepPlayback(void)
{
    int result = Movie_DecodeAndPresentFrame();

    if (result != 0) {
        Movie_StopStream(0);
    }
    return result;
}

int Movie_MoveDisplayImage(int first, int second)
{
    DISPENV local;

    GetDispEnv(&local);
    while (IsIdleGPU(3) != 0) {}
    while (MoveImage2(&local.disp, first, second) != 0) {}
    return 0;
}

#include "../psyq/libpress.h"
#include "mdec_sync.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "movie_seek_and_start_read.h"
#include "movie_on_stream_end.h"

void Movie_StartFadeOut(void)
{
    gMovie_bFadingOut = 1;
}

s32 Movie_WaitFrameDecoded(void)
{
    s32 timeout = 0x800000;
    if (gMovie_bFrameDecoded == 0) {
        for (;;) {
            if (--timeout <= 0) {
                DecDCTReset(1);
                break;
            }
            if (gMovie_bFrameDecoded != 0) {
                break;
            }
        }
    }
    gMovie_bFrameDecoded = 0;
    return 0;
}

#define CD_LOCATION_BYTES(location) ((u8 *)(location))
#define DS_LOCATION_VIEW(location) ((DslLOC *)(location))

void Movie_SeekAndStartRead(CdlLOC *loc){while(!CdControlB(CdlSetloc,CD_LOCATION_BYTES(loc),0)){}while(!CdControlB(CdlSeekP,CD_LOCATION_BYTES(loc),0)){}while(!DsRead2(DS_LOCATION_VIEW(loc),0x1E0)){} }

void Movie_OnStreamEnd(void)
{
    gMovie_bStreamEnded = 1;
}
