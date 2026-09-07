#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libpress.h"

extern u8 D_8009B060;
extern u8 D_8009B063;
extern u8 D_8009B064;
extern u8 D_8009B066;
extern u32 D_8009B068;
extern u32 D_8009B06C;
extern u32 D_8009B070;
extern u8 *D_8009B498;
extern CdlLOC D_8009B49C;

extern void func_8005C62C(CdlLOC *loc);

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
