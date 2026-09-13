#define D_8009B0D8_IN_DATA
#include "../types.h"
#include "graphics_frame.h"
#include "fade.h"

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
        func_80015CFC();
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
        func_80015CFC();
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
        func_80015D0C();
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
    func_80015CFC();
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
