#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "graphics_frame.h"
#include "main_frame.h"
#define ORDERING_TABLE_SLOT1_ARRAY
#include "ordering_tables.h"

#include "fade.h"
#include "../unmatched.h"

/* The complete fade overlay, setup, blocking, and wrapper runtime. All thirty
   contiguous functions operate on gFade_State and compile at
   gcc_2_8_1_g8_split. Fade_Update below needs a distinct assembler threshold;
   display_projection.c above is owned by the pending #3859 reclassification. */

/* Full-screen fade / brightness overlay, drawn once per frame from
   func_8001306C's dispatcher.

   The fade state lives in the gFade_State record: byte 4 is the current
   level, byte 6 is the flag byte, bytes 0/1/2 are per-channel tint, and
   bytes 0xA..0x27 are 30 per-band levels. Fade_Update updates the state
   before the draw gate: FADE_FLAG_ACTIVE, or a nonzero D_8009B141 with
   level != 0xFF. Level 0xFF is not a universal completion sentinel.

   Rendering reuses a GsBOXF-compatible descriptor in PS1 scratchpad RAM
   at 0x1F8003C0. GsSortBoxFill builds the GPU packet from it:

     - FADE_FLAG_BANDED: 30 stacked bands, each 320x8, stepping y by 8 for
       240 lines total -- one band per level byte at 0xA+i, each shaded
       0xFF - level. This is the banded/wipe variant.
     - without band mode, the tail submits one 320x240 box at (0,0).
       After the bands it returns unless FADE_FLAG_KEEP_OVERLAY is set; that combined
       path reaches the tail with height 8 and y=240, not a full-screen
       box. The tail shades by 0xFF - level[4].
       If FADE_FLAG_TINTED is set the attribute switches to 0x50000000 and the
       three channels are shaded independently by colour[n] - level[4],
       clamped at 0, giving a tinted rather than grey fade.

   Bands always submit at depth 4. The tail also uses 4 unless
   FADE_FLAG_KEEP_OVERLAY is set, when it uses D_8009B140 (falling back to
   0x3F when that is zero).

   Shape notes for anyone re-deriving this: the GsBOXF descriptor's 0x04 and
   0x08 words are each written whole (x+y and w+h together), while y and h
   are updated as halves, so both access widths go through casts. The tail
   reads the fade record through gFade_State and the tint block through the
   pointer set up for the Fade_Update call -- that is what makes gcc
   rematerialise the record address once for the tail instead of reusing $s2
   throughout. Matches 0/117 with -G8 -msplit-addresses. */

/* The GsBOXF descriptor lives in scratchpad RAM, not in a GPU packet. */
#define FADEBOX_Y(p) (*(u16 *)&(p)->y)
#define FADEBOX_H(p) (*(s16 *)&(p)->h)
#define FADEBOX ((GsBOXF *)0x1F8003C0)

