#define D_8009B142_IN_DATA
#include "../types.h"
#include "graphics_frame.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libpress.h"
#include "../psyq/libcd.h"
#include "file_cd_helpers.h"
#include "func_8005C62C.h"
#include "graphics_constants.h"
#include "mdec_sync.h"
#include "sound_mix.h"

/* The start of the movie player, in image order: the stop path that tears the
   stream down and repaints the screen, then two of the three stages that
   decode and present one frame. The three are contiguous and follow
   func_8005B85C. The third stage, func_8005C1F4, comes next and is now a
   candidate in src/candidates/func_8005C1F4.c. */

s32 func_8005BB7C(s32 arg0) {
    RECT rect;
    u8 buf[0x3C0];
    s32 i;
    s32 m;
    u8 *q;

    DrawSync(0);
    func_80044F58(0);
    /* volatile: retail sets a0 = 0 above this store and leaves the jal's
       delay slot empty, which only a store that cannot enter a slot gives. */
    *(volatile u8 *)&D_8009B063 = 1;
    DecDCToutCallback(0);
    StUnSetRing();
    while (CdControlB(9, 0, 0) == 0) {
    }
    if (D_8009B061 != 0 || arg0 != 0) {
        func_8005C568(0, 0x100);
    }
    if (D_8009B060 != 0) {
        i = 0;
        m = GsGetActiveBuff();
        q = buf;
        for (; i < 0x3C0; i += 3) {
            *q++ = D_8009B144;
            *q++ = D_8009B143;
            *q++ = D_8009B142;
        }
        rect.x = 0;
        i = 0;
        rect.y = m << 8;
        rect.w = 0x1E0;
        rect.h = 1;
        for (; i < D_800FE0D4; i++) {
            LoadImage(&rect, (u32 *)buf);
            DrawSync(0);
            rect.y++;
        }
        rect.x = 0x140;
        rect.y = 0;
        rect.h = 1;
        rect.w = D_800FE0D0;
        for (i = 0; i < D_800FE0D4; i++) {
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
        for (; i < D_800FE0D4; i++) {
            LoadImage(&rect, (u32 *)buf);
            DrawSync(0);
            rect.y++;
        }
        DrawSync(0);
        VSync(0);
        D_800FE0CC = 1;
        GsSwapDispBuff();
        GsDefDispBuff(0, 0, 0x140, 0);
        GsInitGraph2(GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT, 4, 1, 0);
        rect.x = 0;
        rect.y = 0;
        rect.w = *(s32 *)&D_800FE0D0 * 2;
        rect.h = *(u16 *)&D_800FE0D4;
        ClearImage(&rect, D_8009B144, D_8009B143, D_8009B142);
        D_8009B060 = 0;
    }
    return 0;
}

/* Decodes and presents one movie frame. D_8009B063 marks the stream as
 * finished and D_8009B064 as fading out, in which case the fade level in
 * D_8009B065 drops by 0x40 a frame and the stream ends once it reaches zero.
 * The work area at D_8009B498 + 0x40000 carries two RECTs, the frame rect at
 * +0x2420 and the display rect at +0x2428. Both are centred against the
 * current drawing clip: horizontally in the 320- or 640-wide screen chosen by
 * D_8009B060, vertically in 240. The ring slots are 0xE000 apart from
 * +0x1B000 for the coded data and 0x2D00 apart from +0x37000 for the decoded
 * output. Returns non-zero once the stream is done. */
s32 func_8005BE3C(void) {
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

    if (D_8009B063 != 0) {
        return 1;
    }
    if (D_8009B064 != 0) {
        if (D_8009B065 == 0) {
            return 1;
        }
        fade = D_8009B065 - 0x40;
        if (fade < 0) {
            fade = 0;
        }
        D_8009B065 = fade;
        func_80044F58(D_8009B065);
    }
    /* volatile: retail sets up the following call's argument above this store
       and leaves its delay slot empty, which only a store that cannot be
       moved into a slot gives. */
    *(volatile u8 *)&D_8009B062 = 0;
    GetDrawEnv(&env);

    frame = (MovieWorkArea *)(D_8009B498 + 0x40000);
    display = (MovieWorkArea *)(D_8009B498 + 0x40000);
    x = env.clip.x +
        ((D_8009B060 != 0 ? 0x1E0 : 0x140) - frame->frame.w) / 2;
    side = D_8009B060;
    slot = D_8009B066;
    /* frame and display hold one address, so typed these two stores
       hit provably distinct members and GCC reschedules them against
       the surrounding gp loads. Through u8 * it cannot tell them
       apart, which is the order retail has; kept opaque, and re-based
       so retyping the cursor does not rescale the offsets. */
    *(s16 *)((u8 *)display + 0x2428) = x;
    *(s16 *)((u8 *)frame + 0x2420) = x;

    rects = (MovieWorkArea *)(D_8009B498 + 0x40000);
    y = *(u16 *)&env.clip.y + (0xF0 - rects->frame.h) / 2;
    rects->strip.y = y;
    rects->frame.y = y;
    DecDCTin((u32 *)(D_8009B498 + 0x1B000 + slot * 0xE000), side);

    out = (MovieWorkArea *)(D_8009B498 + 0x40000);
    DecDCTout((u32 *)(D_8009B498 + 0x37000 + D_8009B067 * 0x2D00),
              out->strip.w * out->strip.h / 2);

    result = func_8005BFC8(1);
    if (result != 0) {
        return result;
    }
    func_8005C5D4();
    return 0;
}

/* Fetches the next movie frame from the CD stream ring and runs the VLC
 * decode on it. D_8009B063 marks the stream as finished and D_8009B064 as
 * fading out; both are set from the sector position and frame count in the
 * STR header against the limits in D_8009B070 and D_8009B06C. While no
 * sector is ready the loop waits on VSync, and after 60 frames without one
 * it re-issues the read at D_8009B49C, at once when resync is requested and
 * after ten timeouts otherwise. Frames behind D_8009B068 are dropped. The
 * decoded output alternates between the two coded slots at D_8009B498 +
 * 0x1B000, the VLC table being the start of the work area, and the frame
 * rect at D_8009B498 + 0x42424 takes the header's width (three halves of it
 * in the 640-wide mode chosen by D_8009B060), height and column count.
 * Returns non-zero once the stream is done. */
s32 func_8005BFC8(s32 resync) {
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

    if (D_8009B063 != 0) {
        return 1;
    }
    set = 1;
    if (resync != 0) {
        D_8009B068 = StGetBackloc(&D_8009B49C);
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
            if (D_8009B063 != 0) {
                goto fail;
            }
            timeouts++;
            if (resync != 0 || timeouts >= 10) {
                func_8005C62C((s32)&D_8009B49C);
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

        if (CdPosToInt(&hdr->loc) + hdr->nSectors * 2 >= D_8009B070) {
            D_8009B063 = set;
        }
        if (D_8009B068 >= D_8009B06C) {
            D_8009B064 = set;
        }
        if (hdr->frameCount >= D_8009B06C) {
            D_8009B064 = set;
        }
        if (D_8009B063 == 0) {
            if (hdr->frameCount < D_8009B068) {
                StFreeRing(ring);
                continue;
            }
        }
        break;
    }
    D_8009B066 = (D_8009B066 + 1) & 1;
    DecDCTvlc2(ring, (u32 *)(D_8009B498 + 0x1B000 + D_8009B066 * 0xE000),
               (u16 *)D_8009B498);

    rects = (MovieWorkArea *)(D_8009B498 + 0x40000);
    width = hdr->width;
    frame_width = D_8009B060 != 0 ? width * 3 / 2 : width;
    columns = 0x10;
    *(u16 *)&rects->frame.w = frame_width;

    /* Reading the height into a local before the mode byte is what puts
       the work-area pointer ahead of the height in local-alloc. */
    header = hdr;
    rects = (MovieWorkArea *)(D_8009B498 + 0x40000);
    height = header->height;
    wide = D_8009B060;
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

