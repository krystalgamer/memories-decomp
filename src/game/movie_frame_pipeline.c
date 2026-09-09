#include "../types.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libpress.h"
#include "../psyq/libcd.h"
#include "graphics_constants.h"
#include "mdec_sync.h"
#include "sound_mix.h"

/* The movie player, in image order: the stop path that tears the stream down
   and repaints the screen, the three stages that decode and present one frame,
   and the setter for the three bytes at D_8009B4A0. The five are contiguous
   and are the whole gcc_2_8_1_g8 run between func_8005B85C and
   movie_stream_requests.c. */

extern u8 D_8009B142 __attribute__((section(".data")));
extern u8 D_8009B143 __attribute__((section(".data")));
extern u8 D_8009B144 __attribute__((section(".data")));
extern u8 D_8009B4A0;
extern u8 D_8009B4A1;
extern u8 D_8009B4A2;
/* Four bytes, so at -G8 a plain declaration would land in sdata and both
   accesses would come out gp-relative; retail reaches this one through
   lui %hi / %lo. */
extern s32 D_800F5D44 __attribute__((section(".data")));

extern void func_8005C62C(CdlLOC *loc);

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
    u8 *frame;
    u8 *display;
    u8 *rects;
    u8 *out;
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

    frame = D_8009B498 + 0x40000;
    display = D_8009B498 + 0x40000;
    x = env.clip.x +
        ((D_8009B060 != 0 ? 0x1E0 : 0x140) - *(s16 *)(frame + 0x2424)) / 2;
    side = D_8009B060;
    slot = D_8009B066;
    *(s16 *)(display + 0x2428) = x;
    *(s16 *)(frame + 0x2420) = x;

    rects = D_8009B498 + 0x40000;
    y = *(u16 *)&env.clip.y + (0xF0 - *(s16 *)(rects + 0x2426)) / 2;
    *(s16 *)(rects + 0x242A) = y;
    *(s16 *)(rects + 0x2422) = y;
    DecDCTin((u32 *)(D_8009B498 + 0x1B000 + slot * 0xE000), side);

    out = D_8009B498 + 0x40000;
    DecDCTout((u32 *)(D_8009B498 + 0x37000 + D_8009B067 * 0x2D00),
              *(s16 *)(out + 0x242C) * *(s16 *)(out + 0x242E) / 2);

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
    u8 *rects;
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
                func_8005C62C(&D_8009B49C);
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

    rects = D_8009B498 + 0x40000;
    width = hdr->width;
    frame_width = D_8009B060 != 0 ? width * 3 / 2 : width;
    columns = 0x10;
    *(u16 *)(rects + 0x2424) = frame_width;

    /* Reading the height into a local before the mode byte is what puts
       the work-area pointer ahead of the height in local-alloc. */
    header = hdr;
    rects = D_8009B498 + 0x40000;
    height = header->height;
    wide = D_8009B060;
    *(u16 *)(rects + 0x2426) = height;
    if (wide != 0) {
        columns = 0x18;
    }
    base = ring;
    *(u16 *)(rects + 0x242C) = columns;
    *(u16 *)(rects + 0x242E) = header->height;
    StFreeRing(base);
    return 0;
}

/* Feeds the MDEC one strip of the current movie frame. The strip header at
 * D_8009B498 + 0x42428 is copied into the ring slot for D_8009B067, that slot
 * and the matching decoded-output buffer go to LoadImage, the slot index
 * advances modulo four, and the accumulated strip height at +0x42428 grows by
 * the strip's own height. While the accumulation is still short of the frame
 * rect at +0x42420 the next strip is queued with DecDCTout; otherwise
 * D_8009B062 marks the frame complete.
 *
 * func_8005BE3C is the sibling to read for the conventions here: a fresh
 * pointer local is assigned D_8009B498 + 0x40000 for each group of accesses
 * rather than one being shared, which is what makes CSE collapse the repeated
 * global read into a register copy while still rebuilding the address
 * arithmetic. Writing dst off D_8009B498 with its own +0x40000, rather than
 * off src, is what keeps retail's two separate additions of that constant.
 *
 * The modulo is written out because GCC's own expansion of % 4 is correct but
 * lands its three values in the wrong registers, and only one of the three can
 * be named while the operator is used. Written out, all three are nameable. */
void func_8005C1F4(void) {
    u8 *src;
    u8 *dst;
    s32 idx;
    s32 sum;
    /* rem holds the new slot index and nxt the un-wrapped one. A pinned local
       only takes its register for free when something can write it there
       directly - a load, or the store that consumes it - so the two operands
       of the subtraction are pinned and tmp, which is only ever copied, is
       left alone. */
    register s32 rem asm("$3");
    register s32 nxt asm("$5");
    s32 tmp;
    register s32 add asm("$4");
    u8 *out;

    if (D_8009B060 != 0) {
        if (D_800F5D44 != 0) {
            StCdInterrupt();
            D_800F5D44 = 0;
        }
    }
    dst = D_8009B498 + D_8009B067 * 8 + 0x40000;
    src = D_8009B498 + 0x40000;
    /* One eight-byte aggregate: two four-byte copies would issue load, store,
       load, store with a load-delay nop after each load, where retail pairs
       both lwl/lwr and then both swl/swr through two temporaries. */
    *(RECT *)(dst + 0x2400) = *(RECT *)(src + 0x2428);
    idx = D_8009B067;
    LoadImage((RECT *)(D_8009B498 + 0x42400 + idx * 8),
              (u32 *)(D_8009B498 + 0x37000 + idx * 0x2D00));
    nxt = D_8009B067 + 1;
    tmp = nxt;
    if (nxt < 0) {
        tmp = D_8009B067 + 4;
    }
    rem = nxt - (tmp & ~3);
    D_8009B067 = rem;
    out = D_8009B498 + 0x40000;
    /* Loaded in two statements so +0x2428 is read before +0x242C, which
       naming the second one for its pin would otherwise reverse. */
    sum = *(u16 *)(out + 0x2428);
    add = *(u16 *)(out + 0x242C);
    sum = sum + add;
    *(s16 *)(out + 0x2428) = sum;
    if ((s16)sum < *(s16 *)(out + 0x2420) + *(s16 *)(out + 0x2424)) {
        DecDCTout((u32 *)(D_8009B498 + 0x37000 + D_8009B067 * 0x2D00),
                  *(s16 *)(out + 0x242C) * *(s16 *)(out + 0x242E) / 2);
    } else {
        D_8009B062 = 1;
    }
}

void func_8005C374(s32 first, s32 second, s32 third)
{
    D_8009B4A0 = first;
    D_8009B4A1 = second;
    D_8009B4A2 = third;
}