void Fade_DrawOverlay(void) {
    GsBOXF *p;
    u8 *rec;
    GsOT *ot;
    s32 i;
    s32 band;
    s32 shade;
    s32 tint;
    s32 depth;
    u8 flags;

    rec = D_800E9EC8_arr;
    Fade_Update((FadeTransitionState *)rec);
    flags = rec[6];
    if ((flags & FADE_FLAG_ACTIVE) ||
        (D_8009B141 != 0 && rec[4] != 0xFF)) {
        p = FADEBOX;
        p->attribute = GsALON | GsATWO;
        *(u32 *)&p->w = (FADE_SCREEN_HEIGHT << 16) | FADE_SCREEN_WIDTH;
        *(u32 *)&p->x = 0;
        ot = D_800E9D94[0];

        if (flags & FADE_FLAG_BANDED) {
            FADEBOX_H(p) = FADE_BAND_HEIGHT;
            for (i = 0; i < FADE_BAND_COUNT; i++) {
                u8 *lvl = rec + i;
                band = 0xFF - lvl[0xA];
                p->b = (u8) band;
                p->g = (u8) band;
                p->r = (u8) band;
                GsSortBoxFill(p, ot, 4);
                FADEBOX_Y(p) = FADEBOX_Y(p) + FADE_BAND_HEIGHT;
            }
            if (!(gFade_State.flags & FADE_FLAG_KEEP_OVERLAY)) {
                return;
            }
        }

        depth = 4;
        if (gFade_State.flags & FADE_FLAG_KEEP_OVERLAY) {
            depth = D_8009B140;
            if (depth == 0) {
                depth = 0x3F;
            }
        }

        shade = 0xFF - gFade_State.level;
        p->b = (u8) shade;
        p->g = (u8) shade;
        p->r = (u8) shade;
        if (gFade_State.flags & FADE_FLAG_TINTED) {
            p->attribute = GsALON | GsAONE;
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
        GsSortBoxFill(p, ot, depth);
    }
}

void func_800156B8(s32 arg0)
{
    u8 *p = (u8 *)&gFade_State;
    s32 i;

    for (i = 0x1D; i >= 0; i--) {
        *(p + i + 0xA) = arg0;
    }
}

void func_800156DC(void)
{
    FadeTransitionState *state;
    D_8009B145 = 1;
    Fade_InitOut();
    state = &gFade_State;
    state->flags = 0;
    state->level = 0;
    D_8009B142 = 0xFF;
    D_8009B143 = 0xFF;
    D_8009B144 = 0xFF;
    func_80015D0C();
}

void func_8001572C(void)
{
    FadeTransitionState *state;
    int value;
    if (D_8009B145 != 0) {
        value = 0xFFFFFF;
        state = &gFade_State;
        *(s32 *)state = value;
        state->flags = FADE_FLAG_ACTIVE | FADE_FLAG_TINTED;
        state->step = 0xC;
        D_8009B14C = 1;
        D_8009B144 = 1;
        D_8009B14B = 1;
        D_8009B143 = 1;
        D_8009B14A = 1;
        D_8009B142 = 1;
    }
}

void Fade_InitIn(void)
{
    FadeTransitionState *state = &gFade_State;

    state->target_level = 0xFF;
    state->flags = FADE_FLAG_ACTIVE;
    D_8009B141 &= ~FADE_ORDERING_TABLE_HIDE_SECONDARY;
    state->field_08 = 0;
    func_800156B8(state->level);
    state->step = 0xC;
    func_8001572C();
}

void Fade_StartIn(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->step = 8;
    state->flags |= FADE_FLAG_BANDED;
    func_8001572C();
}

void Fade_InitInColor(int color)
{
    FadeTransitionState *state;

    if (color == 0xFFFFFF) {
        D_8009B145 = 1;
    }
    *(s32 *)&gFade_State = color;
    Fade_InitIn();
    state = &gFade_State;
    state->flags |=
        FADE_FLAG_TINTED | FADE_FLAG_RESTORE_TINT_AFTER_BLACK;
    func_8001572C();
}

void func_80015870(void)
{
    FadeTransitionState *state;
    int color;

    if (D_8009B145 != 0) {
        color = 0xFFFFFF;
        state = &gFade_State;
        *(s32 *)state = color;
        state->flags = FADE_FLAG_ACTIVE | FADE_FLAG_TINTED |
                       FADE_FLAG_RESTORE_TINT_AFTER_BLACK;
        state->step = 0xC;
        D_8009B14A = 0xFF;
        D_8009B14B = 0xFF;
        D_8009B14C = 0xFF;
    }
}

void Fade_InitOut(void)
{
    FadeTransitionState *state = &gFade_State;

    state->field_08 = 0xFF;
    state->target_level = 0;
    state->flags = FADE_FLAG_ACTIVE;
    func_800156B8(state->level);
    state->step = 0xC;
    func_80015870();
}

void Fade_StartOut(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->step = 8;
    state->flags |= FADE_FLAG_BANDED;
    func_80015870();
}

void Fade_InitOutColor(int color)
{
    FadeTransitionState *state;

    if (color == 0xFFFFFF) {
        D_8009B145 = 1;
    }
    *(s32 *)&gFade_State = color;
    Fade_InitOut();
    state = &gFade_State;
    state->flags |=
        FADE_FLAG_TINTED | FADE_FLAG_RESTORE_TINT_AFTER_BLACK;
    func_80015870();
}

void Fade_Wait(void)
{
    FadeTransitionState *state = &gFade_State;

    do {
        func_80012D4C();
    } while (state->flags & FADE_FLAG_ACTIVE);
}

void Fade_WaitInitIn(void)
{
    Fade_InitIn();
    Fade_Wait();
}

void Fade_WaitIn(void)
{
    Fade_StartIn();
    Fade_Wait();
}

void Fade_WaitInitInColor(s32 color)
{
    Fade_InitInColor(color);
    Fade_Wait();
}

void func_80015A50(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY;
    func_8001572C();
    Fade_Wait();
}

void func_80015A94(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY |
                    FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE;
    func_8001572C();
    Fade_Wait();
}

void Fade_WaitInitOut(void)
{
    Fade_InitOut();
    Fade_Wait();
}

void Fade_WaitOut(void)
{
    Fade_StartOut();
    Fade_Wait();
}

void Fade_WaitInitOutColor(s32 color)
{
    Fade_InitOutColor(color);
    Fade_Wait();
}

void func_80015B50(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY;
    func_80015870();
    Fade_Wait();
}

void func_80015B94(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY |
                    FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE;
    func_80015870();
    Fade_Wait();
}

void Fade_SetTargetLevel(s32 value, s32 flags)
{
    FadeTransitionState *state = &gFade_State;

    state->target_level = value;
    state->flags = flags | FADE_FLAG_ACTIVE;
}

void Fade_SetLevel(s32 value)
{
    FadeTransitionState *state = &gFade_State;

    state->level = value;
    state->target_level = value;
    state->flags = FADE_FLAG_ACTIVE;
}

void func_80015C0C(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY;
    func_8001572C();
}

void func_80015C48(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY |
                    FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE;
    func_8001572C();
}

void func_80015C84(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY;
    func_80015870();
}

void func_80015CC0(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY |
                    FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE;
    func_80015870();
}

void func_80015CFC(void)
{
    D_8009B141 = FADE_ORDERING_TABLE_ACTIVE;
}

void func_80015D0C(void)
{
    D_8009B141 = 0;
}
