#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

#include "fade.h"

/* Full-screen fade / brightness overlay, drawn once per frame from
   func_8001306C's dispatcher.

   The fade state lives in the D_800E9EC8 record: byte 4 is the current
   fade level (0 = no fade, 0xFF = fully faded, and 0xFF also doubles as
   the "finished" sentinel), byte 6 is the flag word, bytes 0/1/2 are a
   per-channel target colour, and bytes 0xA..0x27 are 30 per-band levels.
   func_80015310 advances that state; this function only renders it.

   Rendering builds one GPU flat box-fill primitive in PS1 scratchpad RAM
   at 0x1F8003C0 and submits it through GsSortBoxFill:

     - flag bit 0 set: 30 stacked bands, each 320x8, stepping y by 8 for
       240 lines total -- one band per level byte at 0xA+i, each shaded
       0xFF - level. This is the banded/wipe variant.
     - then (or instead) a single 320x240 box shaded 0xFF - level[4].
       If flag bit 0x10 is set the tag switches to 0x50000000 and the
       three channels are shaded independently by colour[n] - level[4],
       clamped at 0, giving a tinted rather than grey fade.

   The submit "depth" argument is normally 4, but when D_800E9ECE bit 1 is
   set it comes from D_8009B140 (falling back to 0x3F when that is zero).

   Shape notes for anyone re-deriving this: the primitive's 0x04 and 0x08
   words are each written whole (x+y and w+h together), while y and h are
   updated as halves, so those two go through casts. The tail reads the
   fade record through D_800E9EC8 and the tint block through the pointer
   set up for the func_80015310 call -- that is what makes gcc rematerialise
   the record address once for the tail instead of reusing $s2 throughout.
   Matches 0/117 with -G8 -msplit-addresses. */

/* GPU box-fill primitive assembled in scratchpad at 0x1F8003C0. */
typedef struct {
    u32 tag;   /* 0x00  0x60000000 flat box, 0x50000000 tinted box */
    u32 xy;    /* 0x04  x = low half, y = high half (stepped per band) */
    u32 wh;    /* 0x08  w = low half (320), h = high half (240, or 8) */
    u8  r;     /* 0x0C */
    u8  g;     /* 0x0D */
    u8  b;     /* 0x0E */
} FadeBox;

/* y (0x06) and h (0x0A) are the upper halves of xy and wh. */
#define FADEBOX_Y(p) (*(u16 *)((u8 *)(p) + 6))
#define FADEBOX_H(p) (*(s16 *)((u8 *)(p) + 0xA))

#define FADEBOX ((FadeBox *) 0x1F8003C0)

/* D_8009B140/41 are reached through $gp (small data), so they are plain
   scalars. The 0x800E9xxx globals are reached lui/%lo (absolute), so each
   is declared oversized -- a size over 8 bytes keeps it out of the -G8
   small-data section. */
extern u8 D_800E9ECE[16];
extern s32 D_800E9D94[4];      /* [0] = ordering table the boxes sort into */
extern u8 D_8009B140;
extern u8 D_8009B141;

extern void func_80015310(u8 *);

void Fade_DrawOverlay(void) {
    FadeBox *p;
    u8 *rec;
    s32 ot;
    s32 i;
    s32 band;
    s32 shade;
    s32 tint;
    s32 depth;
    u8 flags;

    rec = D_800E9EC8_arr;
    func_80015310(rec);
    flags = rec[6];
    if ((flags & 0x80) || (D_8009B141 != 0 && rec[4] != 0xFF)) {
        p = FADEBOX;
        p->tag = 0x60000000;
        p->wh = (FADE_SCREEN_HEIGHT << 16) | FADE_SCREEN_WIDTH;
        p->xy = 0;
        ot = D_800E9D94[0];

        if (flags & 1) {
            FADEBOX_H(p) = FADE_BAND_HEIGHT;
            for (i = 0; i < FADE_BAND_COUNT; i++) {
                u8 *lvl = rec + i;
                band = 0xFF - lvl[0xA];
                p->b = (u8) band;
                p->g = (u8) band;
                p->r = (u8) band;
                GsSortBoxFill((GsBOXF *)p, (GsOT *)ot, 4);
                FADEBOX_Y(p) = FADEBOX_Y(p) + FADE_BAND_HEIGHT;
            }
            if (!(D_800E9ECE[0] & 2)) {
                return;
            }
        }

        depth = 4;
        if (D_800E9ECE[0] & 2) {
            depth = D_8009B140;
            if (depth == 0) {
                depth = 0x3F;
            }
        }

        shade = 0xFF - D_800E9EC8.level;
        p->b = (u8) shade;
        p->g = (u8) shade;
        p->r = (u8) shade;
        if (D_800E9EC8.flags & 0x10) {
            p->tag = 0x50000000;
            tint = rec[0] - rec[4];
            if (tint < 0) tint = 0;
            p->r = (u8) tint;
            tint = rec[1] - rec[4];
            if (tint < 0) tint = 0;
            p->g = (u8) tint;
            tint = rec[2] - rec[4];
            if (tint < 0) tint = 0;
            p->b = (u8) tint;
        }
        GsSortBoxFill((GsBOXF *)p, (GsOT *)ot, depth);
    }
}
