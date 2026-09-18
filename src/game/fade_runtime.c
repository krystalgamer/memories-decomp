#define D_8009B0D8_IN_DATA
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

/* The per-frame fade step and the complete fade overlay, setup, blocking,
   and wrapper runtime. All thirty-one contiguous functions operate on
   gFade_State and compile at gcc_2_8_1_g8_split_comm: Fade_Update's tint
   bytes below are tentative definitions that must stay common, and the other
   thirty functions are byte-identical with or without the comm section.
   display_projection.c after the unit is owned by the pending #3859
   reclassification. */

/* Tentative definitions keep these bytes common and gp-relative, supplying
   the three load-delay nops in the tint copy. c_symbols.ld overrides them. */
u8 D_8009B142;
u8 D_8009B143;
u8 D_8009B144;

void Fade_Update(FadeTransitionState *p)
{
    u8 f;
    u8 g;
    s32 lvl;
    s32 tgt;
    s32 delta;

    if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
        return;
    }
    if ((D_8009B141 & FADE_ORDERING_TABLE_HIDE_SECONDARY) == 0) {
        Fade_EnableOrderingTables();
    }
    delta = p->step * *(volatile s32 *)&D_8009B0D8;
    lvl = p->level;
    tgt = p->target_level;
    if (lvl != tgt) {
        goto step;
    }
    f = gFade_State.flags;
    gFade_State.flags = f & ~FADE_FLAG_ACTIVE;
    if (lvl == 0xFF) {
        gFade_State.flags =
            f & ~(FADE_FLAG_ACTIVE | FADE_FLAG_KEEP_OVERLAY |
                  FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE);
        Fade_EnableOrderingTables();
        D_8009B140 = 0;
        D_8009B145 = 0;
        D_8009B144 = D_8009B14C;
        D_8009B143 = D_8009B14B;
        D_8009B142 = D_8009B14A;
    }
    if (lvl != 0) {
        return;
    }
    f = gFade_State.flags;
    if (f & FADE_FLAG_KEEP_OVERLAY) {
        if ((f & FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE) == 0) {
            return;
        }
        D_8009B141 = FADE_ORDERING_TABLE_HIDE_SECONDARY;
    } else {
        Fade_DisableOrderingTables();
    }
    D_8009B144 = D_8009B14C;
    D_8009B143 = D_8009B14B;
    D_8009B142 = D_8009B14A;
    g = gFade_State.flags;
    if ((g & FADE_FLAG_TINTED) == 0) {
        return;
    }
    if ((g & FADE_FLAG_RESTORE_TINT_AFTER_BLACK) == 0) {
        return;
    }
    gFade_State.flags = g & ~FADE_FLAG_RESTORE_TINT_AFTER_BLACK;
    D_8009B144 = p->tint_r;
    D_8009B143 = p->tint_g;
    D_8009B142 = p->tint_b;
    Fade_EnableOrderingTables();
    gFade_State.flags = gFade_State.flags | FADE_FLAG_ACTIVE;
    return;
step:
    if (gFade_State.flags & FADE_FLAG_BANDED) {
        Fade_StepBands();
        return;
    }
    if (lvl < tgt) {
        lvl += delta;
        if (lvl >= tgt) {
            lvl = tgt;
        }
    } else {
        lvl -= delta;
        if (tgt >= lvl) {
            lvl = tgt;
        }
    }
    D_800E9ECC[0] = lvl;
}

/* Full-screen fade / brightness overlay, drawn once per frame from
   the Main_RunFrameServices dispatcher.

   The fade state lives in the gFade_State record: byte 4 is the current
   level, byte 6 is the flag byte, bytes 0/1/2 are per-channel tint, and
   bytes 0xA..0x27 are 30 per-band levels. Fade_Update updates the state
   before the draw gate: FADE_FLAG_ACTIVE, or a nonzero D_8009B141 with
   level != 0xFF. Level 0xFF is not a universal completion sentinel.

   Rendering reuses a GsBOXF-compatible descriptor in PS1 scratchpad RAM
   at 0x1F8003C0. GsSortBoxFill builds the GPU packet from it:

     - FADE_FLAG_BANDED: 30 stacked bands, each 320x8, stepping y by 8 for
       240 lines total -- one band per band_levels[i], each shaded
       0xFF - that level. This is the banded/wipe variant.
     - without band mode, the tail submits one 320x240 box at (0,0).
       After the bands it returns unless FADE_FLAG_KEEP_OVERLAY is set; that combined
       path reaches the tail with height 8 and y=240, not a full-screen
       box. The tail shades by 0xFF - level.
       If FADE_FLAG_TINTED is set the attribute switches to 0x50000000 and the
       three channels are shaded independently by tint_r/g/b - level,
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
    FadeTransitionState *rec;
    GsOT *ot;
    s32 i;
    s32 band;
    s32 shade;
    s32 tint;
    s32 depth;
    u8 flags;

    rec = (FadeTransitionState *)D_800E9EC8_arr;
    Fade_Update(rec);
    flags = rec->flags;
    if ((flags & FADE_FLAG_ACTIVE) ||
        (D_8009B141 != 0 && rec->level != 0xFF)) {
        p = FADEBOX;
        p->attribute = GsALON | GsATWO;
        *(u32 *)&p->w = (FADE_SCREEN_HEIGHT << 16) | FADE_SCREEN_WIDTH;
        *(u32 *)&p->x = 0;
        ot = D_800E9D94[0];

        if (flags & FADE_FLAG_BANDED) {
            FADEBOX_H(p) = FADE_BAND_HEIGHT;
            for (i = 0; i < FADE_BAND_COUNT; i++) {
                band = 0xFF - rec->band_levels[i];
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
            tint = rec->tint_r - rec->level;
            if (tint < 0) tint = 0;
            p->r = (u8) tint;
            tint = rec->tint_g - rec->level;
            if (tint < 0) tint = 0;
            p->g = (u8) tint;
            tint = rec->tint_b - rec->level;
            if (tint < 0) tint = 0;
            p->b = (u8) tint;
        }
        GsSortBoxFill(p, ot, depth);
    }
}

void Fade_FillBandLevels(s32 level)
{
    u8 *p = (u8 *)&gFade_State;
    s32 i;

    for (i = FADE_BAND_COUNT - 1; i >= 0; i--) {
        *(p + i + (u32)&((FadeTransitionState *)0)->band_levels) = level;
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
    Fade_DisableOrderingTables();
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
    Fade_FillBandLevels(state->level);
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
    Fade_FillBandLevels(state->level);
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
        Main_AdvanceFrame();
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

void Fade_StartInKeepOverlay(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY;
    func_8001572C();
}

void Fade_StartInKeepOverlayAndHideSecondaryTables(void)
{
    FadeTransitionState *state;

    Fade_InitIn();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY |
                    FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE;
    func_8001572C();
}

void Fade_StartOutKeepOverlay(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY;
    func_80015870();
}

void Fade_StartOutKeepOverlayAndHideSecondaryTables(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &gFade_State;
    state->flags |= FADE_FLAG_KEEP_OVERLAY |
                    FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE;
    func_80015870();
}

void Fade_EnableOrderingTables(void)
{
    D_8009B141 = FADE_ORDERING_TABLE_ACTIVE;
}

void Fade_DisableOrderingTables(void)
{
    D_8009B141 = 0;
}
