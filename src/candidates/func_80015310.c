/* Reclassified from matching_c (#3859). This was src/game/fade_update.c,
 * byte-exact only under gcc_2_8_1_cc_g8_as_g1_split_comm, whose compiler
 * and assembler disagree about small data (GCC -G8, MASPSX -G1). Under
 * gcc_2_8_1_g8_split_comm, the same flags at a single threshold, it is 116
 * instructions against the target's 117, opcode distance 1. That profile
 * also keeps the three tentative definitions below common rather than
 * .sbss, which a candidate may not allocate; spelled extern instead the
 * function drops to 113 instructions. The source below is the match,
 * unchanged apart from its include paths. */

#include "../types.h"
#include "../game/graphics_frame.h"
#include "../game/fade.h"
#include "../unmatched.h"

/* Spelled as an array so it stays outside the -G small-data model and is
   reached through %hi/%lo, whose lui covers the store's load delay. */
extern u8 D_800E9ECC[];
/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, and that is what supplies
   the three load-delay nops in the tint copy below. c_symbols.ld overrides
   these common symbols, so no storage is allocated here. */
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

    if ((gFade_State.flags & 0x80) == 0) {
        return;
    }
    if ((D_8009B141 & 0x80) == 0) {
        func_80015CFC();
    }
    delta = p->step * D_8009B0D8;
    lvl = p->level;
    tgt = p->target_level;
    if (lvl != tgt) {
        goto step;
    }
    f = gFade_State.flags;
    gFade_State.flags = f & 0x7F;
    if (lvl == 0xFF) {
        gFade_State.flags = f & 0x79;
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
    if (f & 2) {
        if ((f & 4) == 0) {
            return;
        }
        D_8009B141 = 0x80;
    } else {
        func_80015D0C();
    }
    D_8009B144 = D_8009B14C;
    D_8009B143 = D_8009B14B;
    D_8009B142 = D_8009B14A;
    g = gFade_State.flags;
    if ((g & 0x10) == 0) {
        return;
    }
    if ((g & 0x20) == 0) {
        return;
    }
    gFade_State.flags = g & 0xDF;
    D_8009B144 = p->tint_r;
    D_8009B143 = p->tint_g;
    D_8009B142 = p->tint_b;
    func_80015CFC();
    gFade_State.flags = gFade_State.flags | 0x80;
    return;
step:
    if (gFade_State.flags & 1) {
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
