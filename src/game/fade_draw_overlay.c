#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

#include "fade.h"

/* Full-screen fade / brightness overlay, drawn once per frame from
   func_8001306C's dispatcher.

   The fade state lives in the gFade_State record: byte 4 is the current
   level, byte 6 is the flag byte, bytes 0/1/2 are per-channel tint, and
   bytes 0xA..0x27 are 30 per-band levels. Fade_Update updates the state
   before the draw gate: active flag 0x80, or a nonzero D_8009B141 with
   level != 0xFF. Level 0xFF is not a universal completion sentinel.

   Rendering reuses a GsBOXF-compatible descriptor in PS1 scratchpad RAM
   at 0x1F8003C0. GsSortBoxFill builds the GPU packet from it:

     - flag bit 0 set: 30 stacked bands, each 320x8, stepping y by 8 for
       240 lines total -- one band per level byte at 0xA+i, each shaded
       0xFF - level. This is the banded/wipe variant.
     - without band mode, the tail submits one 320x240 box at (0,0).
       After the bands it returns unless flag 0x02 is set; that combined
       path reaches the tail with height 8 and y=240, not a full-screen
       box. The tail shades by 0xFF - level[4].
       If flag 0x10 is set the attribute switches to 0x50000000 and the
       three channels are shaded independently by colour[n] - level[4],
       clamped at 0, giving a tinted rather than grey fade.

   Bands always submit at depth 4. The tail also uses 4 unless flag 0x02
   is set, when it uses D_8009B140 (falling back to 0x3F when that is zero).

   Shape notes for anyone re-deriving this: the descriptor's 0x04 and 0x08
   words are each written whole (x+y and w+h together), while y and h are
   updated as halves, so those two go through casts. The tail reads the
   fade record through gFade_State and the tint block through the pointer
   set up for the Fade_Update call -- that is what makes gcc rematerialise
   the record address once for the tail instead of reusing $s2 throughout.
   Matches 0/117 with -G8 -msplit-addresses. */

/* GsBOXF-compatible descriptor at 0x1F8003C0, not a GPU packet. */
typedef struct {
    u32 tag;   /* 0x00  GsBOXF attribute: GsALON | GsATWO by default
                 (subtractive, so the box darkens), GsALON | GsAONE when
                 tinted (additive). */
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
extern s32 D_800E9D94[4];      /* [0] = ordering table the boxes sort into */
extern u8 D_8009B140;
extern u8 D_8009B141;

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
    Fade_Update(rec);
    flags = rec[6];
    if ((flags & 0x80) || (D_8009B141 != 0 && rec[4] != 0xFF)) {
        p = FADEBOX;
        p->tag = GsALON | GsATWO;
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
            if (!(gFade_State.flags & 2)) {
                return;
            }
        }

        depth = 4;
        if (gFade_State.flags & 2) {
            depth = D_8009B140;
            if (depth == 0) {
                depth = 0x3F;
            }
        }

        shade = 0xFF - gFade_State.level;
        p->b = (u8) shade;
        p->g = (u8) shade;
        p->r = (u8) shade;
        if (gFade_State.flags & 0x10) {
            p->tag = GsALON | GsAONE;
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
